#include "timer.h"

namespace xKBEngine {

Timer::Timer(TIME_STAMP startTime, TIME_INVL interval, std::shared_ptr<TimerHandler> pHandler, void* pUserData):
	time_(startTime),
	interval_(interval),
	pHandler_(pHandler),
	pUserData_(pUserData),
	state_(TIME_PENDING)
{
	pHandler->incTimerRegisterCount();
}

void Timer::cancel()
{
	if (isCancelled())
		return;

	assert((state_ == TIME_PENDING) || (state_ == TIME_EXECUTING));
	state_ = TIME_CANCELLED;

	if (pHandler_)
	{
		pHandler_->release(TimerHandle(shared_from_this()), pUserData_);
		pHandler_ = NULL;
	}
}

void Timer::triggerTimer()
{
	if (!isCancelled())
	{
		state_ = TIME_EXECUTING;

		pHandler_->handleTimeout(TimerHandle(shared_from_this()), pUserData_);

		if ((interval_ == TIME_INVL(0)) && !isCancelled())
		{
			cancel();
		}
	}

	// 定时任务可能在handleTimeout中被取消了，所以这边要判断下
	if (!isCancelled())
	{
		time_ += interval_;
		state_ = TIME_PENDING;
	}
}

void TimerHandle::cancel()
{
	if (pTimer_ != NULL)
	{
		pTimer_->cancel();
		pTimer_ = NULL;
	}
}

}