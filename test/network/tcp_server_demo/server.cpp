#include <arpa/inet.h>
#include <boost/log/trivial.hpp>
#include "network/packet.h"
#include "network/interfaces.h"
#include "network/event_poller.h"
#include "network/message_handler.h"
#include "network/network_interface.h"

using namespace xKBEngine::network;

class MyMessageHandler : public MessageHandler, public ChannelDeregisterHandler
{
protected:
	enum MSG
	{
		LOGIN = 0,
		LOGOUT
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
		std::string number;
		pkg >> number;

		switch (id)
		{
		case LOGIN:
		{
			BOOST_LOG_TRIVIAL(info) << "User login: " << number;
			pPacket->clear();
			(*pPacket) << "Welcome";
			pChannel->send(pPacket);
			break;
		}
		case LOGOUT:
		{
			BOOST_LOG_TRIVIAL(info) << "User logout: " << number;
			pPacket->clear();
			(*pPacket) << "Godbye";
			pChannel->send(pPacket);
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
	if (!networkInterface.initialize("0.0.0.0", htons(8080)))
	{
		BOOST_LOG_TRIVIAL(error) << "initialize failed!";
		return -1;
	}

	std::shared_ptr<MessageHandler> pMsgHandler = std::make_shared<MyMessageHandler>();
	networkInterface.pChannelDeregisterHandler(std::dynamic_pointer_cast<ChannelDeregisterHandler>(pMsgHandler));
	while (true)
	{
		pPoller->processPendingEvents(-1);
		if (networkInterface.pEndPoint()->good())
			networkInterface.processChannels(pMsgHandler);
		else
		{
			BOOST_LOG_TRIVIAL(error) << "Accept Close!";
			break;
		}
	}
	return 0;
}