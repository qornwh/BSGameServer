#pragma once

#include "EpollFD.h"

class ServerSock : public EpollFD
{
public:
	ServerSock() :EpollFD() {};
	~ServerSock();

public:
	bool StartListener();
	int AccpetClient();

public:
	bool isServerFd(int i)
	{
		return _epollEvents[i].data.fd == _serverFd;
	}

	bool isEpollIn(int i)
	{
		return _epollEvents[i].events & EPOLLIN;
	}

	int getServerFd()
	{
		return _serverFd;
	}

protected:
	int _serverFd;
};
