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
	    �����յ������ݰ����뻺�棬��������ݶѻ��Ƿ񳬹���ֵ
	*/
	void addReceiveWindow(std::shared_ptr<Packet> pPacket);

	/**
	    ��ȡ���ջ���
	 */
	std::vector<std::shared_ptr<Packet>>& bufferedReceives() { return bufferedReceives_; }

	/**
	    ��ȡ���ͻ���
	*/
	std::vector<std::shared_ptr<Packet>>& bundles() { return bundles_; }

	/**
	    ��ȡ���ͻ��������ݰ��ֽ���
	*/
	size_t bundlesLength();

	/**
	    ��շ��ͻ���
	*/
	void clearBundle();

	/**
	    ���ͨ��Ϊ���Ϸ�
	*/
	void condemn();

	/**
	    ����ͨ������
	*/
	const char * c_str() const;

	/**
	    ����ͨ��
	*/
	void destroy();

	/**
	    ͨ������
	*/
	bool finalise();

	/**
	    ͨ����ʼ����Ĭ��UDPͨ������Ҫ�Զ�ע�ᵽEPOLL����ΪUDPPacketReceiver��recvfrom�󴴽���UDPͨ��ֻ���������ͷ�����Ϣ���Զ˵ģ�������Ҫ������Ϣ
	*/
	bool initialize(bool registerForRead = true);

	/**
	    �ж�ͨ���Ƿ�Ϸ�
	*/
	bool isCondemn() const { return (flags_ & FLAG_CONDEMN) > 0; }

	/**
	    �ж�ͨ���Ƿ�����
	*/
	bool isDestroyed() const { return (flags_ & FLAG_DESTROYED) > 0; }

	/**
	    ��ȡͨ�����һ�λ�ȡ���ݰ���ʱ��
	*/
	TIME_STAMP lastReceivedTime() const { return lastReceivedTime_; }

	/**
	    ��ȡͨ����������ӿ�
	*/
	NetworkInterface& networkInterface() { return networkInterface_; }

	/**
	    ͨ��ͳ����Ϣ
	*/
	uint32_t	numBytesReceived() const { return numBytesReceived_; }
	uint32_t	numBytesSent() const { return numBytesSent_; }
	uint32_t	numPacketsReceived() const { return numPacketsReceived_; }
	uint32_t	numPacketsSent() const { return numPacketsSent_; }

	/**
	    ͨ�����ݰ����ջص�����
	*/
	void onPacketReceived(int bytes);

	/**
	    ͨ�����ݷ��ͻص�����
	*/
	void onPacketSent(int bytes, bool sentCompleted);

	/**
	    ͨ�����ݷ��ͻص�����
	*/
	void onSendCompleted();

	/**
	    ���Դ�ȡ
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
	    ��ȡͨ��Э������
	*/
	ProtocolType protocoltype() { return protocoltype_; }

	/**
	    �����ϲ�ҵ���߼���������յ������ݰ�
	*/
	void processPackets(std::shared_ptr<MessageHandler> pMsgHandler);

	/**
	    �ж�ͨ���Ƿ�ע���˽����ߵ�EPOLL
	*/
	bool recving() const { return (flags_ & FLAG_RECVING) > 0; }

	/**
	    ����ͨ������
	*/
	void stopRecv();

	/**
	    �������ݰ�
	*/
	void send(std::shared_ptr<Packet> pPacket = NULL);

	/**
	    �ж�ͨ���Ƿ��Ѿ��ڷ���
	*/
	bool sending() const { return (flags_ & FLAG_SENDING) > 0; }

	/**
	    ����ͨ������
	*/
	void stopSend();

	/**
	    ����ͨ��������ݽ���ʱ��
	*/
	void updateLastReceivedTime() { lastReceivedTime_ = TIME_NOW; }

protected:
	/**
	    ����ͨ��״̬
	*/
	void clearState();

protected:
	enum Flags
	{
		FLAG_SENDING = 0x00000001,	// ������Ϣ��
		FLAG_RECVING = 0x00000002,	// ������Ϣ��
		FLAG_DESTROYED = 0x00000004,	// ͨ���Ѿ�����
		FLAG_CONDEMN = 0x00000008	// ��Ƶ���Ѿ���ò��Ϸ�
	};

	ProtocolType							protocoltype_; // ͨ��Э������
	uint32_t								flags_; // ͨ��״̬��־
	uint32_t								lastTickBytesReceived_; // �����ϴ����ݰ������½��յ����ݰ��ֽ���
	uint32_t								lastTickBytesSent_; // �����ϴ����ݰ������·��͵����ݰ��ֽ���
	uint32_t								numBytesReceived_;
	uint32_t								numBytesSent_;
	uint32_t								numPacketsReceived_;
	uint32_t								numPacketsSent_;
	NetworkInterface&						networkInterface_;
	std::vector<std::shared_ptr<Packet>>	bufferedReceives_; // ���ջ���
	std::vector<std::shared_ptr<Packet>>	bundles_; // ���ͻ���
	TIME_STAMP								lastReceivedTime_;
	std::shared_ptr<EndPoint>				pEndPoint_;
	std::shared_ptr<PacketReader>			pPacketReader_;
	std::shared_ptr<PacketReceiver>			pPacketReceiver_;
	std::shared_ptr<PacketSender>			pPacketSender_;
};

}
}