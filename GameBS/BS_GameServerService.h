#pragma once

#include "../CoreLib/Pch.h"
#include "../CoreLib/Service.h"

class BS_GameServerService : public ServerService
{
public:
	BS_GameServerService();
	~BS_GameServerService();

	shared_ptr<BS_GameServerService> GetGameServerServiceRef() { return static_pointer_cast<BS_GameServerService>(shared_from_this()); }

public:
	void Broadcast(SendBufferRef buffer) override;
	void Broadcast(SendBufferRef buffer, int32 socketFd) override;
	SessionRef CreateSession(int32 i) override;

	void ReleaseSessionMesssage(SessionRef session) override;
};
