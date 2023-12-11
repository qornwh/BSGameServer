#include "EpollFD.h"

EpollFD::EpollFD()
{
	_epollFd = epoll_create(MAX_CLIENT);
}

EpollFD::~EpollFD()
{
	close(_epollFd);
}

bool EpollFD::Register(int socketFd)
{
	/* --- 일단 클라 서버 통일 ----*/
	struct epoll_event events;
	events.events = EPOLLIN | EPOLLRDHUP | EPOLLET; // 수신데이터 | 연결 끊김 감지 | 에지 트리거
	events.data.fd = socketFd;

	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, socketFd, &events) < 0)
	{
		close(socketFd);
		return false;
	}
	return true;
}

void EpollFD::Delete(int socketFd)
{
	close(socketFd);
	epoll_ctl(_epollFd, EPOLL_CTL_DEL, socketFd, NULL);
}

int EpollFD::EpollWait()
{
	return epoll_wait(_epollFd, _epollEvents, MAX_CLIENT, TIMEOUT);
}
