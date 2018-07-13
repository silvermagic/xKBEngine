#pragma once

#include "common.h"
#include "packet.h"

namespace xKBEngine { namespace network {

class UDPPacket : public Packet
{
public:
	UDPPacket(): Packet(PACKET_MAX_SIZE_UDP)
	{
		data_resize(PACKET_MAX_SIZE_UDP);
		wpos(0);
	}
	virtual ~UDPPacket() {}
};

}
}

