#ifdef BS_NETWORKING_ON
#include "Logging/LoggingMacros.h"
#include "Networking.h"

// BSTODO: Finish implementation

namespace Bloodshot::Networking::Private
{
	void IClient::Connect(std::string_view Name,
		std::string_view IP,
		const uint32_t Port,
		const uint16_t WaitTime)
	{
		BS_LOG_IF(Data, Error, "Attempt to connect twice, disconnect first");

		Data = new FData();

		TReference<ENetHost> Host = Data->Host;

		BS_LOG_IF(!(Host = enet_host_create(nullptr, 1, 2, 0, 0)), Fatal, "Failed to create Client: {0}", Name);

		Data->Name = Name;
		Data->ConnectedIP = IP;

		ENetAddress& Address = Data->Address;
		ENetEvent& Event = Data->Event;
		ENetPeer*& Peer = Data->Server;

		enet_address_set_host(&Address, IP.data());
		Address.port = Port;

		BS_LOG_IF(!(Peer = enet_host_connect(Host, &Address, 2, 0)), Error, "No available Peers for connection initializing");
	}

	void IClient::Disconnect()
	{
		enet_peer_disconnect(Data->Server, 0);
		enet_host_destroy(Data->Host);

		delete Data;
	}

	void IClient::SendPacket(const enet_uint8 ChannelID,
		const TReference<void> PacketData,
		const uint32_t DataLength,
		const ENetPacketFlag Flags)
	{
		ENetPacket* const Packet = enet_packet_create(PacketData, DataLength, Flags);
		enet_peer_send(Data->Server, ChannelID, Packet);
	}

	void IServer::Run(std::string_view Name,
		const uint32_t Host,
		const uint32_t Port)
	{
		BS_LOG_IF(Data, Error, "Attempt to start second Server");
		BS_LOG(Info, "Starting the Server...");

		Data = new FData();

		ENetAddress& Address = Data->Address;

		Address.host = Host;
		Address.port = Port;

		BS_LOG_IF(!(Data->Host = enet_host_create(&Address,
			32, 2, 0, 0)),
			Fatal,
			"Failed to create Server: {0}",
			Name);
	}

	void IServer::Stop()
	{
		BS_LOG_IF(!Data, Error, "Attempt to stop not started Server");

		enet_host_destroy(Data->Host);

		delete Data;
	}

	TReference<ENetEvent> IServer::GetEvent()
	{
		if (!Data)
		{
			BS_LOG(Error, "Attempt to get Event from not running Server");
			return nullptr;
		}

		ENetEvent& Event = Data->Event;

		if (enet_host_service(Data->Host, &Event, 0) > 0)
		{
			return &Event;
		}

		return nullptr;
	}

	void IServer::Send(TReference<ENetPeer> DestinationPeer,
		const enet_uint8 ChannelID,
		TReference<void> PacketData,
		const uint32_t DataLength,
		const ENetPacketFlag Flags)
	{
		BS_LOG(Trace, "Sending packet...");

		ENetPacket* const Packet = enet_packet_create(PacketData, DataLength, Flags);
		enet_peer_send(DestinationPeer, ChannelID, Packet);
	}

	void IServer::Broadcast(const enet_uint8 ChannelID,
		const TReference<void> PacketData,
		const uint32_t DataLength,
		const ENetPacketFlag Flags)
	{
		BS_LOG(Trace, "Sending packet...");

		ENetPacket* const Packet = enet_packet_create(Data, DataLength, Flags);
		enet_host_broadcast(Data->Host, ChannelID, Packet);
	}
}
#endif
