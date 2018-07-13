#pragma once

#include "common.h"
#include "interfaces.h"
#include "network_interface.h"

namespace xKBEngine { namespace network {

class EndPoint;
class ListenerReceiver : public InputNotificationHandler
{
public:
	ListenerReceiver(std::shared_ptr<EndPoint> pEndpoint, NetworkInterface &networkInterface);
	~ListenerReceiver();

protected:
	virtual int handleInputNotification(int fd);

protected:
	std::shared_ptr<EndPoint> pEndpoint_;
	NetworkInterface &networkInterface_;
};

}
}