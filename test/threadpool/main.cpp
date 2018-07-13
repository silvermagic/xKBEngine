#include <boost/log/trivial.hpp>
#include "common/timer.h"
#include "threadpool/threadpool.h"

using namespace xKBEngine;
using namespace xKBEngine::threadpool;

class MyTask : public Task
{
public:
	MyTask(uint32_t sleep) :
		sleep_(sleep)
	{
	}

	virtual bool process()
	{
		for (size_t i = 0; i < 3; i++)
		{
			std::this_thread::sleep_for(TIME_INVL(sleep_));
			BOOST_LOG_TRIVIAL(info) << "Sleep " << sleep_.count() << "s.";
		}
		return true;
	}

protected:
	TIME_S sleep_;
};

int main()
{
	ThreadPool::timeout = 5;
	ThreadPool tp(2, 1, 4);
	tp.initialize();

	std::shared_ptr<Task> pTaskA = std::make_shared<MyTask>(1);
	std::shared_ptr<Task> pTaskB = std::make_shared<MyTask>(3);
	std::shared_ptr<Task> pTaskC = std::make_shared<MyTask>(4);
	std::shared_ptr<Task> pTaskD = std::make_shared<MyTask>(7);
	tp.execute(pTaskA);
	tp.execute(pTaskB);
	tp.execute(pTaskC);
	tp.execute(pTaskD);

	size_t t = 0;
	while (t++ < 3)
	{
		std::this_thread::sleep_for(TIME_INVL(10_s));
		tp.recycle();
	}
	tp.destroy();
	return 0;
}