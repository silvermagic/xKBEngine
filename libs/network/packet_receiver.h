#pragma once

#include <vector>
#include <memory>
#include "common.h"
#include "interfaces.h"

namespace xKBEngine { namespace network {

class Packet;
class Channel;
class PacketReceiver : public InputNotificationHandler
{
public:
	PacketReceiver(std::shared_ptr<Channel> pChannel);
	virtual ~PacketReceiver();

	/**
	    ���ݽ��մ���
	*/
	virtual int handleInputNotification(int fd);

	/**
	    �׽��ֶ�ȡ����
	*/
	virtual bool processRecv() = 0;

	/**
	    ��������
	*/
	virtual PACKET_RECEIVER_TYPE type() const
	{
		return TCP_PACKET_RECEIVER;
	}

protected:
	std::shared_ptr<Channel> pChannel_; // ����ͨ��
};

}
}