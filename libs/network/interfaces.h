#pragma once

#include <memory>

namespace xKBEngine { namespace network {

class Channel;
/**
    ����ӿ����ڽ�����ͨ��Network������Ϣ
*/
class InputNotificationHandler
{
public:
	virtual ~InputNotificationHandler() {};
	virtual int handleInputNotification(int fd) = 0;
};

/**
    ����ӿ����ڽ�����ͨ��Network�����Ϣ
*/
class OutputNotificationHandler
{
public:
	virtual ~OutputNotificationHandler() {};
	virtual int handleOutputNotification(int fd) = 0;
};

/**
    ����ӿ����ڽ���һ������ͨ����ʱ��Ϣ
*/
class ChannelTimeOutHandler
{
public:
	virtual void onChannelTimeOut(Channel * pChannel) = 0;
};

/**
    ����ӿ����ڽ���һ���ڲ�����ͨ��ȡ��ע��
*/
class ChannelDeregisterHandler
{
public:
	virtual void onChannelDeregister(std::shared_ptr<Channel> pChannel) = 0;
};

}
}