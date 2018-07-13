#pragma once

#include "common/timer.h"

namespace xKBEngine { namespace dispatcher {

/**
    定时任务排序
*/
class Comparator
{
public:
	bool operator()(const std::shared_ptr<Timer> l, const std::shared_ptr<Timer> r)
	{
		return l->time() > r->time();
	}
};

/**
    定时任务管理器
*/
class Timers
{
public:
	Timers();
	virtual ~Timers();

	inline size_t size() const { return timeQueue_.size(); }
	inline bool empty() const { return timeQueue_.empty(); }

	// 添加定时任务
	TimerHandle	add(TIME_STAMP startTime, TIME_INVL interval, std::shared_ptr<TimerHandler> pHandler, void * pUser);
	// 清空定时任务队列
	void clear();
	// 处理定时任务
	int process(TIME_STAMP now);
	// 获取下次定时任务触发的时间间隔
	TIME_INVL nextExp(TIME_STAMP now) const;
	// 响应定时任务取消
	virtual void onCancel() {}

protected:
	std::priority_queue<std::shared_ptr<Timer>, std::vector<std::shared_ptr<Timer>>, Comparator> timeQueue_;
};

}
}