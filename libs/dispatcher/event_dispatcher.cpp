#include <cassert>
#include <boost/log/trivial.hpp>
#include "network/interfaces.h"
#include "network/event_poller.h"
#include "tasks.h"
#include "timers.h"
#include "event_dispatcher.h"

namespace xKBEngine { namespace dispatcher {

EventDispatcher::EventDispatcher():
	breakProcessing_(EVENT_DISPATCHER_STATUS_RUNNING),
	numTimerCalls_(0),
	pTasks_(NULL),
	pTimers_(NULL),
	pPoller_(NULL)
{
}

EventDispatcher::~EventDispatcher()
{
	numTimerCalls_ = 0;
	pTasks_ = NULL;
	pPoller_ = NULL;
	pTimers_->clear();
	pTimers_ = NULL;
}

TimerHandle EventDispatcher::addTimer(uint64_t microseconds, std::shared_ptr<TimerHandler> handler, void* arg, bool repeat)
{
	assert(handler);

	if (microseconds <= 0)
		return TimerHandle();

	return pTimers_->add(TIME_NOW + TIME_INVL(TIME_MS(microseconds)),
		(repeat ? TIME_INVL(TIME_MS(microseconds)) : TIME_INVL::zero()), handler, arg);
}

void EventDispatcher::breakProcessing(bool breakState)
{
	if (breakState)
		breakProcessing_ = EVENT_DISPATCHER_STATUS_BREAK_PROCESSING;
	else
		breakProcessing_ = EVENT_DISPATCHER_STATUS_RUNNING;
}

void EventDispatcher::clearSpareTime()
{
	pPoller_->clearSpareTime();
}

bool EventDispatcher::deregisterReadFileDescriptor(int fd)
{
	return pPoller_->deregisterForRead(fd);
}

bool EventDispatcher::deregisterWriteFileDescriptor(int fd)
{
	return pPoller_->deregisterForWrite(fd);
}

TIME_INVL EventDispatcher::getSpareTime() const
{
	return pPoller_->spareTime();
}

bool EventDispatcher::initialize()
{
	if (pPoller_ == NULL)
		pPoller_ = std::make_shared<network::EventPoller>();

	if (pTasks_ == NULL)
		pTasks_ = std::make_shared<Tasks>();

	if (pTimers_ == NULL)
		pTimers_ = std::make_shared<Timers>();

	BOOST_LOG_TRIVIAL(info) << "EventDispatcher::initialize: is successfully!";
	return true;
}

int EventDispatcher::maxWait() const
{
	return maxWait_;
}

void EventDispatcher::maxWait(double seconds)
{
	maxWait_ = int(ceil(seconds * 1000));
}

int EventDispatcher::processNetwork(bool shouldIdle)
{
	int maxWait = shouldIdle ? calculateWait() : -1;
	return pPoller_->processPendingEvents(maxWait);
}

int EventDispatcher::processOnce(bool shouldIdle)
{
	if (breakProcessing_ != EVENT_DISPATCHER_STATUS_BREAK_PROCESSING)
		breakProcessing_ = EVENT_DISPATCHER_STATUS_RUNNING;

	processTasks();

	if (breakProcessing_ != EVENT_DISPATCHER_STATUS_BREAK_PROCESSING) {
		processTimers();
	}

	if (breakProcessing_ != EVENT_DISPATCHER_STATUS_BREAK_PROCESSING) {
		return processNetwork(shouldIdle);
	}

	return 0;
}

void EventDispatcher::processUntilBreak()
{
	if (breakProcessing_ != EVENT_DISPATCHER_STATUS_BREAK_PROCESSING)
		breakProcessing_ = EVENT_DISPATCHER_STATUS_RUNNING;

	while (breakProcessing_ != EVENT_DISPATCHER_STATUS_BREAK_PROCESSING)
		processOnce(true);
}

bool EventDispatcher::registerReadFileDescriptor(int fd, std::shared_ptr<network::InputNotificationHandler> handler)
{
	return pPoller_->registerForRead(fd, handler);
}

bool EventDispatcher::registerWriteFileDescriptor(int fd, std::shared_ptr<network::OutputNotificationHandler> handler)
{
	return pPoller_->registerForWrite(fd, handler);
}

void EventDispatcher::setWaitBreakProcessing()
{
	breakProcessing_ = EVENT_DISPATCHER_STATUS_WAITING_BREAK_PROCESSING;
}

bool EventDispatcher::waitingBreakProcessing() const
{
	return breakProcessing_ == EVENT_DISPATCHER_STATUS_WAITING_BREAK_PROCESSING;
}

int EventDispatcher::calculateWait() const
{
	int maxWait = maxWait_;

	if (!pTimers_->empty())
	{
		TIME_INVL nextExp = pTimers_->nextExp(TIME_NOW);
		int wait = nextExp.count();
		maxWait = std::min(maxWait_, wait);
	}

	return maxWait;
}

void EventDispatcher::processTasks()
{
	pTasks_->process();
}

void EventDispatcher::processTimers()
{
	numTimerCalls_ += pTimers_->process(TIME_NOW);
}

}
}