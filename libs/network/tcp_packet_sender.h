#pragma once

#include "packet_sender.h"

namespace xKBEngine { namespace network {

class TCPPacketSender : public PacketSender
{
public:
	TCPPacketSender(std::shared_ptr<Channel> pChannel);
	~TCPPacketSender();

	/**
	    套接字读取处理
	*/
	virtual bool processSend(bool isEpoll);

protected:
	/**
	    发送数据包
	*/
	virtual Reason doSend(std::shared_ptr<Packet> pPacket);

protected:
	uint8_t sendfailCount_;
};

}
}

