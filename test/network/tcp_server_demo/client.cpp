#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include "network/packet.h"
#include "network/endpoint.h"

using namespace xKBEngine::network;

enum MSG
{
	LOGIN = 0,
	LOGOUT
};

int main()
{
	Address address("127.0.0.1", htons(8080));
	EndPoint ep(address);
	ep.socket(SOCK_STREAM);
	if (!ep.good())
	{
		std::cout << "socket error(" << strerror(errno) << ")!" << std::endl;
		return -1;
	}

	if (-1 == ep.connect(false))
	{
		std::cout << "connect error(" << strerror(errno) << ")!" << std::endl;
		return -1;
	}

	std::shared_ptr<Packet> pPacket = std::make_shared<Packet>(sizeof(uint32_t) * 4);
	// DATA_LEN + MSG_ID + NUMBER
	std::string number = "88888888";
	uint16_t len = sizeof(uint16_t) + sizeof(uint32_t) + number.length() + 1;
	(*pPacket) << len << uint32_t(LOGIN) << number;
	if (ep.send(pPacket) <= 0)
	{
		std::cout << "send error(" << strerror(errno) << ")!" << std::endl;
		return -1;
	}

	pPacket->clear();
	if (ep.recv(pPacket) <= 0)
	{
		std::cout << "recv error(" << strerror(errno) << ")!" << std::endl;
		return -1;
	}
	std::cout << "recv msg: " << std::endl << pPacket->toString();

	pPacket->clear();
	(*pPacket) << len << uint32_t(LOGOUT) << number;
	if (ep.send(pPacket) <= 0)
	{
		std::cout << "send error(" << strerror(errno) << ")!" << std::endl;
		return -1;
	}

	pPacket->clear();
	if (ep.recv(pPacket) <= 0)
	{
		std::cout << "recv error(" << strerror(errno) << ")!" << std::endl;
		return -1;
	}
	std::cout << "recv msg: " << std::endl << pPacket->toString();
}