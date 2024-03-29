#pragma once
#include "../CoreLib/Pch.h"

namespace BS_Protocol
{
	struct FVector
	{
		int32 X;
		int32 Y;
		int32 Z;
		float Yaw;

		int32 size()
		{
			return sizeof(FVector);
		}
	};

	struct User
	{
		uint32 Code;
		uint16 NameLen;
		BYTE *Name;

		int32 size()
		{
			return NameLen + sizeof(uint16) * 2;
		}
	};

	struct BS_USER_DATA
	{
		vector<User> Users;

		int32 size()
		{
			int32 size = 0;
			for (User user : Users)
			{
				size += user.size();
			}
			return size;
		}
	};

	// 1001 500 123 10 10 10
	struct BS_C_MOVE
	{
		int32 Code;
		FVector Position;
		int32 Target;

		int32 size()
		{
			return sizeof(BS_C_MOVE);
		}
	};

	struct BS_C_MOVE_LIST
	{
		vector<BS_C_MOVE> moveList;

		int32 size()
		{
			int32 _size = 0;
			for (auto move : moveList)
			{
				_size += sizeof(move);
			}
			return _size + sizeof(uint16);
		}
	};

	struct BS_C_CHAT
	{
		uint32 Code;
		uint8 Type;
		uint32 TextLen;
		BYTE *Text;

		int32 size()
		{
			return sizeof(uint8) + sizeof(uint32) + sizeof(uint32) + TextLen;
		}
	};

	struct Monster
	{
		Monster(uint16 Type, uint8 Hp, int32 Code) : Type(Type), Hp(Hp), Code(Code) {}
		uint16 Type;
		uint8 Hp;
		int32 Code;
		FVector Position;

		// 이름
		uint16 NameLen;
		BYTE *Name;
		int32 Target;

		int32 size()
		{
			return sizeof(uint8) + sizeof(int32) + sizeof(FVector) + sizeof(uint16) * 2 + sizeof(Name) + sizeof(int32);
		}
	};

	struct Player
	{
		Player(uint16 Type, uint8 Hp, int32 Code) : Type(Type), Hp(Hp), Code(Code) {}
		uint16 Type;
		uint8 Hp;
		int32 Code;
		FVector Position;

		// 이름
		uint16 NameLen;
		BYTE *Name;

		int32 size()
		{
			return sizeof(uint8) + sizeof(int32) + sizeof(FVector) + sizeof(uint16) * 2 + sizeof(Name);
		}
	};

	struct MapData
	{
		uint8 MapCode;
		FVector StartPoint;
		FVector EndPoint;
	};

	// [ 1005 123123 1100 5 dfskfieowenv 1101 1 dsfsdfewf ]
	struct BS_LOAD_DATA
	{
		vector<Monster> monsters;
		vector<Player> players;
		// MapData mapData;
		int32 size()
		{
			int32 _size = 0;
			for (auto player : players)
			{
				_size += sizeof(player);
			}

			int32 _size2 = 0;
			for (auto monster : monsters)
			{
				_size2 += sizeof(monster);
			}
			return _size + _size2 + sizeof(uint16) * 2;
		}
	};

	struct BS_CLOSE_PLAYER
	{
		int32 Code;

		int32 size()
		{
			return sizeof(int32);
		}
	};

	struct BS_ATTACK_UNIT
	{
		int32 Code;

		int32 SkillCode;

		int32 size()
		{
			return sizeof(BS_ATTACK_UNIT);
		}
	};

	struct BS_ATTACK_UNIT_LIST
	{
		vector<BS_ATTACK_UNIT> attackList;

		int32 size()
		{
			int32 _size = 0;
			for (auto attack : attackList)
			{
				_size += sizeof(attack);
			}
			return _size + sizeof(uint16);
		}
	};

	struct BS_HIT_UNIT
	{
		int32 TargetCode;

		int32 AttackCode;

		int32 Damage;

		int32 size()
		{
			return sizeof(BS_HIT_UNIT);
		}
	};
}
