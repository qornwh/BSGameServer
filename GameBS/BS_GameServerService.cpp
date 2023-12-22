#include "BS_GameServerService.h"
#include "../CoreLib/Session.h"
#include "BS_GameSession.h"
#include "BS_Packet.h"
#include "BS_PacketHandler.h"
#include "../CoreLib/Job.h"
#include "BS_GameRoom.h"
#include "BS_GameRoomManger.h"

BS_GameServerService::BS_GameServerService() : ServerService()
{
}

BS_GameServerService::~BS_GameServerService()
{
}

void BS_GameServerService::Broadcast(SendBufferRef buffer)
{
	ReadLockGuard read(lock, "read");
	for (const auto &sessionPair : GetSessionMap())
	{
		int _socketFd = sessionPair.first;
		SessionRef session = GetSession(_socketFd);
		bool isSend = session->Send(buffer);
		// if (!isSend)
		//{
		//	session->Disconnect();
		//	ReleaseSession(session);
		//	// 읽어올때 끊는 걸로 가자!!
		// }
	}
}

void BS_GameServerService::Broadcast(SendBufferRef buffer, int32 socketFd)
{
	ReadLockGuard read(lock, "read");
	for (const auto &sessionPair : GetSessionMap())
	{
		int _socketFd = sessionPair.first;
		if (_socketFd != socketFd)
		{
			shared_ptr<BS_GameSession> gameSession = static_pointer_cast<BS_GameSession>(sessionPair.second);
			SessionRef session = GetSession(_socketFd);
			if (gameSession->getPlayer() != nullptr)
			{
				session->Send(buffer);
			}
			/*else
			{
				session->Disconnect();
				ReleaseSession(session);
				// 읽어올때 끊는 걸로 가자!!
			}*/
		}
	}
}

SessionRef BS_GameServerService::CreateSession(int32 i)
{
	SessionRef session = make_shared<BS_GameSession>(i);
	session->SetService(shared_from_this());
	return session;
}

void BS_GameServerService::ReleaseSessionMesssage(SessionRef session)
{
	// BS_Protocol::BS_CLOSE_PLAYER pkt;
	// pkt.Code = session->getSocketFd();

	// SendBufferRef sendBuffer = BS_PacketHandler::MakeMyPacket(pkt);

	// Broadcast(sendBuffer, session->getSocketFd());

	shared_ptr<BS_GameRoom> room = GBSRoomManger->getRoom(0);
	if (room != nullptr)
	{
		{
			BS_Protocol::BS_CLOSE_PLAYER pkt;
			pkt.Code = session->getSocketFd();

			SendBufferRef sendBuffer = BS_PacketHandler::MakePacket(pkt);

			JobRef job = make_shared<Job>(&BS_GameRoom::BroadcastAnother, room, sendBuffer, session->getSocketFd());
			room->PushJobQueue(job);
		}

		{
			JobRef job = make_shared<Job>(&BS_GameRoom::DelSession, room, static_pointer_cast<BS_GameSession>(session));
			room->PushJobQueue(job);
		}
	}
}
