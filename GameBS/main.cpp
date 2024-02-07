#include <string.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../CoreLib/ServerSock.h"
#include "../CoreLib/SocketUtils.h"
#include "../CoreLib/ThreadManager.h"

#include "../CoreLib/Pch.h"
#include "../CoreLib/Utils.h"
#include "../CoreLib/Job.h"

#include "BS_GameServerService.h"
#include "BS_GameRoom.h"
#include "BS_GameRoomManger.h"

void RunTask(BS_GameServerServiceRef serverRef)
{
	while (1)
	{
		serverRef->Dispatch();

		shared_ptr<BS_GameRoom> room = GBSRoomManger->getRoom(0);
		if (room != nullptr)
		{
			// 게임룸 Task
			room->RoomTask();
		}
	}
}

int main()
{
	BS_GameServerServiceRef serverRef = make_shared<BS_GameServerService>();

	ASSERT_CRASH(serverRef->Start());
	{
		ThreadManager::GetInstance().CreateThread([&serverRef]()
																							{ RunTask(serverRef); });
	}
	RunTask(serverRef);

	// 몬스터 구현
	// 0. 리눅스 타이머 구현 - OK
	// 0. 맵 지정 - OK
	// 1. 몬스터들의 이동은, 500ms마다 이동된 좌표들을 클라이언트로 던져 본다. -OK
	// 2. 몬스터들의 Hp 피격시 데미지 계산 구현 - OK
	// 3. 몬스터들의 Hp 피격 데미지 클라전송. - OK
	// 4. 몬스터 맞으면 플레이어 쫓아가도록 구현. - OK

	serverRef->reSet();

	cout << "server close !!!" << endl;
	return 0;
}
