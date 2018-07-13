#pragma once

#include <vector>
#include <memory>
#include "common.h"
#include "interfaces.h"

namespace xKBEngine { namespace network {

class Packet;
class PacketSender : public OutputNotificationHandler
{
public:
	PacketSender(std::shared_ptr<Channel> pChannel);
	virtual ~PacketSender();
	virtual int handleOutputNotification(int fd);
	/**
	    �׽��ֶ�ȡ����
	*/
	virtual bool processSend(bool isEpoll) = 0;

protected:
	std::shared_ptr<Channel> pChannel_; // ����ͨ��
};

}
}