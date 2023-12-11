#pragma once

#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
#include <memory>

using namespace std;

const int MAX_CLIENT = 500;
const int TIMEOUT = 10;

class EpollFD : public enable_shared_from_this<EpollFD>
{
public:
	EpollFD();
	~EpollFD();

	bool Register(int socketFd);
	void Delete(int socketFd);

	int EpollWait();

public:
	int getEpoollFD() { return _epollFd; }
	epoll_event &getEpollEvent(int i)
	{
		// 사실상 _epollEvents 리스트는 연결된  socket 개수 리스트 이기때문에 이걸로
		// 실제 socket fd 찾기는 불가 다른 map으로 만듬
		return _epollEvents[i];
	}

protected:
	int _epollFd;
	struct epoll_event _epollEvents[MAX_CLIENT];
};
