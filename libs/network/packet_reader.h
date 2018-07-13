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
	    将接收到的数据包合并成一个完整的消息并处理
	*/
	virtual void processMessages(std::shared_ptr<MessageHandler> pMsgHandler, std::shared_ptr<Packet> pPacket);

	/**
	    重置
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
	    写入分段数据
	*/
	virtual void writeFragmentMessage(FragmentDataTypes fragmentDatasFlag, std::shared_ptr<Packet> pPacket, uint32_t datasize);

protected:
	Packet						currPacket; // 用于保存合并后的消息
	FragmentDataTypes			fragmentDatasFlag_; // 消息缺失标志
	std::shared_ptr<Channel>	pChannel_; // 所属通道
};

}
}