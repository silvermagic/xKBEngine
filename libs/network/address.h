#pragma once

#include <string>
#include <cstdint>

namespace xKBEngine { namespace network {

class Address
{
public:
	Address();
	Address(uint32_t ipArg, uint16_t portArg);
	Address(const char *ipArg, uint16_t portArg);
	virtual ~Address();

	const char * c_str() const;

public:
	uint32_t ip;
	uint16_t port;
};

bool operator==(const Address &lvalue, const Address &rvalue);
bool operator!=(const Address &lvalue, const Address &rvalue);
bool operator<(const Address &lvalue, const Address &rvalue);

}
}
