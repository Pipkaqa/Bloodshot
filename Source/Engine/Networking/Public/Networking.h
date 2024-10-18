#pragma once

#ifdef BS_NETWORKING_ON
#include "Casts.h"
#include "Platform.h"
#include "Templates/Singleton.h"
#include "Templates/SmartPointers.h"

THIRD_PARTY_INCLUDES_START
#include <enet/enet.h>
THIRD_PARTY_INCLUDES_END

#include <cstdint>

// BSTODO: Finish implementation

namespace Bloodshot::Networking
{
	enum class ENetInterfaceType : uint8_t
	{
		None = 0,
		Client,
		Server
	};

	struct INetPacketType abstract
	{
		uint16_t Value;

		INetPacketType& operator=(const INetPacketType& Other)
		{
			Value = Other.Value;
			return *this;
		}

		INetPacketType& operator=(uint16_t Value)
		{
			this->Value = Value;
			return *this;
		}

		bool operator==(uint16_t Value)
		{
			return this->Value == Value;
		}
	};

	struct FNetPacketHeader final
	{
		uint16_t Type;
	};

	struct INetPacket abstract
	{
		FNetPacketHeader Header;
	};

	template<typename T>
	concept CIsNetPacket = std::is_base_of_v<INetPacket, T>;

	class INetInterface abstract
	{
		friend class FNetwork;
		friend class FNetworkingSystem;

	public:
		NODISCARD FORCEINLINE const ENetHost* GetHost() const noexcept
		{
			return Host;
		}

		NODISCARD FORCEINLINE const ENetAddress& GetAddress() const noexcept
		{
			return Address;
		}

		NODISCARD FORCEINLINE const ENetEvent& GetEvent() const noexcept
		{
			return Event;
		}

	protected:
		ENetHost* Host;
		ENetAddress Address;
		ENetEvent Event;
	};

	class FNetClient final : public INetInterface
	{
		friend class FNetwork;
		friend class FNetworkingSystem;

		FNetClient(std::string_view Name);
		~FNetClient();

		ENetPeer* Peer = nullptr;
	};

	class FNetServer final : public INetInterface
	{
		friend class FNetwork;
		friend class FNetworkingSystem;

		FNetServer(std::string_view Name, const uint32_t Host, const uint32_t Port);
		~FNetServer();
	};

	class FNetwork final : public TSingleton<FNetwork>
	{
		friend class FNetworkingSystem;

	public:
		FNetwork();

		virtual void Init() override;
		virtual void Dispose() override;

		//NODISCARD FORCEINLINE static bool IsCreated()
		//{
		//	return Instance->NetInterface;
		//}
		//
		//NODISCARD FORCEINLINE static ENetInterfaceType GetType()
		//{
		//	return Instance->NetInterfaceType;
		//}
		//
		//NODISCARD FORCEINLINE static bool IsConnected()
		//{
		//	return Instance->bConnected;
		//}

		//static INetInterface* CreateNetClient(std::string_view Name);
		//static INetInterface* CreateNetServer(std::string_view Name, const uint32_t Host, const uint32_t Port);
		//
		//static void Connect(std::string_view HostName, const uint32_t Port, const uint16_t WaitTime);
		//static void Disconnect();
		//
		//static void DestroyNetInterface();
		//
		//template<CIsNetPacket T, typename... ArgTypes>
		//static void SendNetPacket(const ENetPeer* Peer,
		//	const enet_uint8 ChannelID,
		//	const INetPacketType* Type,
		//	const ENetPacketFlag Flags,
		//	ArgTypes&&... ArgTypes)
		//{
		//	T NetPacket({*Type}, std::forward<ArgTypes>(ArgTypes));
		//	ENetPacket* NetPacket = enet_packet_create(&NetPacket, sizeof(T), Flags);
		//	enet_peer_send(Peer, ChannelID, NetPacket);
		//}
		//
		//NODISCARD FORCEINLINE static FNetPacketHeader GetNetPacketHeader(ENetPacket* const SourceNetPacket)
		//{
		//	FNetPacketHeader NetPacketHeader;
		//	memcpy(&NetPacketHeader, SourceNetPacket->data, sizeof(FNetPacketHeader));
		//	return NetPacketHeader;
		//}
		//
		//template<CIsNetPacket T>
		//NODISCARD FORCEINLINE static T GetNetPacket(ENetPacket* const SourceNetPacket)
		//{
		//	//T NetPacket;
		//	//memcpy(&NetPacket, SourceNetPacket->data, SourceNetPacket->dataLength);
		//	//return NetPacket;
		//
		//	return *ReinterpretCast<T*>(SourceNetPacket->data);
		//}

	private:
		//INetInterface* NetInterface = nullptr;
		//ENetInterfaceType NetInterfaceType = ENetInterfaceType::None;
		//
		//bool bConnected = false;
	};
}
#endif
