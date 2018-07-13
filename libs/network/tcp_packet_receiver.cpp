#include <boost/log/trivial.hpp>
#include "channel.h"
#include "endpoint.h"
#include "tcp_packet.h"
#include "tcp_packet_receiver.h"

namespace xKBEngine { namespace network {

TCPPacketReceiver::TCPPacketReceiver(std::shared_ptr<Channel> pChannel):
	PacketReceiver(pChannel)
{
}

TCPPacketReceiver::~TCPPacketReceiver()
{
}

bool TCPPacketReceiver::processRecv()
{
	if (pChannel_->isCondemn())
		return false;

	std::shared_ptr<Packet> pPacket = std::make_shared<TCPPacket>();
	int len = pChannel_->pEndPoint()->recv(pPacket);
	if (len < 0)
	{
		if (errno != EAGAIN)
		{
			BOOST_LOG_TRIVIAL(error) << "TCPPacketReceiver::processRecv() is failed!";
			pChannel_->condemn();
		}

		return false;
	}
	else if (len == 0)
	{
		BOOST_LOG_TRIVIAL(debug) << "TCPPacketReceiver::processRecv(): channel("
			<< pChannel_->c_str()
			<< "), connection has been disconnected!";
		pChannel_->condemn();
		return false;
	}

	pChannel_->onPacketReceived(len);
	pChannel_->addReceiveWindow(pPacket);

	return true;
}

}
}