#pragma once

#include "common.h"
#include "packet.h"

namespace xKBEngine { namespace network {

class TCPPacket : public Packet
{
public:
	TCPPacket(): Packet(PACKET_MAX_SIZE_TCP)
	{
		data_resize(PACKET_MAX_SIZE_TCP);
		wpos(0);
	}
	virtual ~TCPPacket() {}
};

}
}