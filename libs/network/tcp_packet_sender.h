#pragma once

#include "packet_sender.h"

namespace xKBEngine { namespace network {

class TCPPacketSender : public PacketSender
{
public:
	TCPPacketSender(std::shared_ptr<Channel> pChannel);
	~TCPPacketSender();

	/**
	    �׽��ֶ�ȡ����
	*/
	virtual bool processSend(bool isEpoll);

protected:
	/**
	    �������ݰ�
	*/
	virtual Reason doSend(std::shared_ptr<Packet> pPacket);

protected:
	uint8_t sendfailCount_;
};

}
}

