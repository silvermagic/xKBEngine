#pragma once

#include <map>
#include <memory>
#include "common/timer.h"

namespace xKBEngine { namespace network {

class InputNotificationHandler;
class OutputNotificationHandler;
class EventPoller
{
public:
	EventPoller();
	virtual ~EventPoller();

	/**
	    ���epoll�ۻ�ִ��ʱ��
	*/
	void clearSpareTime() { spareTime_ = TIME_INVL::zero(); }

	/**
	    ������������epoll���Ƴ�
	*/
	bool deregisterForRead(int fd);

	/**
	    ��д��������epoll���Ƴ�
	*/
	bool deregisterForWrite(int fd);

	/**
	    ��ȡ����������Ӧ�Ĵ�����
	*/
	std::shared_ptr<InputNotificationHandler> findForRead(int fd);

	/**
	    ��ȡд��������Ӧ�Ĵ�����
	*/
	std::shared_ptr<OutputNotificationHandler> findForWrite(int fd);


	//virtual int getFileDescriptor() const;
	/**
	    ��ȡд��������Ӧ�Ĵ�����
	*/
	virtual int processPendingEvents(int maxWaitInMilliseconds);

	/**
	    ����������ע�ᵽepoll�У����󶨶�Ӧ������
	*/
	bool registerForRead(int fd, std::shared_ptr<InputNotificationHandler> handler);

	/**
	    ��д������ע�ᵽepoll�У����󶨶�Ӧ������
	*/
	bool registerForWrite(int fd, std::shared_ptr<OutputNotificationHandler> handler);

	/**
	    ��ȡepoll�ۻ�ִ��ʱ��
	*/
	TIME_INVL spareTime() const { return spareTime_; }

protected:
	/**
	    ����д������ע�ᵽepoll�л����Ƴ�
	*/
	bool doRegister(int fd, bool isRead, bool isRegister);

	/**
	    ����д�������Ƿ��Ѿ�ע��
	*/
	bool isRegistered(int fd, bool isForRead) const;

	/**
	    �����������Ķ�����
	*/
	bool triggerRead(int fd);

	/**
	    ������������д����
	*/
	bool triggerWrite(int fd);

	/**
	    �����������Ĵ�����
	*/
	bool triggerError(int fd);

protected:
	int epfd_; // epoll������
	TIME_INVL spareTime_; // epoll�ۻ�ִ��ʱ��
	std::map<int, std::shared_ptr<InputNotificationHandler>> fdReadHandlers_; // ���������������б�
	std::map<int, std::shared_ptr<OutputNotificationHandler>> fdWriteHandlers_; // д�������������б�
};

}
}