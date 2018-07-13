#pragma once

#include <memory>

namespace xKBEngine { namespace network {

class Channel;
/**
    此类接口用于接收普通的Network输入消息
*/
class InputNotificationHandler
{
public:
	virtual ~InputNotificationHandler() {};
	virtual int handleInputNotification(int fd) = 0;
};

/**
    此类接口用于接收普通的Network输出消息
*/
class OutputNotificationHandler
{
public:
	virtual ~OutputNotificationHandler() {};
	virtual int handleOutputNotification(int fd) = 0;
};

/**
    此类接口用于接收一个网络通道超时消息
*/
class ChannelTimeOutHandler
{
public:
	virtual void onChannelTimeOut(Channel * pChannel) = 0;
};

/**
    此类接口用于接收一个内部网络通道取消注册
*/
class ChannelDeregisterHandler
{
public:
	virtual void onChannelDeregister(std::shared_ptr<Channel> pChannel) = 0;
};

}
}