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
	    清除epoll累积执行时间
	*/
	void clearSpareTime() { spareTime_ = TIME_INVL::zero(); }

	/**
	    将读描述符从epoll中移除
	*/
	bool deregisterForRead(int fd);

	/**
	    将写描述符从epoll中移除
	*/
	bool deregisterForWrite(int fd);

	/**
	    获取读描述符对应的处理句柄
	*/
	std::shared_ptr<InputNotificationHandler> findForRead(int fd);

	/**
	    获取写描述符对应的处理句柄
	*/
	std::shared_ptr<OutputNotificationHandler> findForWrite(int fd);


	//virtual int getFileDescriptor() const;
	/**
	    获取写描述符对应的处理句柄
	*/
	virtual int processPendingEvents(int maxWaitInMilliseconds);

	/**
	    将读描述符注册到epoll中，并绑定对应处理句柄
	*/
	bool registerForRead(int fd, std::shared_ptr<InputNotificationHandler> handler);

	/**
	    将写描述符注册到epoll中，并绑定对应处理句柄
	*/
	bool registerForWrite(int fd, std::shared_ptr<OutputNotificationHandler> handler);

	/**
	    获取epoll累积执行时间
	*/
	TIME_INVL spareTime() const { return spareTime_; }

protected:
	/**
	    将读写描述符注册到epoll中或将其移除
	*/
	bool doRegister(int fd, bool isRead, bool isRegister);

	/**
	    检测读写描述符是否已经注册
	*/
	bool isRegistered(int fd, bool isForRead) const;

	/**
	    触发描述符的读处理
	*/
	bool triggerRead(int fd);

	/**
	    触发描述符的写处理
	*/
	bool triggerWrite(int fd);

	/**
	    触发描述符的错误处理
	*/
	bool triggerError(int fd);

protected:
	int epfd_; // epoll描述符
	TIME_INVL spareTime_; // epoll累积执行时间
	std::map<int, std::shared_ptr<InputNotificationHandler>> fdReadHandlers_; // 读描述符处理句柄列表
	std::map<int, std::shared_ptr<OutputNotificationHandler>> fdWriteHandlers_; // 写描述符处理句柄列表
};

}
}