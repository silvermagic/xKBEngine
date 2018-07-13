#pragma once

#include <vector>
#include <cstdint>

namespace xKBEngine {

class MemoryStream
{
public:
	MemoryStream();
	MemoryStream(size_t res);
	MemoryStream(const MemoryStream &buf);
	virtual ~MemoryStream() { clear(true); }

	/**
	    重载输入运算符
	*/
	MemoryStream& operator<<(uint8_t value);
	MemoryStream& operator<<(uint16_t value);
	MemoryStream& operator<<(uint32_t value);
	MemoryStream& operator<<(uint64_t value);
	MemoryStream& operator<<(int8_t value);
	MemoryStream& operator<<(int16_t value);
	MemoryStream& operator<<(int32_t value);
	MemoryStream& operator<<(int64_t value);
	MemoryStream& operator<<(float value);
	MemoryStream& operator<<(double value);
	MemoryStream& operator<<(bool value);
	MemoryStream& operator<<(const std::string& value);
	MemoryStream& operator<<(const char* value);

	/**
	    重载输出运算符
	*/
	MemoryStream& operator>>(uint8_t& value);
	MemoryStream& operator>>(uint16_t& value);
	MemoryStream& operator>>(uint32_t& value);
	MemoryStream& operator>>(uint64_t& value);
	MemoryStream& operator>>(int8_t& value);
	MemoryStream& operator>>(int16_t& value);
	MemoryStream& operator>>(int32_t& value);
	MemoryStream& operator>>(int64_t& value);
	MemoryStream& operator>>(float& value);
	MemoryStream& operator>>(double& value);
	MemoryStream& operator>>(bool& value);
	MemoryStream& operator>>(std::string& value);
	MemoryStream& operator>>(char* value);

	/**
	    重载[]运算符
	*/
	uint8_t operator[](size_t pos) const { return data_[pos]; }
	
	/**
	    清空流
	*/
	virtual void clear(bool clearData = false);

	/**
	    获取流的底层存储
	*/
	uint8_t* data() { return data_.data(); }
	const uint8_t* data() const { return data_.data(); }

	/**
	    仅仅调整流的存储空间，不修改读写索引
	*/
	void data_resize(size_t newsize);

	/**
	    流读操作结束
	*/
	void done() { read_skip(length()); }

	/**
	    判断流是否为空
	*/
	virtual bool empty() const { return data_.empty(); }

	/**
	    未读流的长度
	*/
	virtual size_t length() const { return rpos() >= wpos() ? 0 : wpos() - rpos(); }

	/**
	    跳过读取skip个字节
	*/
	void read_skip(size_t skip);

	/**
	    扩展流的预留存储空间
	*/
	void reserve(size_t ressize);

	/**
	    调整流的存储空间，重置读写索引
	*/
	void resize(size_t newsize);

	/**
	    读光标操作
	*/
	size_t rpos() const { return rpos_; }
	void rpos(size_t rpos) { rpos_ = rpos; }

	/**
	    获取流的存储空间
	*/
	virtual size_t size() const { return data_.size(); }

	/**
	    流的剩余存储空间
	*/
	virtual size_t space() const { return wpos() >= size() ? 0 : size() - wpos(); }

	/**
	    交换两个流对象
	*/
	void swap(MemoryStream& s);

	/**
	    打印流内容,格式如下:
        起始字节号   字节十六进制格式                             字节ASCII格式
	    0000:        36 33 00 a8 03 00 00 00 d4 b0 01 00          63..........
	*/
	std::string toString(bool total = false) const;

	/**
	    写光标操作
	*/
	size_t wpos() const { return wpos_; }
	void wpos(size_t wpos) { wpos_ = wpos; }

protected:
	void append(const uint8_t* value, size_t cnt);
	void read(uint8_t* value, size_t cnt);

public:
	const static size_t DEFAULT_SIZE = 0x100;
	const static size_t MAX_SIZE = 10000000;

protected:
	size_t rpos_, wpos_;
	std::vector<uint8_t> data_;
};

}