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
	    ��ȡ����ӿ��ϵ�ͨ���б�
	*/
	const std::map<Address, std::shared_ptr<Channel>>& channels() { return channelMap_; }

	/**
	    �ر�����ӿ�
	*/
	void closeSocket();

	/**
	    �Ƴ�����ӿ������е�ͨ��
	*/
	void deregisterAllChannels();

	/**
	    �Ƴ�����ӿ��ϵĶ�Ӧͨ��
	*/
	bool deregisterChannel(std::shared_ptr<Channel> pChannel);

	/**
	    ���ݵ�ַ��ѯͨ��
	*/
	std::shared_ptr<Channel> findChannel(const Address &addr);

	/**
	    �����׽��ֲ�ѯͨ��
	*/
	std::shared_ptr<Channel> findChannel(int fd);

	/**
	    ����ӿڳ�ʼ�������������׽��֣���ѡ�������������Ҫ�����׽��ֿ��Բ�����
	*/
	bool initialize(const char * addr, uint16_t port);

	/**
	    ���Դ�ȡ
	*/
	std::shared_ptr<ChannelDeregisterHandler> pChannelDeregisterHandler() const { return pChannelDeregisterHandler_; }
	void pChannelDeregisterHandler(std::shared_ptr<ChannelDeregisterHandler> pHandler) { pChannelDeregisterHandler_ = pHandler; }
	std::shared_ptr<EndPoint> pEndPoint() const { return pEndPoint_; }
	void pEndPoint(std::shared_ptr<EndPoint> pEndPoint) { pEndPoint_ = pEndPoint; }
	std::shared_ptr<EventPoller> pPoller() const { return pPoller_; }
	void pPoller(std::shared_ptr<EventPoller> pPoller) { pPoller_ = pPoller; }

	/**
	    ��������ӿ���ע�������ͨ��
	*/
	void processChannels(std::shared_ptr<MessageHandler> pMsgHandler);

	/**
	    ע��ͨ��������ӿ�
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