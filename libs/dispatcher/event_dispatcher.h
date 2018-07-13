#pragma once

#include <memory>
#include "common/timer.h"

namespace xKBEngine {

namespace network
{
	class EventPoller;
	class InputNotificationHandler;
	class OutputNotificationHandler;
}

namespace dispatcher {

enum EVENT_DISPATCHER_STATUS
{
	EVENT_DISPATCHER_STATUS_RUNNING = 0,
	EVENT_DISPATCHER_STATUS_WAITING_BREAK_PROCESSING = 1,
	EVENT_DISPATCHER_STATUS_BREAK_PROCESSING = 2
};

class Tasks;
class Timers;
class EventDispatcher
{
public:
	EventDispatcher();
	virtual ~EventDispatcher();

	/**
	    添加一个定时器任务
	*/
	TimerHandle addTimer(uint64_t microseconds, std::shared_ptr<TimerHandler> handler, void* arg = NULL, bool repeat = true);

	/**
	    停止分发器运行
	*/
	void breakProcessing(bool breakState = true);

	/**
	    清除空闲统计时间
	*/
	void clearSpareTime();

	/**
	    将读描述符从EPOLL中移除
	*/
	bool deregisterReadFileDescriptor(int fd);

	/**
	    将写描述符从EPOLL中移除
	*/
	bool deregisterWriteFileDescriptor(int fd);

	/**
	    清除空闲统计时间
	*/
	TIME_INVL getSpareTime() const;

	/**
	    判断分发器是否已经停止运行
	*/
	bool hasBreakProcessing() const { return breakProcessing_ == EVENT_DISPATCHER_STATUS_BREAK_PROCESSING; }

	/**
	    分发器初始化
	*/
	bool initialize();

	/**
	    获取分发器EPOLL最大等待时间
	*/
	int maxWait() const;

	/**
	    设置分发器EPOLL最大等待时间
	*/
	void maxWait(double seconds);

	/**
	    属性存取
	*/
	void pPoller(std::shared_ptr<network::EventPoller> pPoller) { pPoller_ = pPoller; }
	std::shared_ptr<network::EventPoller> pPoller() { return pPoller_; }

	/**
	    网络任务处理
	*/
	int processNetwork(bool shouldIdle);

	/**
	    处理一遍任务
	*/
	int  processOnce(bool shouldIdle = false);

	/**
	    持续处理任务直到分发器停止运行
	*/
	void processUntilBreak();

	/**
	    将读描述符注册到epoll中，并绑定对应处理句柄
	*/
	bool registerReadFileDescriptor(int fd, std::shared_ptr<network::InputNotificationHandler> handler);

	/**
	    将写描述符注册到epoll中，并绑定对应处理句柄
	*/
	bool registerWriteFileDescriptor(int fd, std::shared_ptr<network::OutputNotificationHandler> handler);

	/**
	    标记分发器的状态为等待退出
	*/
	void setWaitBreakProcessing();

	/**
	    判断分发器是否处于等待退出状态
	*/
	bool waitingBreakProcessing() const;

protected:
	/**
	    计算EPOLL最大等待时间
	*/
	int calculateWait() const;

	/**
	    普通任务处理
	*/
	void processTasks();

	/**
	    定时任务处理
	*/
	void processTimers();

protected:
	uint8_t breakProcessing_; // 分发器运行状态
	int maxWait_; // EPOLL最大等待时间
	uint32_t numTimerCalls_; // 已触发的定时任务总数
	std::shared_ptr<Tasks> pTasks_; // 普通任务管理器
	std::shared_ptr<Timers> pTimers_; // 定时任务管理器
	std::shared_ptr<network::EventPoller> pPoller_; // 网络任务管理器
};

}
}