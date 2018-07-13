#include "timers.h"

namespace xKBEngine { namespace dispatcher {

Timers::Timers():
	timeQueue_()
{

}

Timers::~Timers()
{
	clear();
}

int Timers::process(TIME_STAMP now)
{
	int numFired = 0;
	while ((!timeQueue_.empty()) && (
		timeQueue_.top()->time() <= now ||
		timeQueue_.top()->isCancelled()))
	{
		auto pTimer = timeQueue_.top();
		timeQueue_.pop();

		if (!pTimer->isCancelled())
		{
			++numFired;
			pTimer->triggerTimer();
		}

		if (!pTimer->isCancelled())
		{
			timeQueue_.push(pTimer);
		}

		if (pTimer->isCancelled())
			onCancel();
	}

	return numFired;
}

TIME_INVL Timers::nextExp(TIME_STAMP now) const
{
	if (timeQueue_.empty() || now > timeQueue_.top()->time())
	{
		return TIME_INVL(0);
	}

	return timeQueue_.top()->time() - now;
}

void Timers::clear()
{
	while (!timeQueue_.empty())
	{
		auto pTimer = timeQueue_.top();
		timeQueue_.pop();
		if (!pTimer->isCancelled())
		{
			pTimer->cancel();
		}
	}
}

TimerHandle	Timers::add(TIME_STAMP startTime, TIME_INVL interval, std::shared_ptr<TimerHandler> pHandler, void * pUser)
{
	std::shared_ptr<Timer> pTimer = std::make_shared<Timer>(startTime, interval, pHandler, pUser);
	timeQueue_.push(pTimer);
	return TimerHandle(pTimer);
}

}
}