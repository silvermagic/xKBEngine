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
	    接收套接字连接
	 */
	std::shared_ptr<EndPoint> accept(bool autosetflags = true);

	/**
	    获取终端地址
	*/
	Address address() const { return address_; }

	/**
	    设置终端地址
	*/
	void address(const Address& address) { address_ = address; }

	/**
	    绑定套接字(TCP/UDP)
	*/
	int bind();

	/**
	    关闭套接字
	*/
	int close();

	/**
	    连接套接字(TCP)
	*/
	int connect(bool autosetflags = true);

	/**
	    获取终端描述
	*/
	const char * c_str() const { return address_.c_str(); }

	/**
	    检测终端是否可用
	*/
	bool good() const;

	/**
	    加入广播组
	*/
	int joinMulticastGroup(const char *addr);

	/**
	    监听套接字(TCP)
	*/
	int listen(int backlog = g_SOMAXCONN);

	/**
	    退出广播组
	*/
	int quitMulticastGroup(const char *addr);

	/**
	    接收数据
	*/
	int recv(std::shared_ptr<Packet> pPacket);
	int recvfrom(std::shared_ptr<Packet> pPacket, Address &address);

	/**
	    发送数据
	*/
	int send(std::shared_ptr<Packet> pPacket);
	int sendto(std::shared_ptr<Packet> pPacket);

	/**
	    设置套接字为广播模式
	*/
	int setbroadcast(bool broadcast);

	/**
	    设置套接字为非延迟模式
	*/
	int setnodelay(bool nodelay = true);

	/**
	    设置套接字为非阻塞模式
	*/
	int setnonblocking(bool nonblocking = true);

	/**
	    设置套接字描述符
	*/
	void setFileDescriptor(int fd);

	/**
	    设置套接字描述符类型
	*/
	void socket(int type);

protected:
	int socket_;
	Address address_;
};

}
}