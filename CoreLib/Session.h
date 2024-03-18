#pragma once

#include "Pch.h"
#include "Service.h"
#include "RecvBuffer.h"
#include "SendBuffer.h"
#include "LockUtils.h"

using namespace std;

class Session : public enable_shared_from_this<Session>
{
	enum
	{
		BUFFER_SIZE = 0x10000, // 64KB
	};

public:
	Session(int socketFd);
	~Session();

public:
	ServiceRef GetService() { return _service.lock(); }
	void SetService(ServiceRef service) { _service = service; }
	int getSocketFd() { return _socketFd; }
	bool IsConnected() { return _connected; }

public:
	bool Send(SendBufferRef buffer);
	void PushBuffer(SendBufferRef buffer);
	void Connect();
	bool ReciveMessage();
	virtual int32 OnRecv(BYTE *buffer, int32 len) { return len; }
	void Disconnect();

private:
	/* 읽기 버퍼 */
	RecvBuffer _recvBuffer;

	/* 쓰기버퍼 */
	vector<SendBufferRef> _sendBuffers;

private:
	int _socketFd;
	weak_ptr<Service> _service;
	Atomic<bool> _connected{false};

	// USE_LOCK;
	Lock lock;
};

class PacketSession : public Session
{
public:
	PacketSession(int socketFd);
	~PacketSession();

	PacketSessionRef GetPacketSessionRef() { return static_pointer_cast<PacketSession>(shared_from_this()); }

public:
	virtual int32 OnRecv(BYTE *buffer, int32 len);
	virtual void OnRecvPacket(BYTE *buffer, int32 len) = 0;
};