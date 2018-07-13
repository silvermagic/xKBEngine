#include <iostream>
#include <cstring>
#include <thread>
#include "common/timer.h"

using namespace xKBEngine;

class Echo : public TimerHandler
{
public:
	virtual void handleTimeout(TimerHandle handle, void * pUser)
	{
		std::cout << "Echo" << std::endl;
	}
};

class XEcho : public TimerHandler
{
public:
	virtual void handleTimeout(TimerHandle handle, void * pUser)
	{
		int *count = reinterpret_cast<int *>(pUser);
		std::cout << "Echo by X " << *count << " times" << std::endl;
		(*count)++;
	}
};

int main()
{
	// Timer Test
	auto now = TIME_NOW;
	auto end = now + TIME_INVL(900_ms);

	std::shared_ptr<Echo> e = std::make_shared<Echo>();
	std::shared_ptr<XEcho> xe = std::make_shared<XEcho>();
	// 间隔2秒(重复)
	std::shared_ptr<Timer> te = std::make_shared<Timer>(now + TIME_INVL(200_ms),
		TIME_INVL(200_ms),
		std::dynamic_pointer_cast<TimerHandler>(e),
		nullptr);
	int count = 0;
	// 间隔5秒(不重复)
	std::shared_ptr<Timer> txe = std::make_shared<Timer>(now + TIME_INVL(500_ms),
		TIME_INVL::zero(),
		std::dynamic_pointer_cast<TimerHandler>(xe),
		&count);
	do
	{
		if (!te->isCancelled())
		{
			if (te->time() <= now)
			{
				te->triggerTimer();
				// 不重复，则取消定时任务
				if (te->interval() == TIME_INVL::zero())
					te->cancel();
			}
		}

		if (!txe->isCancelled())
		{
			if (txe->time() <= now)
			{
				txe->triggerTimer();
				if (txe->interval() == TIME_INVL::zero())
					txe->cancel();
			}
		}

		std::this_thread::sleep_for(100_ms);
		now = TIME_NOW;
	} while (now < end);
	// 减少定时任务内部引用计数，防止析构抛出异常
	te->cancel();
	txe->cancel();
    return 0;
}