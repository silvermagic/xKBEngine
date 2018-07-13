#pragma once

#include <memory>
#include "packet.h"

namespace xKBEngine { namespace network {

class Channel;
class MessageHandler;
class PacketReader
{
public:
	PacketReader(std::shared_ptr<Channel> pChannel);
	virtual ~PacketReader();

	/**
	    �����յ������ݰ��ϲ���һ����������Ϣ������
	*/
	virtual void processMessages(std::shared_ptr<MessageHandler> pMsgHandler, std::shared_ptr<Packet> pPacket);

	/**
	    ����
	*/
	virtual void reset();

protected:
	enum FragmentDataTypes
	{
		FRAGMENT_DATA_UNKNOW,
		FRAGMENT_DATA_MESSAGE_LENGTH,
		FRAGMENT_DATA_MESSAGE_BODY
	};

	/**
	    д��ֶ�����
	*/
	virtual void writeFragmentMessage(FragmentDataTypes fragmentDatasFlag, std::shared_ptr<Packet> pPacket, uint32_t datasize);

protected:
	Packet						currPacket; // ���ڱ���ϲ������Ϣ
	FragmentDataTypes			fragmentDatasFlag_; // ��Ϣȱʧ��־
	std::shared_ptr<Channel>	pChannel_; // ����ͨ��
};

}
}