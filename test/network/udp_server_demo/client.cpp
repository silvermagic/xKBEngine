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
	LOGIN
};

int main()
{
	Address address("127.0.0.1", htons(8080));
	EndPoint ep(address);
	ep.socket(SOCK_DGRAM);
	if (!ep.good())
	{
		std::cout << "socket error(" << strerror(errno) << ")!" << std::endl;
		return -1;
	}

	std::shared_ptr<Packet> pPacket = std::make_shared<Packet>(sizeof(uint32_t) * 4);
	// DATA_LEN + MSG_ID + NUMBER
	std::string number = "88888888";
	uint16_t len = sizeof(uint16_t) + sizeof(uint32_t) + number.length() + 1;
	(*pPacket) << len << uint32_t(LOGIN) << number;
	if (ep.sendto(pPacket) <= 0)
	{
		std::cout << "sendto error(" << strerror(errno) << ")!" << std::endl;
		return -1;
	}

	pPacket->clear();
	Address from;
	if (ep.recvfrom(pPacket, from) <= 0)
	{
		std::cout << "recv error(" << strerror(errno) << ")!" << std::endl;
		return -1;
	}
	std::cout << "from " << from.c_str() << ": " << std::endl << pPacket->toString();
}