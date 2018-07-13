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
	    ���ݰ������Ƿ��Ѿ���ȡ���
	*/
	virtual bool empty() const { return length() == 0; }

	/**
	    ���ݰ������Ƿ�ΪTCP
	*/
	bool isTCPPacket() const { return isTCPPacket_; }

	/**
	    �����������ݰ�
	 */
	virtual void clear(bool clearData = false)
	{
		xKBEngine::MemoryStream::clear(clearData);
		sentSize = 0;
	};

protected:
	bool isTCPPacket_;

public:
	uint32_t sentSize; // ���ڷ������첽�ģ�����һ�η��Ͳ���ȫ������,������Ҫ��¼���ѷ��ͳ���
};

}
}