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
		while (true)
		{
			int clientFd = _serverSock->AccpetClient();
			if (clientFd > 0)
			{
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
			else
			{
				break;
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
		while (true)
		{
			bool isRecv = session->ReciveMessage();
			if (isRecv)
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
				{
					break;
				}
			}
			else
			{
				ReleaseSession(session);
				break;
			}
		}
	}
}

bool ServerService::Register(int clientFd)
{
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
	// atomic으로 compare_exchange_strong을 한 이유.
	// 멀티스레드에서 동시에 읽어오는 이슈가 있어서 사용. fd읽는 도중에 또 같은 fd를 읽어버림 !!!
	int isReading = _reading;
	if (!_reading && CheckReading(isReading))
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
		OffReading();
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

bool ServerService::CheckReading(bool value)
{
	return _reading.compare_exchange_strong(value, true);
}

void ServerService::OffReading()
{
	_reading.exchange(false);
}