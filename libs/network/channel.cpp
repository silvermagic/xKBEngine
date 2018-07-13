#include <cstdio>
#include <boost/log/trivial.hpp>
#include "endpoint.h"
#include "packet.h"
#include "network_interface.h"
#include "event_poller.h"
#include "tcp_packet_receiver.h"
#include "udp_packet_receiver.h"
#include "tcp_packet_sender.h"
#include "packet_reader.h"
#include "channel.h"

namespace xKBEngine { namespace network {

extern size_t g_numPacketsSent;
extern size_t g_numPacketsReceived;
extern size_t g_numBytesSent;
extern size_t g_numBytesReceived;

extern size_t g_receiveWindowMessagesOverflowCritical;
extern size_t g_receiveWindowMessagesOverflow;
extern size_t g_receiveWindowBytesOverflow;

extern size_t g_sendWindowMessagesOverflowCritical;
extern size_t g_sendWindowMessagesOverflow;
extern size_t g_sendWindowBytesOverflow;

Channel::Channel(NetworkInterface& networkInterface,
	std::shared_ptr<EndPoint> pEndPoint,
	ProtocolType pt):
	protocoltype_(pt),
	flags_(0),
	lastTickBytesReceived_(0),
	lastTickBytesSent_(0),
	numBytesReceived_(0),
	numBytesSent_(0),
	numPacketsReceived_(0),
	numPacketsSent_(0),
	networkInterface_(networkInterface),
	lastReceivedTime_(TIME_NOW),
	pEndPoint_(pEndPoint),
	pPacketReader_(NULL),
	pPacketReceiver_(NULL),
	pPacketSender_(NULL)
{
	clearBundle();
}

Channel::~Channel()
{
	finalise();
}

void Channel::addReceiveWindow(std::shared_ptr<Packet> pPacket)
{
	BOOST_LOG_TRIVIAL(debug) << "Channel::addReceiveWindow["
		<< static_cast<void *>(this)
		<< "]: channel("
		<< c_str()
		<< ").";

	bufferedReceives_.push_back(pPacket);
	size_t size = bufferedReceives_.size();

	// 接收窗口告警处理
	if (g_receiveWindowMessagesOverflowCritical > 0 && size > g_receiveWindowMessagesOverflowCritical)
	{
		// 到达此处说明业务处理不及时，导致数据堆积，如果超过阀值则报错，并标记通道不合法
		if (g_receiveWindowMessagesOverflow > 0 && size > g_receiveWindowMessagesOverflow)
		{
			BOOST_LOG_TRIVIAL(error) << "Channel::addReceiveWindow["
				<< static_cast<void *>(this)
				<< "]: channel("
				<< c_str()
				<< "), receive window has overflowed("
				<< size
				<< " > "
				<< g_receiveWindowMessagesOverflow << ").";
			condemn();
		}
		else
		{
			BOOST_LOG_TRIVIAL(warning) << "Channel::addReceiveWindow["
				<< static_cast<void *>(this)
				<< "]: channel("
				<< c_str()
				<< "), receive window has overflowed("
				<< size
				<< " > "
				<< g_receiveWindowMessagesOverflow << ").";
		}
	}
}

size_t Channel::bundlesLength()
{
	size_t len = 0;
	for (auto iter : bundles_)
	{
		len += iter->length();
	}
	return len;
}

void Channel::clearBundle()
{
	bundles_.clear();
}

void Channel::condemn()
{
	if (isCondemn())
		return;

	flags_ |= FLAG_CONDEMN;
}

const char * Channel::c_str() const
{
	static char dodgyString[128] = { "None" };
	::snprintf(dodgyString, sizeof(dodgyString),
		"%s/%s/%d/%d/%d",
		pEndPoint_->c_str(),
		(protocoltype_ ? "UDP" : "TCP"),
		int(*pEndPoint_),
		isCondemn(),
		isDestroyed());
	return dodgyString;
}

void Channel::destroy()
{
	if (isDestroyed())
	{
		BOOST_LOG_TRIVIAL(warning) << "channel("
			<< c_str()
			<< ") has Destroyed!";
		return;
	}

	clearState();
	flags_ |= FLAG_DESTROYED;
}

bool Channel::finalise()
{
	clearState();

	// 释放引用
	pPacketReader_ = NULL;
	pPacketReceiver_ = NULL;
	pPacketSender_ = NULL;

	return true;
}

bool Channel::initialize(bool registerForRead)
{
	BOOST_LOG_TRIVIAL(debug) << "Channel::initialize["
		<< static_cast<void *>(this)
		<< "]: channel("
		<< c_str() << ").";

	if (protocoltype_ == PROTOCOL_TCP)
	{
		if (pPacketReceiver_)
		{
			if (pPacketReceiver_->type() == UDP_PACKET_RECEIVER)
				pPacketReceiver_ = NULL;
		}
		else
		{
			if (registerForRead)
				pPacketReceiver_ = std::make_shared<TCPPacketReceiver>(shared_from_this());
		}
	}
	else
	{
		if (pPacketReceiver_)
		{
			if (pPacketReceiver_->type() == TCP_PACKET_RECEIVER)
				pPacketReceiver_ = NULL;
		}
		else
		{
			if (registerForRead)
				pPacketReceiver_ = std::make_shared<UDPPacketReceiver>(shared_from_this());
		}
	}

	if (registerForRead)
	{
		if (!networkInterface_.pPoller()->registerForRead(*pEndPoint_, pPacketReceiver_))
		{
			BOOST_LOG_TRIVIAL(error) << "Channel::initialize["
				<< static_cast<void *>(this)
				<< "]: channel("
				<< c_str()
				<< "), registerForRead failed.";
			return false;
		}
		flags_ |= FLAG_RECVING;
	}

	if (pPacketReader_ == NULL)
		pPacketReader_ = std::make_shared<PacketReader>(shared_from_this());

	return true;
}

void Channel::onPacketReceived(int bytes)
{
	BOOST_LOG_TRIVIAL(debug) << "Channel::onPacketReceived["
		<< static_cast<void *>(this)
		<< "]: channel("
		<< c_str()
		<< "), received " << bytes << " bytes.";

	lastReceivedTime_ = TIME_NOW;
	++numPacketsReceived_;
	++g_numPacketsReceived;

	if (bytes > 0)
	{
		numBytesReceived_ += bytes;
		lastTickBytesReceived_ += bytes;
		g_numBytesReceived += bytes;
	}

	// 检测堆积的数据包字节数是否已经超过阀值
	if (g_receiveWindowBytesOverflow > 0 && lastTickBytesReceived_ >= g_receiveWindowBytesOverflow)
	{
		BOOST_LOG_TRIVIAL(error) << "Channel::onPacketReceived["
			<< static_cast<void *>(this)
			<< "]: channel("
			<< c_str()
			<< "), bufferedBytes has overflowed("
			<< lastTickBytesReceived_
			<< " > "
			<< g_receiveWindowBytesOverflow << ").";
		condemn();
	}
}

void Channel::onPacketSent(int bytes, bool sentCompleted)
{
	BOOST_LOG_TRIVIAL(debug) << "Channel::onPacketSent["
		<< static_cast<void *>(this)
		<< "]: channel("
		<< c_str()
		<< "), send " << bytes << " bytes(" << (sentCompleted ? "finished" : "unfinished") <<").";

	if (sentCompleted)
	{
		++numPacketsSent_;
		++g_numPacketsSent;
	}

	if (bytes > 0)
	{
		numBytesSent_ += bytes;
		g_numBytesSent += bytes;
		lastTickBytesSent_ += bytes;
	}

	// 检测持续发送的数据包字节数是否已经超过阀值
	if (g_sendWindowBytesOverflow > 0 && lastTickBytesSent_ >= g_sendWindowBytesOverflow)
	{
		BOOST_LOG_TRIVIAL(error) << "Channel::onPacketSent["
			<< static_cast<void *>(this)
			<< "]: channel("
			<< c_str()
			<< "), sentBytes has overflowed("
			<< lastTickBytesSent_
			<< " > "
			<< g_sendWindowBytesOverflow << ").";
		condemn();
	}
}

void Channel::onSendCompleted()
{
	assert((bundles_.size() == 0) && sending());
	stopSend();
}

void Channel::processPackets(std::shared_ptr<MessageHandler> pMsgHandler)
{
	BOOST_LOG_TRIVIAL(debug) << "Channel::processPackets["
		<< static_cast<void *>(this)
		<< "]: channel("
		<< c_str() << ").";

	lastTickBytesReceived_ = 0;
	lastTickBytesSent_ = 0;

	if (this->isDestroyed())
	{
		BOOST_LOG_TRIVIAL(error) << "Channel::processPackets["
			<< static_cast<void *>(this)
			<< "]: channel("
			<< c_str()
			<< ") has destroyed.";
		return;
	}

	if (this->isCondemn())
	{
		BOOST_LOG_TRIVIAL(error) << "Channel::processPackets["
			<< static_cast<void *>(this)
			<< "]: channel("
			<< c_str()
			<< ") is condemn.";
		return;
	}

	try
	{
		for (size_t i = 0; i < bufferedReceives_.size(); ++i)
		{
			pPacketReader_->processMessages(pMsgHandler, bufferedReceives_[i]);
		}
	}
	catch (std::exception &e)
	{
		BOOST_LOG_TRIVIAL(error) << "Channel::processPackets["
			<< static_cast<void *>(this)
			<< "]: channel("
			<< c_str()
			<< "), packet invalid. "
			<< e.what();
		condemn();
	}
	bufferedReceives_.clear();
}

void Channel::stopRecv()
{
	if (!recving())
		return;

	flags_ &= ~FLAG_RECVING;

	networkInterface_.pPoller()->deregisterForRead(*pEndPoint_);
}

void Channel::send(std::shared_ptr<Packet> pPacket)
{
	BOOST_LOG_TRIVIAL(debug) << "Channel::send["
		<< static_cast<void *>(this)
		<< "]: channel("
		<< c_str() << ").";

	if (this->isDestroyed())
	{
		BOOST_LOG_TRIVIAL(error) << "Channel::send["
			<< static_cast<void *>(this)
			<< "]: channel("
			<< c_str()
			<< ") has destroyed.";
		clearBundle();
		return;
	}

	if (this->isCondemn())
	{
		BOOST_LOG_TRIVIAL(error) << "Channel::send["
			<< static_cast<void *>(this)
			<< "]: channel("
			<< c_str()
			<< ") is condemn.";
		clearBundle();
		return;
	}

	if (pPacket)
	{
		bundles_.push_back(pPacket);
	}

	size_t bundleSize = bundles_.size();
	if (bundleSize == 0)
		return;

	// 检测未发送的数据包个数是否超过阀值
	if (g_sendWindowMessagesOverflowCritical > 0 && bundleSize > g_sendWindowMessagesOverflowCritical)
	{
		BOOST_LOG_TRIVIAL(warning) << "Channel::send["
			<< static_cast<void *>(this)
			<< "]: channel("
			<< c_str()
			<< "), send-window bufferedMessages has overflowed("
			<< bundleSize
			<< " > "
			<< g_sendWindowMessagesOverflowCritical
			<< ").";

		if (g_sendWindowMessagesOverflow > 0 && bundleSize > g_sendWindowMessagesOverflow)
		{
			BOOST_LOG_TRIVIAL(error) << "Channel::send["
				<< static_cast<void *>(this)
				<< "]: channel("
				<< c_str()
				<< "), send-window bufferedMessages has overflowed("
				<< bundleSize
				<< " > "
				<< g_sendWindowMessagesOverflow << ").";
			condemn();
		}
	}

	// 检测未发送的数据包总字节数是否超过阀值
	size_t bundleBytes = bundlesLength();
	if (g_sendWindowBytesOverflow > 0 && bundleBytes >= g_sendWindowBytesOverflow)
	{
		BOOST_LOG_TRIVIAL(error) << "Channel::send["
			<< static_cast<void *>(this)
			<< "]: channel("
			<< c_str()
			<< "), bufferedBytes has overflowed("
			<< bundleBytes
			<< " > "
			<< g_sendWindowBytesOverflow << ").";
		condemn();
	}

	if (!sending())
	{
		BOOST_LOG_TRIVIAL(debug) << "Channel::send["
			<< static_cast<void *>(this)
			<< "]: channel("
			<< c_str() << "), process send.";

		if (pPacketSender_ == NULL)
			pPacketSender_ = std::make_shared<TCPPacketSender>(shared_from_this());
		pPacketSender_->processSend(false);
		if (bundles_.size() > 0 && !isCondemn() && !isDestroyed())
		{
			flags_ |= FLAG_SENDING;
			networkInterface_.pPoller()->registerForWrite(*pEndPoint_, pPacketSender_);
		}
	}
}

void Channel::stopSend()
{
	if (!sending())
		return;

	flags_ &= ~FLAG_SENDING;

	networkInterface_.pPoller()->deregisterForWrite(*pEndPoint_);
}

void Channel::clearState()
{
	BOOST_LOG_TRIVIAL(debug) << "Channel::clearState["
		<< static_cast<void *>(this)
		<< "]: channel("
		<< c_str() << ")";

	if (pEndPoint_)
	{
		// 停止发送，并清空发送缓存
		stopSend();
		clearBundle();

		// 停止接收，并清空接收缓存
		stopRecv();
		bufferedReceives_.clear();

		// 释放终端
		pEndPoint_ = NULL;
	}
	
	// 清空统计信息
	lastReceivedTime_ = TIME_NOW;
	numPacketsSent_ = 0;
	numPacketsReceived_ = 0;
	numBytesSent_ = 0;
	numBytesReceived_ = 0;
	lastTickBytesReceived_ = 0;
	lastTickBytesSent_ = 0;
}

}
}