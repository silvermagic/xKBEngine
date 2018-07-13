#include <arpa/inet.h>
#include <boost/log/trivial.hpp>
#include "network/packet.h"
#include "network/interfaces.h"
#include "network/event_poller.h"
#include "network/message_handler.h"
#include "network/network_interface.h"
#include "network/packet_receiver.h"

using namespace xKBEngine::network;

class MyMessageHandler : public MessageHandler, public ChannelDeregisterHandler
{
protected:
	enum MSG
	{
		LOGIN
	};

public:
	MyMessageHandler() :
		pPacket(NULL)
	{
		pPacket = std::make_shared<Packet>(128);
	}

	virtual ~MyMessageHandler()
	{
		pPacket = NULL;
	}

	virtual void handle(std::shared_ptr<Channel> pChannel, Packet &pkg)
	{
		uint32_t id;
		pkg >> id;
		std::string name;
		pkg >> name;

		switch (id)
		{
		case LOGIN:
		{
			if (pChannel->protocoltype() != PROTOCOL_UDP)
			{
				BOOST_LOG_TRIVIAL(warning) << "Wrong channel type.";
				return;
			}
			BOOST_LOG_TRIVIAL(info) << "User login: " << name;
			pPacket->clear();
			(*pPacket) << "Welcome";
			pChannel->pEndPoint()->sendto(pPacket);
			break;
		}
		default:
			break;
		}
	}

	virtual void onChannelDeregister(std::shared_ptr<Channel> pChannel)
	{
		BOOST_LOG_TRIVIAL(info) << "channel(" << pChannel->c_str() << ") deregister.";
	}

protected:
	std::shared_ptr<Packet> pPacket;
};

int main()
{
	std::shared_ptr<EventPoller> pPoller = std::make_shared<EventPoller>();
	NetworkInterface networkInterface(pPoller);

	// 创建UDP套接字
	Address address("127.0.0.1", htons(8080));
	std::shared_ptr<EndPoint> pEndPoint = std::make_shared<EndPoint>(address);
	pEndPoint->socket(SOCK_DGRAM);
	if (!pEndPoint->good())
	{
		BOOST_LOG_TRIVIAL(error) << "Socket create failed!";
		return false;
	}
	networkInterface.pEndPoint(pEndPoint);
	// 这个pChannel就相当于TCP的ListenerReceiver
	std::shared_ptr<Channel> pChannel = std::make_shared<Channel>(networkInterface, pEndPoint, PROTOCOL_UDP);
	if (!pChannel->initialize())
	{
		BOOST_LOG_TRIVIAL(error) << "Channel initialize failed!";
		return -1;
	}
	// 绑定UDP套接字
	if (-1 == pEndPoint->bind())
	{
		BOOST_LOG_TRIVIAL(error) << "Bind the socket to "
			<< pEndPoint->c_str()
			<< " ("
			<< strerror(errno)
			<< ").";
	}
	pEndPoint->setnonblocking();
	pEndPoint->setnodelay();

	// 消息循环
	std::shared_ptr<MessageHandler> pMsgHandler = std::make_shared<MyMessageHandler>();
	networkInterface.pChannelDeregisterHandler(std::dynamic_pointer_cast<ChannelDeregisterHandler>(pMsgHandler));
	while (true)
	{
		pPoller->processPendingEvents(-1);
		if (networkInterface.pEndPoint()->good())
			networkInterface.processChannels(pMsgHandler);
		else
		{
			BOOST_LOG_TRIVIAL(error) << "Socket Close!";
			break;
		}
	}
	return 0;
}