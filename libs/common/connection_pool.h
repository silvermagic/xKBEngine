#pragma once

#include <vector>
#include <memory>
#include <string>
#include <cassert>
#include "singleton.h"

namespace xKBEngine {

template <typename T>
class ConnectionPool : public Singleton<ConnectionPool<T>>
{
public:
	/**
	    ���ݿ����ӷ�װ
	*/
	template <typename U>
	class Connection
	{
	public:
		Connection(std::shared_ptr<U> connection, bool force = false) :
			force_(force),
			connection_(connection)
		{
		}

		~Connection()
		{
			// �����ǿ�ƽ��������ݿ����ӣ���ô���ù黹���ӳأ�ֱ�ӹر�
			if (force_)
			{
				connection_->close();
			}
			else
			{
				ConnectionPool<U>::getSingleton().free(connection_);
			}
		}

		// ��ȡ���ݿ�����
		U& get() const { return *connection_; }

	protected:
		bool force_;
		std::shared_ptr<U> connection_;
	};

public:
	ConnectionPool(size_t init, size_t max, std::string uri) :
		init_(init),
		max_(max),
		nums_(0),
		uri_(uri),
		connections_()
	{
	}

	~ConnectionPool()
	{
		for (auto iter : connections_)
		{
			iter->close();
			nums_--;
		}
		assert(nums_ == 0);
		connections_.clear();
	}

	void initialize()
	{
		for (; nums_ < init_; nums_++)
		{
			std::shared_ptr<T> conn = std::make_shared<T>(uri_);
			connections_.push_back(conn);
		}
	}

	/**
	    ��ȡһ�����е����ݿ�����
	*/
	std::shared_ptr<Connection<T>> get(bool force = false)
	{
		if (connections_.empty())
		{
			// �������������δ�������ӳ����ֵ������һ���µ����ݿ�����
			if (nums_ < max_)
			{
				std::shared_ptr<T> conn = std::make_shared<T>(uri_);
				connections_.push_back(conn);
				nums_++;
			}
			else if (force) // ������ӳ��Ѿ����ˣ�����������ǿ�ƻ�ȡ������⽨���µ����ݿ����ӣ����ӻᵥ���ͷţ�����������ӳ�
			{
				std::shared_ptr<T> conn = std::make_shared<T>(uri_);
				return std::make_shared<Connection<T>>(conn, force);
			}
			else
				return nullptr;
		}

		std::shared_ptr<Connection<T>> conn = std::make_shared<Connection<T>>(connections_.back());
		connections_.pop_back();
		return conn;
	}

protected:
	/**
	    �����ݿ����ӹ黹���ӳ�
	*/
	void free(std::shared_ptr<T> conn)
	{
		connections_.push_back(conn);
	}

protected:
	size_t init_, max_, nums_; // ���ӳس�ʼ��С������С���������Ӹ���
	std::string uri_; // ���ݿ�����URL
	std::vector<std::shared_ptr<T>> connections_; // ���ӳ�
};

}