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
	    ���һ����ʱ������
	*/
	TimerHandle addTimer(uint64_t microseconds, std::shared_ptr<TimerHandler> handler, void* arg = NULL, bool repeat = true);

	/**
	    ֹͣ�ַ�������
	*/
	void breakProcessing(bool breakState = true);

	/**
	    �������ͳ��ʱ��
	*/
	void clearSpareTime();

	/**
	    ������������EPOLL���Ƴ�
	*/
	bool deregisterReadFileDescriptor(int fd);

	/**
	    ��д��������EPOLL���Ƴ�
	*/
	bool deregisterWriteFileDescriptor(int fd);

	/**
	    �������ͳ��ʱ��
	*/
	TIME_INVL getSpareTime() const;

	/**
	    �жϷַ����Ƿ��Ѿ�ֹͣ����
	*/
	bool hasBreakProcessing() const { return breakProcessing_ == EVENT_DISPATCHER_STATUS_BREAK_PROCESSING; }

	/**
	    �ַ�����ʼ��
	*/
	bool initialize();

	/**
	    ��ȡ�ַ���EPOLL���ȴ�ʱ��
	*/
	int maxWait() const;

	/**
	    ���÷ַ���EPOLL���ȴ�ʱ��
	*/
	void maxWait(double seconds);

	/**
	    ���Դ�ȡ
	*/
	void pPoller(std::shared_ptr<network::EventPoller> pPoller) { pPoller_ = pPoller; }
	std::shared_ptr<network::EventPoller> pPoller() { return pPoller_; }

	/**
	    ����������
	*/
	int processNetwork(bool shouldIdle);

	/**
	    ����һ������
	*/
	int  processOnce(bool shouldIdle = false);

	/**
	    ������������ֱ���ַ���ֹͣ����
	*/
	void processUntilBreak();

	/**
	    ����������ע�ᵽepoll�У����󶨶�Ӧ������
	*/
	bool registerReadFileDescriptor(int fd, std::shared_ptr<network::InputNotificationHandler> handler);

	/**
	    ��д������ע�ᵽepoll�У����󶨶�Ӧ������
	*/
	bool registerWriteFileDescriptor(int fd, std::shared_ptr<network::OutputNotificationHandler> handler);

	/**
	    ��Ƿַ�����״̬Ϊ�ȴ��˳�
	*/
	void setWaitBreakProcessing();

	/**
	    �жϷַ����Ƿ��ڵȴ��˳�״̬
	*/
	bool waitingBreakProcessing() const;

protected:
	/**
	    ����EPOLL���ȴ�ʱ��
	*/
	int calculateWait() const;

	/**
	    ��ͨ������
	*/
	void processTasks();

	/**
	    ��ʱ������
	*/
	void processTimers();

protected:
	uint8_t breakProcessing_; // �ַ�������״̬
	int maxWait_; // EPOLL���ȴ�ʱ��
	uint32_t numTimerCalls_; // �Ѵ����Ķ�ʱ��������
	std::shared_ptr<Tasks> pTasks_; // ��ͨ���������
	std::shared_ptr<Timers> pTimers_; // ��ʱ���������
	std::shared_ptr<network::EventPoller> pPoller_; // �������������
};

}
}