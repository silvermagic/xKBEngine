#include <cassert>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <boost/log/trivial.hpp>
#include "common.h"
#include "packet.h"
#include "endpoint.h"

namespace xKBEngine { namespace network {

EndPoint::EndPoint(Address address):
	socket_(-1),
	address_(address)
{
}

EndPoint::EndPoint(uint32_t ipArg, uint16_t portArg):
	socket_(-1),
	address_(ipArg, portArg)
{
}

EndPoint::~EndPoint()
{
	close();
}

std::shared_ptr<EndPoint> EndPoint::accept(bool autosetflags)
{
	struct sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	int ret = ::accept(socket_, (struct sockaddr*)&addr, &addrLen);
	if (ret < 0) return NULL;
	std::shared_ptr<EndPoint> pNew = std::make_shared<EndPoint>(addr.sin_addr.s_addr, addr.sin_port);
	pNew->setFileDescriptor(ret);
	if (autosetflags)
	{
		pNew->setnonblocking();
		pNew->setnodelay();
	}

	return pNew;
}

int EndPoint::bind()
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = address_.ip;
	addr.sin_port = address_.port;
	return ::bind(socket_, (struct sockaddr *)&addr, sizeof(addr));
}

int EndPoint::close()
{
	if (socket_ == -1)
	{
		return 0;
	}

	int ret = ::close(socket_);

	if (ret == 0)
	{
		setFileDescriptor(-1);
	}

	return ret;
}

int EndPoint::connect(bool autosetflags)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = address_.ip;
	addr.sin_port = address_.port;
	int ret = ::connect(socket_, (struct sockaddr *)&addr, sizeof(addr));
	if (autosetflags)
	{
		setnonblocking();
		setnodelay();
	}

	return ret;
}

bool EndPoint::good() const
{
	return socket_ != -1;
}

int EndPoint::joinMulticastGroup(const char *addr)
{
	struct ip_mreq req;
	req.imr_multiaddr.s_addr = inet_addr(addr);
	req.imr_interface.s_addr = htonl(INADDR_ANY);
	return ::setsockopt(socket_, IPPROTO_IP, IP_ADD_MEMBERSHIP, &req, sizeof(req));
}

int EndPoint::listen(int backlog)
{
	return ::listen(socket_, backlog);
}

int EndPoint::quitMulticastGroup(const char *addr)
{
	struct ip_mreq req;
	req.imr_multiaddr.s_addr = inet_addr(addr);
	req.imr_interface.s_addr = htonl(INADDR_ANY);
	return ::setsockopt(socket_, IPPROTO_IP, IP_DROP_MEMBERSHIP, &req, sizeof(req));
}

int EndPoint::recv(std::shared_ptr<Packet> pPacket)
{
	assert(PACKET_MAX_SIZE_TCP > pPacket->wpos());
	int len = ::recv(socket_, pPacket->data() + pPacket->wpos(), pPacket->size() - pPacket->wpos(), 0);
	if (len > 0)
	{
		pPacket->wpos(pPacket->wpos() + len);
	}
	return len;
}

int EndPoint::recvfrom(std::shared_ptr<Packet> pPacket, Address &address)
{
	assert(PACKET_MAX_SIZE_UDP > pPacket->wpos());
	struct sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	int len = ::recvfrom(socket_, pPacket->data() + pPacket->wpos(), pPacket->size() - pPacket->wpos(), 0, (struct sockaddr *)&addr, &addrLen);
	if (len > 0)
	{
		pPacket->wpos(pPacket->wpos() + len);
		address.ip = addr.sin_addr.s_addr;
		address.port = addr.sin_port;
	}
	return len;
}

int EndPoint::send(std::shared_ptr<Packet> pPacket)
{
	int  len = ::send(socket_, pPacket->data() + pPacket->sentSize, pPacket->length() - pPacket->sentSize, 0);
	if (len > 0)
	{
		pPacket->sentSize += len;
	}
	return len;
}
	
int EndPoint::sendto(std::shared_ptr<Packet> pPacket)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = address_.ip;
	addr.sin_port = address_.port;
	int len = ::sendto(socket_, pPacket->data() + pPacket->sentSize, pPacket->length() - pPacket->sentSize, 0, (struct sockaddr *)&addr, sizeof(addr));
	if (len > 0)
	{
		pPacket->sentSize += len;
	}
	return len;
}

int EndPoint::setbroadcast(bool broadcast)
{
	uint32_t optval = broadcast ? 1 : 0;
	return ::setsockopt(socket_, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));
}

int EndPoint::setnodelay(bool nodelay)
{
	uint32_t optval = nodelay ? 1 : 0;
	return ::setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

int EndPoint::setnonblocking(bool nonblocking)
{
	uint32_t optval = nonblocking ? O_NONBLOCK : 0;
	return ::fcntl(socket_, F_SETFL, optval);
}

void EndPoint::setFileDescriptor(int fd)
{
	socket_ = fd;
}

void EndPoint::socket(int type)
{
	setFileDescriptor(::socket(AF_INET, type, 0));
}

}
}