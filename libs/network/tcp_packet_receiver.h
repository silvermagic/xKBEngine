#pragma once

#include "packet_receiver.h"

namespace xKBEngine { namespace network {

class Packet;
class Channel;
class TCPPacketReceiver : public PacketReceiver
{
public:
	TCPPacketReceiver(std::shared_ptr<Channel> pChannel);
	virtual ~TCPPacketReceiver();

protected:
	/**
	    �׽��ֶ�ȡ����
	*/
	virtual bool processRecv();
};

}
}