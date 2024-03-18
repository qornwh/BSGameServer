#pragma once

#include <functional>

#include "Pch.h"
#include "ServerSock.h"
#include "SocketUtils.h"
#include "LockUtils.h"

class Service : public enable_shared_from_this<Service>
{
public:
	Service(){};
	~Service(){};

public:
	virtual void Broadcast(SendBufferRef buffer);
	virtual void Broadcast(SendBufferRef buffer, int socketFd);
	virtual bool Start() { return false; }

public:
	void CloseService();
	void AddSession(SessionRef session);
	virtual void ReleaseSession(SessionRef session);
	SessionRef GetSession(int socketFd);
	virtual SessionRef CreateSession(int32 i);
	unordered_map<int32, SessionRef> GetSessionMap();

protected:
	unordered_map<int32, SessionRef> _sessionMap;
	unordered_map<uint32, int32> _addrMap;
	int32 _sessionCount = 0;

protected:
	Lock lock;
};

class ServerService : public Service
{
public:
	ServerService();
	~ServerService();

public:
	bool Start() override;
	int EventCount()
	{
		return _serverSock->EpollWait();
	}
	bool isServerFd(int i)
	{
		return _serverSock->isServerFd(i);
	}
	bool isEpollIn(int i)
	{
		return _serverSock->isEpollIn(i);
	}
	void reSet()
	{
		shared_from_this().reset();
	}

public:
	void AcceptClient(int idx);
	void ReadClient(int idx);
	bool Register(int clientFd);
	void Dispatch();
	void ReleaseSession(SessionRef session);

	virtual void ReleaseSessionMesssage(SessionRef session){};
	bool CheckReading(bool value);
	void OffReading();

private:
	shared_ptr<ServerSock> _serverSock;
	Atomic<bool> _reading{false};
};