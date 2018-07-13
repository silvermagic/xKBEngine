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
	    套接字读取处理
	*/
	virtual bool processRecv();
};

}
}