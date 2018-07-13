#include <boost/log/trivial.hpp>
#include "common/timer.h"
#include "threadpool.h"

namespace xKBEngine { namespace threadpool {

KBE_SINGLETON_INIT(ThreadPool)

uint32_t ThreadPool::timeout = 60;

ThreadPool::ThreadPool(uint32_t initial, uint32_t grow, uint32_t max):
	cv_recycle_(),
	cv_task_(),
	grow_(grow),
	idles_(0),
	initial_(initial),
	isDestroyed_(false),
	isInitialize_(false),
	max_(max),
	recycle_(),
	stats_lock_(),
	task_lock_(),
	tasks_(),
	thread_lock_(),
	threads_()
{
}

ThreadPool::~ThreadPool()
{
	if (!isDestroyed_.load())
		destroy();
}

bool ThreadPool::execute(std::shared_ptr<Task> t)
{
	if (!isInitialize_.load())
	{
		BOOST_LOG_TRIVIAL(info) << "ThreadPool::execute(): thread pool has not been initialized yet.";
		return false;
	}

	// 添加任务到线程池
	{
		std::lock_guard<std::mutex> guard(task_lock_);
		tasks_.push(t);
	}

	// 检测线程池是否繁忙
	{
		std::lock_guard<std::mutex> guard(stats_lock_);
		if (idles_ > 0)
		{
			// 让唤醒线程操作idles_
			guard.~lock_guard();
			cv_task_.notify_one();
			std::this_thread::sleep_for(TIME_MS(10));
			return true;
		}
	}

	BOOST_LOG_TRIVIAL(info) << "ThreadPool::execute(): thread pool is busy.";
	// 线程池繁忙则需要添加临时线程
	{
		std::lock_guard<std::mutex> guard(thread_lock_);
		if (threads_.size() >= max_)
		{
			BOOST_LOG_TRIVIAL(warning) << "ThreadPool::execute(): thread pool is full.";
			return false;
		}

		for (size_t i = 0; i < grow_; ++i)
		{
			std::thread t(&ThreadPool::worker, this, ThreadPool::timeout);
			BOOST_LOG_TRIVIAL(debug) << "ThreadPool::execute(): create thread[" << t.get_id() << "].";
			threads_.push_back(std::move(t));
		}
	}

	return true;
}

void ThreadPool::destroy()
{
	BOOST_LOG_TRIVIAL(info) << "ThreadPool::destroy(): ......";
	isDestroyed_ = true;
	{
		cv_task_.notify_all();
		std::unique_lock<std::mutex> guard(thread_lock_);
		cv_recycle_.wait(guard, [&]() { return recycle_.size() == threads_.size(); });
	}
	recycle();
	BOOST_LOG_TRIVIAL(info) << "ThreadPool::destroy(): is successfully!";
}

std::shared_ptr<Task> ThreadPool::fetchTask()
{
	std::lock_guard<std::mutex> guard(task_lock_);
	if (tasks_.empty())
		return nullptr;

	std::shared_ptr<Task> pTask = tasks_.front();
	tasks_.pop();
	return pTask;
}

bool ThreadPool::initialize()
{
	if (isInitialize_.load())
	{
		BOOST_LOG_TRIVIAL(info) << "ThreadPool::initialize(): the thread pool has been initialized.";
		return true;
	}

	{
		std::lock_guard<std::mutex> guard(thread_lock_);
		for (size_t i = 0; i < initial_; ++i)
		{
			std::thread t(&ThreadPool::worker, this, 0);
			BOOST_LOG_TRIVIAL(debug) << "ThreadPool::initialize(): create thread[" << t.get_id() << "].";
			threads_.push_back(std::move(t));
		}
	}
	// 让出主线程的时间片给线程池的线程
	std::this_thread::sleep_for(TIME_MS(10));

	isInitialize_ = true;
	BOOST_LOG_TRIVIAL(info) << "ThreadPool::initialize(): is successfully!";
	return isInitialize_;
}

void ThreadPool::recycle()
{
	std::lock_guard<std::mutex> guard(thread_lock_);
	BOOST_LOG_TRIVIAL(debug) << "ThreadPool::recycle(): before recycling, threads: " << threads_.size() << ", recycle: " << recycle_.size();
	for (auto id : recycle_)
	{
		auto iter = threads_.begin();
		for (; iter != threads_.end(); ++iter)
		{
			if (iter->get_id() == id)
			{
				iter->join();
				break;
			}
		}

		if (iter != threads_.end())
			threads_.erase(iter);
	}
	BOOST_LOG_TRIVIAL(debug) << "ThreadPool::recycle(): after recycling, threads: " << threads_.size();
	recycle_.clear();
}

void ThreadPool::worker(uint32_t idle)
{
	std::thread::id this_id = std::this_thread::get_id();
	BOOST_LOG_TRIVIAL(debug) << "ThreadPool::worker(): thread[" << this_id << "] start.";

	// 线程池是否进入销毁状态
	while (!isDestroyed_.load())
	{
		// 获取一个待处理任务
		std::shared_ptr<Task> pTask = fetchTask();
		if (pTask != nullptr)
		{
			pTask->process();
			pTask = nullptr;
		}
		else
		{
			
			std::unique_lock<std::mutex> guard(stats_lock_);
			++idles_;
			BOOST_LOG_TRIVIAL(debug) << "ThreadPool::worker(): thread[" << this_id << "] idle.";
			// 线程进入休眠状态，等待线程池的任务通知
			if (idle > 0)
			{
				// 线程等待时间大于0表示线程是临时线程，如果超过等待时间依然没有任务，则释放线程
				std::cv_status ret = cv_task_.wait_for(guard, TIME_MS(idle * 1000));
				--idles_;
				if (ret == std::cv_status::timeout)
					break;
			}
			else
			{
				cv_task_.wait(guard);
				--idles_;
			}
			BOOST_LOG_TRIVIAL(debug) << "ThreadPool::worker(): thread[" << this_id << "] wakeup.";
		}
	}

	// 将线程放入待回收队列
	std::lock_guard<std::mutex> guard(thread_lock_);
	recycle_.push_back(this_id);
	cv_recycle_.notify_one();
	BOOST_LOG_TRIVIAL(debug) << "ThreadPool::worker(): thread[" << this_id << "] end.";
}

}
}