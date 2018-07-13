#include <cassert>
#include <boost/log/trivial.hpp>
#include <sys/socket.h>
#include "channel.h"
#include "event_poller.h"
#include "endpoint.h"
#include "listener_receiver.h"
#include "network_interface.h"

namespace xKBEngine { namespace network {

extern uint32_t g_SOMAXCONN;

NetworkInterface::NetworkInterface(std::shared_ptr<EventPoller> pPoller):
	pEndPoint_(NULL),
	pListenerReceiver_(NULL),
	pPoller_(pPoller)
{
	channelMap_.clear();
}


NetworkInterface::~NetworkInterface()
{
	channelMap_.clear();
	closeSocket();
	pEndPoint_ = NULL;
	pListenerReceiver_ = NULL;
	pPoller_ = NULL;
}

void NetworkInterface::closeSocket()
{
	if (pEndPoint_->good())
	{
		pPoller_->deregisterForRead(*pEndPoint_);
		pEndPoint_->close();
	}
}

void NetworkInterface::deregisterAllChannels()
{
	for (auto iter : channelMap_)
	{
		iter.second->destroy();
	}
	channelMap_.clear();
}

bool NetworkInterface::deregisterChannel(std::shared_ptr<Channel> pChannel)
{
	assert(pChannel->pEndPoint() != NULL);

	if (!channelMap_.erase(pChannel->pEndPoint()->address()))
	{
		return false;
	}

	if (pChannelDeregisterHandler_)
	{
		pChannelDeregisterHandler_->onChannelDeregister(pChannel);
	}

	return true;
}

std::shared_ptr<Channel> NetworkInterface::findChannel(const Address &addr)
{
	if (addr.ip == 0)
		return NULL;

	auto iter = channelMap_.find(addr);
	if (iter == channelMap_.end())
	{
		return NULL;
	}
	else
	{
		return iter->second;
	}
}

std::shared_ptr<Channel> NetworkInterface::findChannel(int fd)
{
	for (auto item : channelMap_)
	{
		if (item.second->pEndPoint() && *(item.second->pEndPoint()) == fd)
			return item.second;
	}

	return NULL;
}

bool NetworkInterface::initialize(const char * addr, uint16_t port)
{
	if (pEndPoint_)
	{
		if (pEndPoint_->good())
		{
			pPoller_->deregisterForRead(*pEndPoint_);
			pEndPoint_->close();
		}
	}
	else
	{
		
		pEndPoint_ = std::make_shared<EndPoint>();
		pEndPoint_->socket(SOCK_STREAM);
		if (!pEndPoint_->good())
		{
			BOOST_LOG_TRIVIAL(error) << "NetworkInterface::initialize(): couldn't create a socket.";
			return false;
		}
	}
	Address address(addr, port);
	pEndPoint_->address(address);
	pListenerReceiver_ = std::make_shared<ListenerReceiver>(pEndPoint_, *this);
	assert(pListenerReceiver_);

	pPoller_->registerForRead(*pEndPoint_, pListenerReceiver_);

	if (-1 == pEndPoint_->bind())
	{
		BOOST_LOG_TRIVIAL(error) << "NetworkInterface::initialize(): Couldn't bind the socket to "
			<< pEndPoint_->c_str()
			<< " ("
			<< strerror(errno)
			<< ").";
	}

	pEndPoint_->setnonblocking();
	pEndPoint_->setnodelay();

	if (-1 == pEndPoint_->listen())
	{
		BOOST_LOG_TRIVIAL(error) << "NetworkInterface::initialize(): listen to "
			<< pEndPoint_->c_str()
			<< " ("
			<< strerror(errno)
			<< ").";
		return false;
	}

	BOOST_LOG_TRIVIAL(info) << "NetworkInterface::initialize(): address("
		<< pEndPoint_->c_str()
		<< "/TCP/"
		<< *(pEndPoint_)
		<< "), SOMAXCONN="
		<< g_SOMAXCONN
		<< ".";

	return true;
}

void NetworkInterface::processChannels(std::shared_ptr<MessageHandler> pMsgHandler)
{
	auto iter = channelMap_.begin();
	for (;iter != channelMap_.end();)
	{
		std::shared_ptr<Channel> pChannel = iter->second;
		++iter;
		if (pChannel->isDestroyed() || pChannel->isCondemn())
		{
			deregisterChannel(pChannel);
		}
		else
			pChannel->processPackets(pMsgHandler);
	}
}

bool NetworkInterface::registerChannel(std::shared_ptr<Channel> pChannel)
{
	assert(pChannel->pEndPoint()->address().ip != 0);

	auto iter = channelMap_.find(pChannel->pEndPoint()->address());
	if (iter != channelMap_.end())
	{
		BOOST_LOG_TRIVIAL(error) << "NetworkInterface::registerChannel: channel "
			<< pChannel->c_str()
			<<" is exist.";
		return false;
	}

	BOOST_LOG_TRIVIAL(debug) << "NetworkInterface::registerChannel: channel "
		<< pChannel->c_str();
	channelMap_[pChannel->pEndPoint()->address()] = pChannel;
	return true;
}

}
}