#include "Service.h"
#include "Session.h"

ServerService::ServerService() : Service()
{
}

ServerService::~ServerService()
{
}

bool ServerService::Start()
{
	_serverSock = make_shared<ServerSock>();
	if (_serverSock->StartListener())
	{
		return true;
	}
	return false;
}

void ServerService::AcceptClient(int idx)
{
	if (isEpollIn(idx))
	{
		int clientFd = _serverSock->AccpetClient();
		ASSERT_CRASH(clientFd > 0);
		if (SocketUtils::SetNonBlockSocket(clientFd))
		{
			// socket fd, epoll 등록
			if (!Register(clientFd))
			{
				printf("client epoll_ctl() error\n");
				return;
			}
		}
	}
	return;
}

void ServerService::ReadClient(int idx)
{
	int clientFd = _serverSock->getEpollEvent(idx).data.fd;

	SessionRef session = GetSession(clientFd);
	if (session != nullptr)
	{
		// atomic으로 compare_exchange_strong을 한 이유.
		// 멀티스레드에서 동시에 읽어오는 이슈가 있어서 사용. fd읽는 도중에 또 같은 fd를 읽어버림 !!!
		bool isReading = session->IsReading();
		if (!isReading && session->CheckReading(isReading))
		{
			bool isRecv = session->ReciveMessage();
			session->OffReading();
			if (!isRecv)
				ReleaseSession(session);
		}
	}
}

bool ServerService::Register(int clientFd)
{
	// sockaddr_in client_addr;
	// socklen_t len;
	// getpeername(clientFd, (sockaddr*)&client_addr, &len);

	// if (_addrMap.find(client_addr.sin_addr.s_addr) != _addrMap.end())
	//{
	//	ReleaseSession(GetSession(_addrMap[client_addr.sin_addr.s_addr]));
	//	_addrMap.erase(client_addr.sin_addr.s_addr);
	// }

	// if (GetSessionMap().find(clientFd) != GetSessionMap().end())
	//	ReleaseSession(GetSessionMap()[clientFd]);

	if (_serverSock->Register(clientFd))
	{
		// 클라 세션 생성
		SessionRef session = CreateSession(clientFd);
		AddSession(session);
		return true;
	}
	return false;
}

void ServerService::Dispatch()
{
	int eventCount = EventCount();

	ASSERT_CRASH(eventCount > -1);

	for (int i = 0; i < eventCount; i++)
	{
		if (isServerFd(i))
		{
			AcceptClient(i);
		}
		else
		{
			ReadClient(i);
		}
	}
}

void Service::Broadcast(SendBufferRef buffer)
{
}

void Service::Broadcast(SendBufferRef buffer, int socketFd)
{
}

void Service::CloseService()
{
}

void Service::AddSession(SessionRef session)
{
	// TODO : lock 구현
	WriteLockGuard write(lock, "write");
	int32 clientFd = session->getSocketFd();
	sockaddr_in client_addr;
	socklen_t len;
	getpeername(clientFd, (sockaddr *)&client_addr, &len);
	_sessionCount++;
	_sessionMap[clientFd] = session;
	_addrMap[client_addr.sin_addr.s_addr] = clientFd;
}

void Service::ReleaseSession(SessionRef session)
{
}

SessionRef Service::GetSession(int socketFd)
{
	return _sessionMap[socketFd];
}

SessionRef Service::CreateSession(int32 i)
{
	SessionRef session = make_shared<Session>(i);
	session->SetService(shared_from_this());

	return session;
}

unordered_map<int32, SessionRef> Service::GetSessionMap()
{
	return _sessionMap;
}

void ServerService::ReleaseSession(SessionRef session)
{
	// TODO : lock 구현

	WriteLockGuard write(lock, "write");
	ASSERT_CRASH(_sessionMap.find(session->getSocketFd()) != GetSessionMap().end());
	// map은 일단 각 객체에 nullptr로 shared_ptr끊어야 카운트가 감소된다 erase는 사실상 상관없는듯 하다 해야함으로 중요 지금까지 모르고 있었다.!!!
	_sessionMap.at(session->getSocketFd()) = nullptr;
	_sessionMap.erase(session->getSocketFd());

	_sessionCount--;
	_serverSock->Delete(session->getSocketFd());

	ReleaseSessionMesssage(session);
}