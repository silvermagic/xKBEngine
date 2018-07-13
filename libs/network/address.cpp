#include <cstdio>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "address.h"

namespace xKBEngine { namespace network {

Address::Address():
	ip(0),
	port(0)
{
}

Address::Address(uint32_t ipArg, uint16_t portArg):
	ip(ipArg),
	port(portArg)
{
}

Address::Address(const char *ipArg, uint16_t portArg) :
	port(portArg)
{
	if (ipArg != NULL)
		inet_pton(AF_INET, ipArg, &ip);
}

Address::~Address()
{
}

const char * Address::c_str() const
{
	static char dodgyString[32];

	uint32_t hip = ntohl(ip);
	uint16_t hport = ntohs(port);

	::snprintf(dodgyString, sizeof(dodgyString),
		"%d.%d.%d.%d:%d",
		(int)(uint8_t)(hip >> 24),
		(int)(uint8_t)(hip >> 16),
		(int)(uint8_t)(hip >> 8),
		(int)(uint8_t)(hip),
		(int)hport);

	return dodgyString;
}

bool operator==(const Address &lvalue, const Address &rvalue)
{
	return (lvalue.ip == rvalue.ip) && (lvalue.port == rvalue.port);
}

bool operator!=(const Address &lvalue, const Address &rvalue)
{
	return (lvalue.ip != rvalue.ip) || (lvalue.port != rvalue.port);
}

bool operator<(const Address &lvalue, const Address &rvalue)
{
	return (lvalue.ip < rvalue.ip) || (lvalue.ip == rvalue.ip && (lvalue.port < rvalue.port));
}

}
}