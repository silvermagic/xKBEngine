#include <cstdint>
#include <cstddef>
#include "common.h"

namespace xKBEngine { namespace network {

float g_channelInternalTimeout = 60.f;
float g_channelExternalTimeout = 60.f;

uint8_t g_channelExternalEncryptType = 0;

uint32_t g_SOMAXCONN = 5;

// network stats
size_t g_numPacketsSent = 0;
size_t g_numPacketsReceived = 0;
size_t g_numBytesSent = 0;
size_t g_numBytesReceived = 0;

size_t g_receiveWindowMessagesOverflowCritical = 32;
size_t g_receiveWindowMessagesOverflow = 65535;
size_t g_receiveWindowBytesOverflow = 256;

size_t g_sendWindowMessagesOverflowCritical = 32;
size_t g_sendWindowMessagesOverflow = 65535;
size_t g_sendWindowBytesOverflow = 65535;

// 通道发送超时重试
uint32_t g_intReSendInterval = 10;
uint32_t g_intReSendRetries = 0;
uint32_t g_extReSendInterval = 10;
uint32_t g_extReSendRetries = 3;

const char * reasonToString(Reason reason)
{
	const char * reasons[] =
	{
		"REASON_SUCCESS",
		"REASON_TIMER_EXPIRED",
		"REASON_NO_SUCH_PORT",
		"REASON_GENERAL_NETWORK",
		"REASON_CORRUPTED_PACKET",
		"REASON_NONEXISTENT_ENTRY",
		"REASON_WINDOW_OVERFLOW",
		"REASON_INACTIVITY",
		"REASON_RESOURCE_UNAVAILABLE",
		"REASON_CLIENT_DISCONNECTED",
		"REASON_TRANSMIT_QUEUE_FULL",
		"REASON_CHANNEL_LOST",
		"REASON_SHUTTING_DOWN",
		"REASON_WEBSOCKET_ERROR",
		"REASON_CHANNEL_CONDEMN"
	};

	unsigned int index = -reason;

	if (index < sizeof(reasons) / sizeof(reasons[0]))
	{
		return reasons[index];
	}

	return "REASON_UNKNOWN";
}

}
}