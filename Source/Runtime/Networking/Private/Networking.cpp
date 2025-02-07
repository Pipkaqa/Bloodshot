#ifdef BS_NETWORKING_ON

#include "Networking.h"
#include "Logging/LoggingMacros.h"

// BSTODO: Finish implementation

namespace Bloodshot::Networking::Private
{
	void IClient::Connect(FStringView Name,
		FStringView IP,
		const uint32_t Port,
		const uint16_t WaitTime)
	{
		BS_LOG_IF(Data, Error, "Attempt to connect twice, disconnect first");

		Data = new FData();

		ENetHost*& Host = Data->Host;

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
		void* PacketData,
		const uint32_t DataLength,
		const ENetPacketFlag Flags)
	{
		ENetPacket* const Packet = enet_packet_create(PacketData, DataLength, Flags);
		enet_peer_send(Data->Server, ChannelID, Packet);
	}

	void IServer::Run(FStringView Name,
		const uint32_t Host,
		const uint32_t Port)
	{
		BS_LOG_IF(Data, Error, "Trying to start Server twice");
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
		BS_LOG_IF(!Data, Error, "Trying to stop not started Server");

		enet_host_destroy(Data->Host);

		delete Data;
	}

	ENetEvent* IServer::GetEvent()
	{
		if (!Data)
		{
			BS_LOG(Error, "Trying to get Event from not running Server");
			return nullptr;
		}

		ENetEvent& Event = Data->Event;

		if (enet_host_service(Data->Host, &Event, 0) > 0)
		{
			return &Event;
		}

		return nullptr;
	}

	void IServer::Send(ENetPeer* DestinationPeer,
		const enet_uint8 ChannelID,
		void* PacketData,
		const uint32_t DataLength,
		const ENetPacketFlag Flags)
	{
		BS_LOG(Trace, "Sending the Packet...");

		ENetPacket* const Packet = enet_packet_create(PacketData, DataLength, Flags);
		enet_peer_send(DestinationPeer, ChannelID, Packet);
	}

	void IServer::Broadcast(const enet_uint8 ChannelID,
		void* PacketData,
		const uint32_t DataLength,
		const ENetPacketFlag Flags)
	{
		BS_LOG(Trace, "Sending the Packet...");

		ENetPacket* const Packet = enet_packet_create(Data, DataLength, Flags);
		enet_host_broadcast(Data->Host, ChannelID, Packet);
	}
}

#endif
