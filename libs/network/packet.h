#pragma once

#include "common/memorystream.h"

namespace xKBEngine { namespace network {

class Packet : public xKBEngine::MemoryStream
{
public:
	Packet(size_t res = 200):
		MemoryStream(res),
		isTCPPacket_(true),
		sentSize(0)
	{
		data_resize(res);
		wpos(0);
	}

	virtual ~Packet() {}

	/**
	    数据包内容是否已经读取完毕
	*/
	virtual bool empty() const { return length() == 0; }

	/**
	    数据包类型是否为TCP
	*/
	bool isTCPPacket() const { return isTCPPacket_; }

	/**
	    快速重置数据包
	 */
	virtual void clear(bool clearData = false)
	{
		xKBEngine::MemoryStream::clear(clearData);
		sentSize = 0;
	};

protected:
	bool isTCPPacket_;

public:
	uint32_t sentSize; // 由于发送是异步的，可能一次发送不了全部数据,所以需要记录下已发送长度
};

}
}