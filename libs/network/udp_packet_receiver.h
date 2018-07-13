#pragma once

#include "packet_receiver.h"

namespace xKBEngine { namespace network {

class UDPPacketReceiver : public PacketReceiver
{
public:
	UDPPacketReceiver(std::shared_ptr<Channel> pChannel);
	virtual ~UDPPacketReceiver();

	/**
	    ��������
	*/
	virtual PACKET_RECEIVER_TYPE type() const
	{
		return UDP_PACKET_RECEIVER;
	}

protected:
	/**
	    �׽��ֶ�ȡ����
	*/
	virtual bool processRecv();
};

}
}