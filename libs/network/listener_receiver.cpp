#include <boost/log/trivial.hpp>
#include "endpoint.h"
#include "channel.h"
#include "listener_receiver.h"

namespace xKBEngine { namespace network {

ListenerReceiver::ListenerReceiver(std::shared_ptr<EndPoint> pEndpoint, NetworkInterface &networkInterface):
	pEndpoint_(pEndpoint),
	networkInterface_(networkInterface)
{
}

ListenerReceiver::~ListenerReceiver()
{
	pEndpoint_ = NULL;
}

int ListenerReceiver::handleInputNotification(int fd)
{
	while (true)
	{
		std::shared_ptr<EndPoint> pNewEndPoint = pEndpoint_->accept();
		if (pNewEndPoint == NULL)
		{
			// EAGAIN/EWOULDBLOCK表明我们已经处理完所有连接
			if ((errno != EAGAIN) && (errno != EWOULDBLOCK))
			{ 
				BOOST_LOG_TRIVIAL(error) << "ListenerReceiver::handleInputNotification: accept endpoint("
					<< fd
					<< ") "
					<< strerror(errno)
					<< "! channelSize="
					<< networkInterface_.channels().size();
				networkInterface_.closeSocket();
			}

			break;
		}
		else
		{
			std::shared_ptr<Channel> pNewChannel = std::make_shared<Channel>(networkInterface_, pNewEndPoint);
			if (!pNewChannel->initialize())
			{
				BOOST_LOG_TRIVIAL(error) << "ListenerReceiver::handleInputNotification: initialize("
					<< pNewChannel->c_str()
					<< ") is failed!";
				continue;
			}

			if (!networkInterface_.registerChannel(pNewChannel))
			{
				BOOST_LOG_TRIVIAL(error) << "ListenerReceiver::handleInputNotification: registerChannel("
					<< pNewChannel->c_str()
					<< ") is failed!";
				continue;
			}

			BOOST_LOG_TRIVIAL(debug) << "ListenerReceiver::handleInputNotification: accept channel("
				<< pNewChannel->c_str()
				<< ")";
		}
	}

	return 0;
}

}
}