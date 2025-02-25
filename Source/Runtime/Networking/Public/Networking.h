#pragma once

#ifdef BS_NETWORKING_ON

#include "Core.h"

THIRD_PARTY_INCLUDES_START
#include <enet/enet.h>
THIRD_PARTY_INCLUDES_END

namespace Bloodshot::Private::Networking
{
	class IClient final
	{
	public:
		NODISCARD FORCEINLINE static bool IsConnected()
		{
			return Data->bConnected;
		}

		static void Connect(FStringView Name,
			FStringView IP,
			const uint32_t Port,
			const uint16_t WaitTime);

		static void Disconnect();

		static void SendPacket(const enet_uint8 ChannelID,
			void* Data,
			const uint32_t DataLength,
			const ENetPacketFlag Flags);

	private:
		struct FData final
		{
			FString Name = {};
			FString ConnectedIP = {};

			bool bConnected = false;

			ENetHost* Host = nullptr;
			ENetAddress Address = {};
			ENetEvent Event = {};
			ENetPeer* Server = nullptr;
		};

		static inline FData* Data = nullptr;
	};

	class IServer final
	{
	public:
		NODISCARD FORCEINLINE static bool IsRunning()
		{
			return Data;
		}

		static void Run(FStringView Name,
			const uint32_t Host,
			const uint32_t Port);

		static void Stop();

		static ENetEvent* GetEvent();

		static void Send(ENetPeer* DestinationPeer,
			const enet_uint8 ChannelID,
			void* Data,
			const uint32_t DataLength,
			const ENetPacketFlag Flags);

		static void Broadcast(const enet_uint8 ChannelID,
			void* Data,
			const uint32_t DataLength,
			const ENetPacketFlag Flags);

	private:
		struct FData final
		{
			FString Name = {};

			ENetHost* Host = nullptr;
			ENetAddress Address = {};
			ENetEvent Event = {};
		};

		static inline FData* Data = nullptr;
	};
}

#endif
