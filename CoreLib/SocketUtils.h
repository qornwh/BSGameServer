#pragma once
#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <arpa/inet.h>

class SocketUtils
{
public:
	static int CreateSocketFd()
	{
		int _serverFd = socket(PF_INET, SOCK_STREAM, 0);
		return _serverFd;
	}

	static bool SetNonBlockSocket(int _serverFd)
	{
		// server fd Non-Blocking Socket으로 설정. 대기 x Edge Trigger 사용하기 위해 설정.
		int sFlags = fcntl(_serverFd, F_GETFL);
		sFlags |= O_NONBLOCK;
		if (fcntl(_serverFd, F_SETFL, sFlags) < 0)
			return false;
		return true;
	}

	static bool SetReuseAddress(int _serverFd, int option)
	{
		int errorCheck = setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
		if (errorCheck < 0)
		{
			perror("setsockopt");
			return false;
		}

		if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEPORT, &option, sizeof(option)) < 0)
		{
			perror("setsockopt(SO_REUSEPORT) failed");
			return false;
		}

		return true;
	}

	static bool Bind(int _serverFd, sockaddr_in &serverAddr)
	{
		int errorCheck = bind(_serverFd, reinterpret_cast<const sockaddr *>(&serverAddr), sizeof(serverAddr));
		if (errorCheck < 0)
			return false;
		return true;
	}

	static bool Listen(int _serverFd)
	{
		if (listen(_serverFd, 5) < 0)
			return false;
		return true;
	}
};
