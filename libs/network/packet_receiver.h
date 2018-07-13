#pragma once

#include <vector>
#include <memory>
#include "common.h"
#include "interfaces.h"

namespace xKBEngine { namespace network {

class Packet;
class Channel;
class PacketReceiver : public InputNotificationHandler
{
public:
	PacketReceiver(std::shared_ptr<Channel> pChannel);
	virtual ~PacketReceiver();

	/**
	    数据接收处理
	*/
	virtual int handleInputNotification(int fd);

	/**
	    套接字读取处理
	*/
	virtual bool processRecv() = 0;

	/**
	    接收类型
	*/
	virtual PACKET_RECEIVER_TYPE type() const
	{
		return TCP_PACKET_RECEIVER;
	}

protected:
	std::shared_ptr<Channel> pChannel_; // 所属通道
};

}
}