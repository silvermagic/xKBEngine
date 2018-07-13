#pragma once

#include <cassert>
#include <queue>
#include <memory>
#include <chrono>

// 时间操作
#define TIME_NOW std::chrono::steady_clock::now()
typedef std::chrono::steady_clock::time_point TIME_STAMP;
typedef std::chrono::steady_clock::duration TIME_INVL;
typedef std::chrono::seconds TIME_S;
typedef std::chrono::milliseconds TIME_MS;
constexpr std::chrono::seconds operator "" _s(unsigned long long s)
{
	return std::chrono::seconds(s);
}
constexpr std::chrono::milliseconds operator "" _ms(unsigned long long ms)
{
	return std::chrono::milliseconds(ms);
}

namespace xKBEngine {

class Timer;

/**
    定时任务句柄
*/
class TimerHandle
{
public:
	explicit TimerHandle(std::shared_ptr<Timer> pTimer = NULL) : pTimer_(pTimer) {}

	bool isSet() const { return pTimer_ != NULL; }
	std::shared_ptr<Timer> timer() const { return pTimer_; }

	void cancel();
	void clearWithoutCancel() { pTimer_ = NULL; }

protected:
	std::shared_ptr<Timer> pTimer_;
};

/**
    定时任务处理接口
*/
class TimerHandler
{
public:
	TimerHandler() : numTimesRegistered_(0) {}
	virtual ~TimerHandler()
	{
		assert(numTimesRegistered_ == 0);
	};

	void release(TimerHandle handle, void * pUser)
	{
		this->decTimerRegisterCount();
		this->onRelease(handle, pUser);
	}

	// 定时任务处理函数
	virtual void handleTimeout(TimerHandle handle, void * pUser) = 0;

protected:
	// 定时任务释放回调函数，主要是对pUser的释放
	virtual void onRelease(TimerHandle handle, void * pUser) {};

	void incTimerRegisterCount() { ++numTimesRegistered_; }
	void decTimerRegisterCount() { --numTimesRegistered_; }

	friend class Timer;

protected:
	int numTimesRegistered_;
};

/**
    定时任务
*/
class Timer : public std::enable_shared_from_this<Timer>
{
public:
	Timer(TIME_STAMP startTime, TIME_INVL interval, std::shared_ptr<TimerHandler> pHandler, void* pUserData);
	virtual ~Timer() {}

	void * getUserData() const { return pUserData_; }
	TIME_STAMP time() const { return time_; }
	TIME_INVL interval() const { return interval_; }
	bool isCancelled() const { return state_ == TIME_CANCELLED; }
	bool isExecuting() const { return state_ == TIME_EXECUTING; }

	// 取消定时任务
	void cancel();
	// 触发定时任务
	void triggerTimer();

protected:
	enum TimeState
	{
		TIME_PENDING,
		TIME_EXECUTING,
		TIME_CANCELLED
	};

	TIME_STAMP time_;
	TIME_INVL interval_;
	std::shared_ptr<TimerHandler> pHandler_;
	void *pUserData_;
	TimeState state_;
};

}