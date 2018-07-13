#include <sys/socket.h>
#include <boost/log/trivial.hpp>
#include "address.h"
#include "channel.h"
#include "endpoint.h"
#include "network_interface.h"
#include "udp_packet.h"
#include "udp_packet_receiver.h"

namespace xKBEngine { namespace network {

UDPPacketReceiver::UDPPacketReceiver(std::shared_ptr<Channel> pChannel):
	PacketReceiver(pChannel)
{
}

UDPPacketReceiver::~UDPPacketReceiver()
{
}

// UDP需要根据接收端来建立通道，并将接收到的消息放入对应通道中
bool UDPPacketReceiver::processRecv()
{
	if (pChannel_->isCondemn())
		return false;

	std::shared_ptr<Packet> pPacket = std::make_shared<UDPPacket>();
	Address address;
	int len = pChannel_->pEndPoint()->recvfrom(pPacket, address);
	if (len < 0)
	{
		if (errno != EAGAIN)
		{
			pChannel_->condemn();
		}

		return false;
	}
	else if (len == 0)
	{
		pChannel_->condemn();
		return false;
	}

	std::shared_ptr<EndPoint> pEndPoint = std::make_shared<EndPoint>(address);
	pEndPoint->socket(SOCK_DGRAM);
	if (!pEndPoint->good())
	{
		BOOST_LOG_TRIVIAL(error) << "UDPPacketReceiver::processRecv: create socket is failed!";
		return false;
	}

	std::shared_ptr<Channel> pSrcChannel = std::make_shared<Channel>(pChannel_->networkInterface(), pEndPoint, PROTOCOL_UDP);
	if (!pSrcChannel->initialize(false))
	{
		BOOST_LOG_TRIVIAL(error) << "UDPPacketReceiver::processRecv: initialize("
			<< pSrcChannel->c_str()
			<< ") is failed!";
		return false;
	}

	if (!pChannel_->networkInterface().registerChannel(pSrcChannel))
	{
		BOOST_LOG_TRIVIAL(error) << "UDPPacketReceiver::processRecv: registerChannel("
			<< pSrcChannel->c_str()
			<< ") is failed!";
		return false;
	}

	if (pSrcChannel->isCondemn())
	{
		return false;
	}

	pSrcChannel->onPacketReceived(len);
	pSrcChannel->addReceiveWindow(pPacket);

	return true;
}

}
}