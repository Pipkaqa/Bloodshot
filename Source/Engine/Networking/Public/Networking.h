#pragma once

#ifdef BS_NETWORKING_ON
#include "Casts.h"
#include "Platform/Platform.h"
#include "Templates/Singleton.h"
#include "Templates/SmartPointers.h"

THIRD_PARTY_INCLUDES_START
#include <enet/enet.h>
THIRD_PARTY_INCLUDES_END

#include <cstdint>

namespace Bloodshot::Networking::Private
{
	class IClient abstract final
	{
	public:
		NODISCARD FORCEINLINE static bool IsConnected()
		{
			return Data->bConnected;
		}

		static void Connect(std::string_view Name,
			std::string_view IP,
			const uint32_t Port,
			const uint16_t WaitTime);

		static void Disconnect();

		static void SendPacket(const enet_uint8 ChannelID,
			const TReference<void> Data,
			const uint32_t DataLength,
			const ENetPacketFlag Flags);

	private:
		struct FData final
		{
			std::string Name = {};
			std::string ConnectedIP = {};

			bool bConnected = false;

			ENetHost* Host = nullptr;
			ENetAddress Address = {};
			ENetEvent Event = {};
			ENetPeer* Server = nullptr;
		};

		static inline FData* Data = nullptr;
	};

	class IServer abstract final
	{
	public:
		NODISCARD FORCEINLINE static bool IsRunning()
		{
			return Data;
		}

		static void Run(std::string_view Name,
			const uint32_t Host,
			const uint32_t Port);

		static void Stop();

		static TReference<ENetEvent> GetEvent();

		static void Send(TReference<ENetPeer> DestinationPeer,
			const enet_uint8 ChannelID,
			const TReference<void> Data,
			const uint32_t DataLength,
			const ENetPacketFlag Flags);

		static void Broadcast(const enet_uint8 ChannelID,
			const TReference<void> Data,
			const uint32_t DataLength,
			const ENetPacketFlag Flags);

	private:
		struct FData final
		{
			std::string Name = {};

			ENetHost* Host = nullptr;
			ENetAddress Address = {};
			ENetEvent Event = {};
		};

		static inline FData* Data = nullptr;
	};
}
#endif
