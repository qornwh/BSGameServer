#include "ServerSock.h"
#include "SocketUtils.h"

ServerSock::~ServerSock()
{
	close(_serverFd);
}

bool ServerSock::StartListener()
{
	_serverFd = SocketUtils::CreateSocketFd();
	if (_serverFd < 0)
		return false;

	if (!SocketUtils::SetNonBlockSocket(_serverFd))
	{
		close(_serverFd);
		return false;
	}

	if (!SocketUtils::SetReuseAddress(_serverFd, 1))
	{
		close(_serverFd);
		return false;
	}

	// broken pipe 에러 해결 옵션인줄 알았지만 'SO_NOSIGPIPE' 옵션이 없다 폐기
	//int set = 1;
	//setsockopt(_serverFd, SOL_SOCKET, SO_NOSIGPIPE, (void*)&set, sizeof(int));

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(12127);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	linger lin = { 0, };
	lin.l_onoff = 1; // Linger 옵션 사용 여부
	lin.l_linger = 0; // Linger Timeout 설정
	if (setsockopt(_serverFd, SOL_SOCKET, SO_LINGER, (char*)&lin, sizeof(lin)) == -1)
	{
		close(_serverFd);
		return false;
	}

	if (!SocketUtils::Bind(_serverFd, serverAddr))
	{
		close(_serverFd);
		return false;
	}

	if (!SocketUtils::Listen(_serverFd))
	{
		close(_serverFd);
		return false;
	}

	if (!Register(_serverFd))
	{
		close(_serverFd);
		return false;
	}

	return true;
}

int ServerSock::AccpetClient()
{
	int clientFd, clientLen;
	struct sockaddr_in client_addr;

	clientLen = sizeof(client_addr);
	clientFd = accept(_serverFd, reinterpret_cast<sockaddr*>(&client_addr), reinterpret_cast<socklen_t*>(&clientLen));

	//cout << "User Accpet : " << client_addr.sin_addr.s_addr << ":" << client_addr.sin_port << endl;
	cout << "Client fd : " << clientFd << endl;
	return clientFd;
}