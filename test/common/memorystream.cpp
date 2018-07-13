#include <iostream>
#include <cstring>
#include <sstream>
#include <iomanip>
#include "common/memorystream.h"

using namespace xKBEngine;

std::string toBytes(const uint8_t *value, size_t cnt)
{
    std::stringstream ss;
    for (size_t i = 0; i < cnt; i++)
        ss << "0x" << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(value[i]) << " ";
    return ss.str();
}

int main()
{
    // MemoryStream Test
    int32_t i = 4096;
    float f = -150.3269f;
    double d = 56.231231246;
    const char *s = "test by fwd";
    std::cout << "Send to Network:" << std::endl;
    std::cout << std::setw(8) << "int32_t:" << std::setw(15) << i << " Hex:"<< toBytes((uint8_t *)&i, sizeof(i)) << std::endl;
    std::cout << std::setw(8) << "float:" << std::setw(15) << f << " Hex:"<< toBytes((uint8_t *)&f, sizeof(f)) << std::endl;
    std::cout << std::setw(8) << "double:" << std::setw(15) << d << " Hex:"<< toBytes((uint8_t *)&d, sizeof(d))  << std::endl;
    std::cout << std::setw(8) << "string:" << std::setw(15) << s << " Hex:"<< toBytes((uint8_t *)s, strlen(s)) << std::endl;

    MemoryStream ms;
    ms << i << f << d << s;
    std::cout << std::endl << ms.toString() << std::endl;

    int32_t ri;
    float rf;
    double rd;
    char rs[32] = {0};
    ms >> ri >> rf >> rd >> rs;
    std::cout << std::setw(8) << "int32_t:" << std::setw(15) << ri << " Hex:"<< toBytes((uint8_t *)&ri, sizeof(ri)) << std::endl;
    std::cout << std::setw(8) << "float:" << std::setw(15) << rf << " Hex:"<< toBytes((uint8_t *)&rf, sizeof(rf)) << std::endl;
    std::cout << std::setw(8) << "double:" << std::setw(15) << rd << " Hex:"<< toBytes((uint8_t *)&rd, sizeof(rd))  << std::endl;
    std::cout << std::setw(8) << "string:" << std::setw(15) << rs << " Hex:"<< toBytes((uint8_t *)rs, strlen(rs)) << std::endl;

    return 0;
}