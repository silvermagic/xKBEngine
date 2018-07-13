#include <cassert>
#include "endpoint.h"
#include "channel.h"
#include "packet_receiver.h"

namespace xKBEngine { namespace network {

PacketReceiver::PacketReceiver(std::shared_ptr<Channel> pChannel):
	pChannel_(pChannel)
{
}


PacketReceiver::~PacketReceiver()
{
	pChannel_ = NULL;
}

int PacketReceiver::handleInputNotification(int fd)
{
	assert(*(pChannel_->pEndPoint()) == fd);
	while (processRecv())
	{
	}

	return 0;
}

}
}