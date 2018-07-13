#include <iostream>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <cassert>
#include <cstring>
#include <endian.h>
#include "memorystream.h"

namespace xKBEngine {

MemoryStream::MemoryStream():
	rpos_(0),
	wpos_(0)
{
	data_.reserve(DEFAULT_SIZE);
}

MemoryStream::MemoryStream(size_t res):
	rpos_(0),
	wpos_(0)
{
	if (res > 0)
		data_.reserve(res);
}

MemoryStream::MemoryStream(const MemoryStream &buf):
	rpos_(buf.rpos_),
	wpos_(buf.wpos_),
	data_(buf.data_)
{
}

MemoryStream& MemoryStream::operator<<(uint8_t value)
{
	append((const uint8_t *)&value, sizeof(value));
	return *this;
}
MemoryStream& MemoryStream::operator<<(uint16_t value)
{
	uint16_t nvalue = htobe16(value);
	append((const uint8_t *)&nvalue, sizeof(nvalue));
	return *this;
}
MemoryStream& MemoryStream::operator<<(uint32_t value)
{
	uint32_t nvalue = htobe32(value);
	append((const uint8_t *)&nvalue, sizeof(nvalue));
	return *this;
}
MemoryStream& MemoryStream::operator<<(uint64_t value)
{
	uint32_t nvalue = htobe64(value);
	append((const uint8_t *)&nvalue, sizeof(nvalue));
	return *this;
}
MemoryStream& MemoryStream::operator<<(int8_t value)
{
	append((const uint8_t *)&value, sizeof(value));
	return *this;
}

MemoryStream& MemoryStream::operator<<(int16_t value)
{
	uint16_t nvalue = htobe16(*((uint16_t *)&value));
	append((const uint8_t *)&nvalue, sizeof(nvalue));
	return *this;
}

MemoryStream& MemoryStream::operator<<(int32_t value)
{
	uint32_t nvalue = htobe32(*((uint32_t *)&value));
	append((const uint8_t *)&nvalue, sizeof(nvalue));
	return *this;
}

MemoryStream& MemoryStream::operator<<(int64_t value)
{
	uint32_t nvalue = htobe64(*((uint64_t *)&value));
	append((const uint8_t *)&nvalue, sizeof(nvalue));
	return *this;
}

MemoryStream& MemoryStream::operator<<(float value)
{
	uint32_t nvalue = htobe32(*((uint32_t *)&value));
	append((const uint8_t *)&nvalue, sizeof(nvalue));
	return *this;
}

MemoryStream& MemoryStream::operator<<(double value)
{
	uint64_t nvalue = htobe64(*((uint64_t *)&value));
	append((const uint8_t *)&nvalue, sizeof(nvalue));
	return *this;
}

MemoryStream& MemoryStream::operator<<(bool value)
{
	*this << (value ? (uint8_t)1 : (uint8_t)0);
	return *this;
}

MemoryStream& MemoryStream::operator<<(const std::string& value)
{
	append((const uint8_t *)value.c_str(), value.length());
	*this << (uint8_t)0;
	return *this;
}

MemoryStream& MemoryStream::operator<<(const char* value)
{
	append((const uint8_t *)value, value ? strlen(value) : 0);
	*this << (uint8_t)0;
	return *this;
}

MemoryStream& MemoryStream::operator>>(uint8_t& value)
{
	uint8_t nvalue;
	read((uint8_t *)&nvalue, sizeof(nvalue));
	value = nvalue;
	return *this;
}

MemoryStream& MemoryStream::operator>>(uint16_t& value)
{
	uint16_t nvalue;
	read((uint8_t *)&nvalue, sizeof(nvalue));
	value = be16toh(nvalue);
	return *this;
}

MemoryStream& MemoryStream::operator>>(uint32_t& value)
{
	uint32_t nvalue;
	read((uint8_t *)&nvalue, sizeof(nvalue));
	value = be32toh(nvalue);
	return *this;
}

MemoryStream& MemoryStream::operator>>(uint64_t& value)
{
	uint64_t nvalue;
	read((uint8_t *)&nvalue, sizeof(nvalue));
	value = be64toh(nvalue);
	return *this;
}

MemoryStream& MemoryStream::operator>>(int8_t& value)
{
	uint8_t nvalue;
	read((uint8_t *)&nvalue, sizeof(nvalue));
	std::memcpy(&value, &nvalue, sizeof(value));
	return *this;
}

MemoryStream& MemoryStream::operator>>(int16_t& value)
{
	uint16_t nvalue;
	read((uint8_t *)&nvalue, sizeof(nvalue));
	nvalue = be16toh(nvalue);
	std::memcpy(&value, &nvalue, sizeof(value));
	return *this;
}

MemoryStream& MemoryStream::operator>>(int32_t& value)
{
	uint32_t nvalue;
	read((uint8_t *)&nvalue, sizeof(nvalue));
	nvalue = be32toh(nvalue);
	std::memcpy(&value, &nvalue, sizeof(value));
	return *this;
}

MemoryStream& MemoryStream::operator>>(int64_t& value)
{
	uint64_t nvalue;
	read((uint8_t *)&nvalue, sizeof(nvalue));
	nvalue = be64toh(nvalue);
	std::memcpy(&value, &nvalue, sizeof(value));
	return *this;
}

MemoryStream& MemoryStream::operator>>(float& value)
{
	uint32_t nvalue;
	read((uint8_t *)&nvalue, sizeof(nvalue));
	nvalue = be32toh(nvalue);
	std::memcpy(&value, &nvalue, sizeof(value));
	return *this;
}

MemoryStream& MemoryStream::operator>>(double& value)
{
	uint64_t nvalue;
	read((uint8_t *)&nvalue, sizeof(nvalue));
	nvalue = be64toh(nvalue);
	std::memcpy(&value, &nvalue, sizeof(value));
	return *this;
}

MemoryStream& MemoryStream::operator>>(bool& value)
{
	uint8_t nvalue;
	read((uint8_t *)&nvalue, sizeof(nvalue));
	value = (nvalue == 1 ? true : false);
	return *this;
}

MemoryStream& MemoryStream::operator>>(std::string& value)
{
	value.clear();
	while (length() > 0)
	{
		char c;
		read((uint8_t *)&c, sizeof(c));
		if (c == 0 || !isascii(c))
			break;

		value += c;
	}
	value += '\0';

	return *this;
}

MemoryStream& MemoryStream::operator>>(char* value)
{
	while (length() > 0)
	{
		char c;
		read((uint8_t *)&c, sizeof(c));
		if (c == 0 || !isascii(c))
			break;

		*(value++) = c;
	}
	*value = '\0';

	return *this;
}

void MemoryStream::clear(bool clearData)
{
	if (clearData)
		data_.clear();

	rpos_ = wpos_ = 0;
}

void MemoryStream::data_resize(size_t newsize)
{
	assert(newsize <= 1310700);
	data_.resize(newsize);
	if (wpos_ > size())
		wpos_ = size();
}

void MemoryStream::read_skip(size_t skip)
{
	if (skip > length())
	{
		std::stringstream ss;
		ss << "Skip exceeds the range of MemoryStream (pos:" << rpos_ << "  skip:"<< skip << "  size: " << length() << ").";
		throw std::out_of_range(ss.str());
	}

	rpos_ += skip;
}

void MemoryStream::reserve(size_t ressize)
{
	assert(ressize <= 1310700);

	if (ressize > size())
		data_.reserve(ressize);
}

void MemoryStream::resize(size_t newsize)
{
	assert(newsize <= 1310700);
	data_.resize(newsize);
	rpos_ = 0;
	if (wpos_ > size())
		wpos_ = size();
}

void MemoryStream::swap(MemoryStream& s)
{
	size_t rpos = s.rpos(), wpos = s.wpos();
	std::swap(data_, s.data_);
	s.rpos((int)rpos_);
	s.wpos((int)wpos_);
	rpos_ = rpos;
	wpos_ = wpos;
}

std::string MemoryStream::toString(bool total) const
{
	size_t j = 0, k = rpos_;
	std::stringstream ss;
	size_t len = length();
	if (total)
	{
		k = 0;
		len = wpos_;
	}

	for (; k < len; k++)
	{
		// 打印起始字节号
		if (!(j % 16))
			ss << "0x" << std::setfill('0') << std::setw(4) << std::hex << static_cast<int>(k) << ": ";
		// 十六进制打印bytes数据
		ss << "0x" << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(data_[k]) << " ";
		j++;
		// 每行打印16个
		if (j != 16)
			continue;

		// ascii打印bytes数据
		ss << "   ";
		size_t m = k - 15;
		for (size_t n = 0; n < 16; n++)
		{
			char c = *(reinterpret_cast<char const *>(&data_[m]));
			m++;
			if (c > 31 && c < 128)
			    ss << c;
			else
				ss << ".";
		}
		ss << std::endl;
		j = 0;
	}

	// 如果最后一次不足16个需要单独处理
	size_t last = len % 16;
	if (last)
	{
		// 用空格代替原本需要打印的十六进制bytes数据
		for (j = 0; j < (16 - last); j++)
			ss << "     ";
		// ascii打印bytes数据
		ss << "   ";
		k = len - last;
		for (size_t n = 0; n < last; n++)
		{
			char c = *(reinterpret_cast<char const *>(&data_[k]));
			k++;
			if (c > 31 && c < 128)
				ss << c;
			else
				ss << ".";
		}
		ss << std::endl;
	}
	return ss.str();
}

void MemoryStream::append(const uint8_t* value, size_t cnt)
{
	if (!cnt)
		return;

	assert(size() < MAX_SIZE);

	if (data_.size() < wpos_ + cnt)
		data_.resize(wpos_ + cnt);

	std::memcpy(&data_[wpos_], value, cnt);
	wpos_ += cnt;
}

void MemoryStream::read(uint8_t* value, size_t cnt)
{
	if (cnt > length())
	{
		std::stringstream ss;
		ss << "Attempted to access in MemoryStream (pos:" << rpos_ << "  cnt:" << cnt << "  size:" << length() << ") failed!";
		throw std::out_of_range(ss.str());
	}

	std::memcpy(value, &data_[rpos_], cnt);
	rpos_ += cnt;
}

}
