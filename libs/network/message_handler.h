#pragma once

#include <memory>
#include <boost/log/trivial.hpp>
#include "packet.h"
#include "channel.h"

namespace xKBEngine { namespace network {

class MessageHandler
{
public:
	virtual void handle(std::shared_ptr<Channel> pChannel, Packet &pkg)
	{
		BOOST_LOG_TRIVIAL(info) << "MessageHandler::handle("
			<< pChannel->c_str()
			<< "):" << pkg.toString(true);
		std::shared_ptr<Packet> pPacket = std::make_shared<Packet>();
		(*pPacket) << "Hello";
		pChannel->send(pPacket);
	}
};

}
}