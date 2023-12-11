#include <string.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../CoreLib/ServerSock.h"
#include "../CoreLib/SocketUtils.h"
#include "../CoreLib/ThreadManager.h"

#include "../CoreLib/Pch.h"

#include "BS_GameServerService.h"

void worker(ServerSock *serverSock)
{
	while (1)
	{
	}
}

int main()
{
	// GameRoom 미리 만들어 두기 !!!!! 및 초기화

	BS_GameServerServiceRef serverRef = make_shared<BS_GameServerService>();

	ASSERT_CRASH(serverRef->Start());
	{
		ThreadManager::GetInstance().CreateThread([&serverRef]()
																							{
			while (true)
			{
				serverRef->Dispatch();
			} });
	}
	while (1)
	{
		serverRef->Dispatch();
	}

	serverRef->reSet();

	cout << "server close !!!" << endl;
	return 0;
}
