#pragma once

#include <memory>
#include "address.h"

namespace xKBEngine { namespace network {

extern uint32_t g_SOMAXCONN;

class Packet;
class EndPoint
{
public:
	EndPoint(Address address);
	EndPoint(uint32_t networkAddr = 0, uint16_t networkPort = 0);
	virtual ~EndPoint();

	operator int() const { return socket_; }

	/**
	    �����׽�������
	 */
	std::shared_ptr<EndPoint> accept(bool autosetflags = true);

	/**
	    ��ȡ�ն˵�ַ
	*/
	Address address() const { return address_; }

	/**
	    �����ն˵�ַ
	*/
	void address(const Address& address) { address_ = address; }

	/**
	    ���׽���(TCP/UDP)
	*/
	int bind();

	/**
	    �ر��׽���
	*/
	int close();

	/**
	    �����׽���(TCP)
	*/
	int connect(bool autosetflags = true);

	/**
	    ��ȡ�ն�����
	*/
	const char * c_str() const { return address_.c_str(); }

	/**
	    ����ն��Ƿ����
	*/
	bool good() const;

	/**
	    ����㲥��
	*/
	int joinMulticastGroup(const char *addr);

	/**
	    �����׽���(TCP)
	*/
	int listen(int backlog = g_SOMAXCONN);

	/**
	    �˳��㲥��
	*/
	int quitMulticastGroup(const char *addr);

	/**
	    ��������
	*/
	int recv(std::shared_ptr<Packet> pPacket);
	int recvfrom(std::shared_ptr<Packet> pPacket, Address &address);

	/**
	    ��������
	*/
	int send(std::shared_ptr<Packet> pPacket);
	int sendto(std::shared_ptr<Packet> pPacket);

	/**
	    �����׽���Ϊ�㲥ģʽ
	*/
	int setbroadcast(bool broadcast);

	/**
	    �����׽���Ϊ���ӳ�ģʽ
	*/
	int setnodelay(bool nodelay = true);

	/**
	    �����׽���Ϊ������ģʽ
	*/
	int setnonblocking(bool nonblocking = true);

	/**
	    �����׽���������
	*/
	void setFileDescriptor(int fd);

	/**
	    �����׽�������������
	*/
	void socket(int type);

protected:
	int socket_;
	Address address_;
};

}
}