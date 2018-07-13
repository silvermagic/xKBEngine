#include "packet_sender.h"

namespace xKBEngine { namespace network {

PacketSender::PacketSender(std::shared_ptr<Channel> pChannel):
	pChannel_(pChannel)
{
}

PacketSender::~PacketSender()
{
	pChannel_ = NULL;
}

int PacketSender::handleOutputNotification(int fd)
{
	processSend(true);
	return 0;
}

}
}