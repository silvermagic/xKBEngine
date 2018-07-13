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
	    数据库连接封装
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
			// 如果是强制建立的数据库连接，那么不用归还连接池，直接关闭
			if (force_)
			{
				connection_->close();
			}
			else
			{
				ConnectionPool<U>::getSingleton().free(connection_);
			}
		}

		// 获取数据库连接
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
	    获取一个空闲的数据库连接
	*/
	std::shared_ptr<Connection<T>> get(bool force = false)
	{
		if (connections_.empty())
		{
			// 如果已有连接数未超过连接池最大值，则建立一个新的数据库连接
			if (nums_ < max_)
			{
				std::shared_ptr<T> conn = std::make_shared<T>(uri_);
				connections_.push_back(conn);
				nums_++;
			}
			else if (force) // 如果连接池已经满了，但是设置了强制获取，则额外建立新的数据库连接，连接会单独释放，不会进入连接池
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
	    将数据库连接归还连接池
	*/
	void free(std::shared_ptr<T> conn)
	{
		connections_.push_back(conn);
	}

protected:
	size_t init_, max_, nums_; // 连接池初始大小、最大大小、已有连接个数
	std::string uri_; // 数据库连接URL
	std::vector<std::shared_ptr<T>> connections_; // 连接池
};

}