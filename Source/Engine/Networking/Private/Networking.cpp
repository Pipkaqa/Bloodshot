#ifdef BS_NETWORKING_ON
#include "Networking.h"

// BSTODO: Finish implementation

namespace Bloodshot::Networking
{
	FNetClient::FNetClient(std::string_view Name)
	{
		BS_LOG_IF(!(Host = enet_host_create(nullptr, 1, 2, 0, 0)), Fatal, "Failed to create NetClient: {0}", Name);
	}

	FNetClient::~FNetClient()
	{
		//if (FNetwork::IsConnected())
		//{
		//	FNetwork::Disconnect();
		//}

		enet_host_destroy(Host);
	}

	FNetServer::FNetServer(std::string_view Name, const uint32_t Host, const uint32_t Port)
	{
		Address.host = Host;
		Address.port = Port;

		BS_LOG_IF(!(this->Host = enet_host_create(&Address, 32, 2, 0, 0)),
			Fatal,
			"Failed to create NetServer: {0}",
			Name);
	}

	FNetServer::~FNetServer()
	{
		enet_host_destroy(Host);
	}

	FNetwork::FNetwork()
	{
		Instance = this;
	}

	void FNetwork::Init()
	{
		BS_LOG(Debug, "Initializing Network...");

		BS_LOG_IF(enet_initialize(), Fatal, "Failed to initialize enet!");
	}

	void FNetwork::Dispose()
	{
		BS_LOG(Debug, "Destroying Network...");

		//if (IsCreated()) delete NetInterface;

		enet_deinitialize();
	}

	//INetInterface* FNetwork::CreateNetClient(std::string_view Name)
	//{
	//	BS_LOG_IF(IsCreated(), Fatal, "An attempt to create second NetInterface");
	//
	//	BS_LOG(Debug, "Creating the Client: {0}...", Name);
	//
	//	Instance->NetInterfaceType = ENetInterfaceType::Client;
	//
	//	return Instance->NetInterface = new FNetClient(Name);
	//}
	//
	//INetInterface* FNetwork::CreateNetServer(std::string_view Name, const uint32_t Host, const uint32_t Port)
	//{
	//	BS_LOG_IF(IsCreated(), Fatal, "An attempt to create second NetInterface");
	//
	//	BS_LOG(Debug, "Creating the Server: {0}...", Name);
	//
	//	Instance->NetInterfaceType = ENetInterfaceType::Server;
	//
	//	return Instance->NetInterface = new FNetServer(Name, Host, Port);
	//}
	//
	//void FNetwork::Connect(std::string_view HostName, const uint32_t Port, const uint16_t WaitTime)
	//{
	//	FNetClient* NetClient = ReinterpretCast<FNetClient*>(Instance->NetInterface);
	//
	//	ENetHost*& Host = NetClient->Host;
	//	ENetAddress& Address = NetClient->Address;
	//	ENetEvent& Event = NetClient->Event;
	//	ENetPeer*& Peer = NetClient->Peer;
	//
	//	enet_address_set_host(&Address, HostName.data());
	//	Address.port = Port;
	//
	//	BS_LOG_IF(!(Peer = enet_host_connect(Host, &Address, 2, 0)), Error,
	//		"No available Peers for connection initializing");
	//
	//	if (enet_host_service(Host, &Event, WaitTime) > 0 && Event.type == ENET_EVENT_TYPE_CONNECT)
	//	{
	//		BS_LOG(Debug, "Connected to {0}:{1}", HostName, Port);
	//	}
	//	else
	//	{
	//		enet_peer_reset(Peer);
	//		BS_LOG(Error, "Failed to connect to {0}:{1}", HostName, Port);
	//		return;
	//	}
	//
	//	Instance->bConnected = true;
	//}
	//
	//void FNetwork::Disconnect()
	//{
	//	FNetClient* NetClient = ReinterpretCast<FNetClient*>(Instance->NetInterface);
	//
	//	const ENetEvent& NetEvent = NetClient->Event;
	//
	//	enet_peer_disconnect(NetClient->Peer, 0);
	//
	//	Instance->bConnected = false;
	//}
	//
	//void FNetwork::DestroyNetInterface()
	//{
	//
	//}
}
#endif
