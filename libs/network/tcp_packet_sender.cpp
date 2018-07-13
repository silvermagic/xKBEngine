#include <boost/log/trivial.hpp>
#include "packet.h"
#include "channel.h"
#include "endpoint.h"
#include "tcp_packet_sender.h"

namespace xKBEngine { namespace network {

TCPPacketSender::TCPPacketSender(std::shared_ptr<Channel> pChannel):
	PacketSender(pChannel),
	sendfailCount_(0)
{
}


TCPPacketSender::~TCPPacketSender()
{
}

bool TCPPacketSender::processSend(bool isEpoll)
{
	if (pChannel_->isCondemn())
		return false;

	Reason reason = REASON_SUCCESS;
	std::vector<std::shared_ptr<Packet>>& bundles = pChannel_->bundles();
	size_t i = 0;
	for (; i < bundles.size(); ++i)
	{
		reason = doSend(bundles[i]);
		if (reason != REASON_SUCCESS)
			break;
	}

	if (reason == REASON_SUCCESS)
	{
		bundles.clear();
		sendfailCount_ = 0;
	}
	else
	{
		bundles.erase(bundles.begin(), bundles.begin() + i);
		if (reason == REASON_RESOURCE_UNAVAILABLE)
		{
			BOOST_LOG_TRIVIAL(warning) << "TCPPacketSender::processSend: Transmit queue full, waiting for space";
			if (++sendfailCount_ >= 10)
			{
				pChannel_->condemn();
			}
		}
		else
			pChannel_->condemn();

		return false;
	}

	if (isEpoll)
		pChannel_->onSendCompleted();

	return true;
}

Reason TCPPacketSender::doSend(std::shared_ptr<Packet> pPacket)
{
	if (pChannel_->isCondemn())
	{
		return REASON_CHANNEL_CONDEMN;
	}

	int len = pChannel_->pEndPoint()->send(pPacket);
	bool sentCompleted = pPacket->sentSize == pPacket->length();
	pChannel_->onPacketSent(len, sentCompleted);

	if (sentCompleted)
	{
		return REASON_SUCCESS;
	}
	else
	{
		if (len > 0)
			return REASON_RESOURCE_UNAVAILABLE;
	}

	switch (errno)
	{
	case ECONNREFUSED:	return REASON_NO_SUCH_PORT;
	case EAGAIN:		return REASON_RESOURCE_UNAVAILABLE;
	case EPIPE:			return REASON_CLIENT_DISCONNECTED;
	case ECONNRESET:	return REASON_CLIENT_DISCONNECTED;
	case ENOBUFS:		return REASON_TRANSMIT_QUEUE_FULL;
	}

	return REASON_GENERAL_NETWORK;
}

}
}