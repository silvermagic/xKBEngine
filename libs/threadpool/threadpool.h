#pragma once

#include <cstdint>
#include <memory>
#include <queue>
#include <mutex>
#include <list>
#include <thread>
#include <atomic>
#include <condition_variable>
#include "common/singleton.h"
#include "common/task.h"

namespace xKBEngine { namespace threadpool {

class ThreadPool : public Singleton<ThreadPool>
{
public:
	/**
	   @param initial	: 线程池会一直保持这么多个数的线程
	   @param max		: 线程池最多只能有这么多个线程
	   @param grow		: 当系统繁忙时线程池会新增加这么多临时线程
	*/
	ThreadPool(uint32_t initial, uint32_t grow, uint32_t max);
	~ThreadPool();

	/**
	    添加任务到线程池
	*/
	bool execute(std::shared_ptr<Task> t);

	/**
	    销毁线程池
	*/
	void destroy();

	/**
	    线程池初始化，创建初始线程
	*/
	bool initialize();

	/**
	    线程池是否为销毁状态
	*/
	bool isDestroyed() const { return isDestroyed_.load(); }

	/**
	    线程池是否已经初始化
	*/
	bool isInitialize(void) const { return isInitialize_.load(); }

	/**
	    回收临时线程
	*/
	void recycle();

protected:
	/**
	    从任务列表取出一个任务,并从列表中删除
	*/
	std::shared_ptr<Task> fetchTask();

	/**
	    线程池默认线程处理函数
	*/
	void worker(uint32_t idle);

public:
	static uint32_t timeout; // 临时线程最大空闲时间

protected:
	std::condition_variable cv_recycle_; // 线程池回收信号
	std::condition_variable cv_task_; // 空闲线程恢复信号
	uint32_t grow_; // 每次新增的临时线程数目
	uint32_t idles_; // 空闲线程数目
	uint32_t initial_; // 线程池初始线程数目
	std::atomic<bool> isDestroyed_; // 线程池是否为销毁状态
	std::atomic<bool> isInitialize_; // 线程池是否已经初始化
	uint32_t max_; // 线程池最大线程数目 = 初始 + 临时
	std::list<std::thread::id> recycle_; // 待回收线程列表
	std::mutex stats_lock_; // 空闲线程统计锁
	std::mutex task_lock_; // 任务列表访问锁
	std::queue<std::shared_ptr<Task>> tasks_; // 任务列表
	std::mutex thread_lock_; // 线程列表访问锁
	std::list<std::thread> threads_; // 线程列表
};

}
}