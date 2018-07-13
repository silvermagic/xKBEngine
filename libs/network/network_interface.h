#pragma once

#include <map>
#include <memory>
#include <string>
#include "endpoint.h"

namespace xKBEngine { namespace network {

class Channel;
class ChannelDeregisterHandler;
class EndPoint;
class EventPoller;
class ListenerReceiver;
class MessageHandler;
class NetworkInterface
{
public:
	NetworkInterface(std::shared_ptr<EventPoller> pPoller);
	~NetworkInterface();

	/**
	    获取网络接口上的通道列表
	*/
	const std::map<Address, std::shared_ptr<Channel>>& channels() { return channelMap_; }

	/**
	    关闭网络接口
	*/
	void closeSocket();

	/**
	    移除网络接口上所有的通道
	*/
	void deregisterAllChannels();

	/**
	    移除网络接口上的对应通道
	*/
	bool deregisterChannel(std::shared_ptr<Channel> pChannel);

	/**
	    根据地址查询通道
	*/
	std::shared_ptr<Channel> findChannel(const Address &addr);

	/**
	    根据套接字查询通道
	*/
	std::shared_ptr<Channel> findChannel(int fd);

	/**
	    网络接口初始化，创建监听套接字，可选操作，如果不需要监听套接字可以不调用
	*/
	bool initialize(const char * addr, uint16_t port);

	/**
	    属性存取
	*/
	std::shared_ptr<ChannelDeregisterHandler> pChannelDeregisterHandler() const { return pChannelDeregisterHandler_; }
	void pChannelDeregisterHandler(std::shared_ptr<ChannelDeregisterHandler> pHandler) { pChannelDeregisterHandler_ = pHandler; }
	std::shared_ptr<EndPoint> pEndPoint() const { return pEndPoint_; }
	void pEndPoint(std::shared_ptr<EndPoint> pEndPoint) { pEndPoint_ = pEndPoint; }
	std::shared_ptr<EventPoller> pPoller() const { return pPoller_; }
	void pPoller(std::shared_ptr<EventPoller> pPoller) { pPoller_ = pPoller; }

	/**
	    处理网络接口上注册的所有通道
	*/
	void processChannels(std::shared_ptr<MessageHandler> pMsgHandler);

	/**
	    注册通道到网络接口
	*/
	bool registerChannel(std::shared_ptr<Channel> pChannel);

protected:
	std::shared_ptr<EndPoint> pEndPoint_;
	std::shared_ptr<ChannelDeregisterHandler> pChannelDeregisterHandler_;
	std::shared_ptr<ListenerReceiver> pListenerReceiver_;
	std::shared_ptr<EventPoller> pPoller_;
	std::map<Address, std::shared_ptr<Channel>> channelMap_;
};

}
}