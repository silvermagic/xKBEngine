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
	    �������������
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
	    ������������
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
	    ����[]�����
	*/
	uint8_t operator[](size_t pos) const { return data_[pos]; }
	
	/**
	    �����
	*/
	virtual void clear(bool clearData = false);

	/**
	    ��ȡ���ĵײ�洢
	*/
	uint8_t* data() { return data_.data(); }
	const uint8_t* data() const { return data_.data(); }

	/**
	    �����������Ĵ洢�ռ䣬���޸Ķ�д����
	*/
	void data_resize(size_t newsize);

	/**
	    ������������
	*/
	void done() { read_skip(length()); }

	/**
	    �ж����Ƿ�Ϊ��
	*/
	virtual bool empty() const { return data_.empty(); }

	/**
	    δ�����ĳ���
	*/
	virtual size_t length() const { return rpos() >= wpos() ? 0 : wpos() - rpos(); }

	/**
	    ������ȡskip���ֽ�
	*/
	void read_skip(size_t skip);

	/**
	    ��չ����Ԥ���洢�ռ�
	*/
	void reserve(size_t ressize);

	/**
	    �������Ĵ洢�ռ䣬���ö�д����
	*/
	void resize(size_t newsize);

	/**
	    ��������
	*/
	size_t rpos() const { return rpos_; }
	void rpos(size_t rpos) { rpos_ = rpos; }

	/**
	    ��ȡ���Ĵ洢�ռ�
	*/
	virtual size_t size() const { return data_.size(); }

	/**
	    ����ʣ��洢�ռ�
	*/
	virtual size_t space() const { return wpos() >= size() ? 0 : size() - wpos(); }

	/**
	    ��������������
	*/
	void swap(MemoryStream& s);

	/**
	    ��ӡ������,��ʽ����:
        ��ʼ�ֽں�   �ֽ�ʮ�����Ƹ�ʽ                             �ֽ�ASCII��ʽ
	    0000:        36 33 00 a8 03 00 00 00 d4 b0 01 00          63..........
	*/
	std::string toString(bool total = false) const;

	/**
	    д������
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