#pragma once

#include "packet_receiver.h"

namespace xKBEngine { namespace network {

class UDPPacketReceiver : public PacketReceiver
{
public:
	UDPPacketReceiver(std::shared_ptr<Channel> pChannel);
	virtual ~UDPPacketReceiver();

	/**
	    接收类型
	*/
	virtual PACKET_RECEIVER_TYPE type() const
	{
		return UDP_PACKET_RECEIVER;
	}

protected:
	/**
	    套接字读取处理
	*/
	virtual bool processRecv();
};

}
}