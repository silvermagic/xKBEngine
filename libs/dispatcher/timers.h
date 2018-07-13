#pragma once

#include "common/timer.h"

namespace xKBEngine { namespace dispatcher {

/**
    ��ʱ��������
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
    ��ʱ���������
*/
class Timers
{
public:
	Timers();
	virtual ~Timers();

	inline size_t size() const { return timeQueue_.size(); }
	inline bool empty() const { return timeQueue_.empty(); }

	// ��Ӷ�ʱ����
	TimerHandle	add(TIME_STAMP startTime, TIME_INVL interval, std::shared_ptr<TimerHandler> pHandler, void * pUser);
	// ��ն�ʱ�������
	void clear();
	// ����ʱ����
	int process(TIME_STAMP now);
	// ��ȡ�´ζ�ʱ���񴥷���ʱ����
	TIME_INVL nextExp(TIME_STAMP now) const;
	// ��Ӧ��ʱ����ȡ��
	virtual void onCancel() {}

protected:
	std::priority_queue<std::shared_ptr<Timer>, std::vector<std::shared_ptr<Timer>>, Comparator> timeQueue_;
};

}
}