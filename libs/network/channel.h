#pragma once

#include <memory>
#include <vector>
#include "common/timer.h"
#include "common.h"

namespace xKBEngine { namespace network {

class EndPoint;
class MessageHandler;
class NetworkInterface;
class Packet;
class PacketReader;
class PacketReceiver;
class PacketSender;
class Channel : public std::enable_shared_from_this<Channel>
{
public:
	Channel(NetworkInterface& networkInterface,
		std::shared_ptr<EndPoint> pEndPoint,
		ProtocolType pt = PROTOCOL_TCP);
	~Channel();

	/**
	    将接收到的数据包放入缓存，并检测数据堆积是否超过阀值
	*/
	void addReceiveWindow(std::shared_ptr<Packet> pPacket);

	/**
	    获取接收缓存
	 */
	std::vector<std::shared_ptr<Packet>>& bufferedReceives() { return bufferedReceives_; }

	/**
	    获取发送缓存
	*/
	std::vector<std::shared_ptr<Packet>>& bundles() { return bundles_; }

	/**
	    获取发送缓存中数据包字节数
	*/
	size_t bundlesLength();

	/**
	    清空发送缓存
	*/
	void clearBundle();

	/**
	    标记通道为不合法
	*/
	void condemn();

	/**
	    返回通道描述
	*/
	const char * c_str() const;

	/**
	    销毁通道
	*/
	void destroy();

	/**
	    通道清理
	*/
	bool finalise();

	/**
	    通道初始化，默认UDP通道不需要自动注册到EPOLL，因为UDPPacketReceiver在recvfrom后创建的UDP通道只是用来发送返回信息到对端的，并不需要接收消息
	*/
	bool initialize(bool registerForRead = true);

	/**
	    判断通道是否合法
	*/
	bool isCondemn() const { return (flags_ & FLAG_CONDEMN) > 0; }

	/**
	    判断通道是否被销毁
	*/
	bool isDestroyed() const { return (flags_ & FLAG_DESTROYED) > 0; }

	/**
	    获取通道最近一次获取数据包的时间
	*/
	TIME_STAMP lastReceivedTime() const { return lastReceivedTime_; }

	/**
	    获取通道所属网络接口
	*/
	NetworkInterface& networkInterface() { return networkInterface_; }

	/**
	    通道统计信息
	*/
	uint32_t	numBytesReceived() const { return numBytesReceived_; }
	uint32_t	numBytesSent() const { return numBytesSent_; }
	uint32_t	numPacketsReceived() const { return numPacketsReceived_; }
	uint32_t	numPacketsSent() const { return numPacketsSent_; }

	/**
	    通道数据包接收回调处理
	*/
	void onPacketReceived(int bytes);

	/**
	    通道数据发送回调处理
	*/
	void onPacketSent(int bytes, bool sentCompleted);

	/**
	    通道数据发送回调处理
	*/
	void onSendCompleted();

	/**
	    属性存取
	*/
	std::shared_ptr<EndPoint> pEndPoint() const { return pEndPoint_; }
	void pEndPoint(std::shared_ptr<EndPoint> pEndPoint) { pEndPoint_ = pEndPoint; }
	std::shared_ptr<PacketReader> pPacketReader() const { return pPacketReader_; }
	void pPacketReader(std::shared_ptr<PacketReader> pPacketReader) { pPacketReader_ = pPacketReader; }
	std::shared_ptr<PacketSender> pPacketSender() const { return pPacketSender_; }
	void pPacketSender(std::shared_ptr<PacketSender> pPacketSender) { pPacketSender_ = pPacketSender; }
	std::shared_ptr<PacketReceiver> pPacketReceiver() const { return pPacketReceiver_; }
	void pPacketReceiver(std::shared_ptr<PacketReceiver> pPacketReceiver) { pPacketReceiver_ = pPacketReceiver; }

	/**
	    获取通道协议类型
	*/
	ProtocolType protocoltype() { return protocoltype_; }

	/**
	    调用上层业务逻辑来处理接收到的数据包
	*/
	void processPackets(std::shared_ptr<MessageHandler> pMsgHandler);

	/**
	    判断通道是否注册了接收者到EPOLL
	*/
	bool recving() const { return (flags_ & FLAG_RECVING) > 0; }

	/**
	    结束通道接收
	*/
	void stopRecv();

	/**
	    发送数据包
	*/
	void send(std::shared_ptr<Packet> pPacket = NULL);

	/**
	    判断通道是否已经在发送
	*/
	bool sending() const { return (flags_ & FLAG_SENDING) > 0; }

	/**
	    结束通道发送
	*/
	void stopSend();

	/**
	    更新通道最近数据接收时间
	*/
	void updateLastReceivedTime() { lastReceivedTime_ = TIME_NOW; }

protected:
	/**
	    清理通道状态
	*/
	void clearState();

protected:
	enum Flags
	{
		FLAG_SENDING = 0x00000001,	// 发送信息中
		FLAG_RECVING = 0x00000002,	// 接收消息中
		FLAG_DESTROYED = 0x00000004,	// 通道已经销毁
		FLAG_CONDEMN = 0x00000008	// 该频道已经变得不合法
	};

	ProtocolType							protocoltype_; // 通道协议类型
	uint32_t								flags_; // 通道状态标志
	uint32_t								lastTickBytesReceived_; // 距离上次数据包处理，新接收的数据包字节数
	uint32_t								lastTickBytesSent_; // 距离上次数据包处理，新发送的数据包字节数
	uint32_t								numBytesReceived_;
	uint32_t								numBytesSent_;
	uint32_t								numPacketsReceived_;
	uint32_t								numPacketsSent_;
	NetworkInterface&						networkInterface_;
	std::vector<std::shared_ptr<Packet>>	bufferedReceives_; // 接收缓存
	std::vector<std::shared_ptr<Packet>>	bundles_; // 发送缓存
	TIME_STAMP								lastReceivedTime_;
	std::shared_ptr<EndPoint>				pEndPoint_;
	std::shared_ptr<PacketReader>			pPacketReader_;
	std::shared_ptr<PacketReceiver>			pPacketReceiver_;
	std::shared_ptr<PacketSender>			pPacketSender_;
};

}
}