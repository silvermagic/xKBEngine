#include <iostream>
#include "message_handler.h"
#include "packet_reader.h"

namespace xKBEngine { namespace network {

PacketReader::PacketReader(std::shared_ptr<Channel> pChannel):
	currPacket(),
	fragmentDatasFlag_(FRAGMENT_DATA_UNKNOW),
	pChannel_(pChannel)
{
}


PacketReader::~PacketReader()
{
	reset();
	pChannel_ = NULL;
}

void PacketReader::processMessages(std::shared_ptr<MessageHandler> pMsgHandler, std::shared_ptr<Packet> pPacket)
{
	BOOST_LOG_TRIVIAL(debug) << "PacketReader::processMessages("
		<< pChannel_->c_str() << ").";

	while (pPacket->length() > 0)
	{
		// 首先获取消息长度，默认2字节0~65535
		if (fragmentDatasFlag_ == FRAGMENT_DATA_UNKNOW)
		{
			// 消息不完整，需要等待下一个数据包
			if (pPacket->length() < 2)
			{
				writeFragmentMessage(FRAGMENT_DATA_MESSAGE_LENGTH, pPacket, pPacket->length());
				break;
			}
			else
			{
				uint16_t bodySize;
				(*pPacket) >> bodySize;
				// 获取body的实际长度
				bodySize -= 2;
				currPacket.data_resize(bodySize);
				currPacket.wpos(0);
			}
		}
		else if (fragmentDatasFlag_ == FRAGMENT_DATA_MESSAGE_LENGTH)
		{
			uint16_t bodySize;
			uint8_t c;
			(*pPacket) >> c;
			currPacket << c;
			currPacket >> bodySize;
			// 获取body的实际长度
			bodySize -= 2;
			currPacket.data_resize(bodySize);
			currPacket.wpos(0);
		}

		// 消息不完整，需要等待下一个数据包
		if (pPacket->length() < currPacket.space())
		{
			writeFragmentMessage(FRAGMENT_DATA_MESSAGE_BODY, pPacket, pPacket->length());
			break;
		}
		else
		{
			writeFragmentMessage(FRAGMENT_DATA_MESSAGE_BODY, pPacket, currPacket.space());
			pMsgHandler->handle(pChannel_, currPacket);
			reset();
		}
	}
}

void PacketReader::reset()
{
	currPacket.clear();
	fragmentDatasFlag_ = FRAGMENT_DATA_UNKNOW;
}

void PacketReader::writeFragmentMessage(FragmentDataTypes fragmentDatasFlag, std::shared_ptr<Packet> pPacket, uint32_t datasize)
{
	BOOST_LOG_TRIVIAL(debug) << "PacketReader::writeFragmentMessage("
		<< pChannel_->c_str() << ") write " << datasize  << " bytes.";

	fragmentDatasFlag_ = fragmentDatasFlag;
	std::memcpy(currPacket.data() + currPacket.wpos(), pPacket->data() + pPacket->rpos(), datasize);
	currPacket.wpos(currPacket.wpos() + datasize);
	pPacket->read_skip(datasize);
}

}
}