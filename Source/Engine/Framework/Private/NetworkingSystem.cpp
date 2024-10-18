#ifdef BS_NETWORKING_ON
#include "NetworkingSystem.h"
#include "NetworkingPackets.h"

// BSTODO: Implement

namespace Bloodshot::Networking::Private
{
	void FNetworkingSystem::Execute(float DeltaTime)
	{
		//if (FNetwork::Instance->NetInterfaceType == ENetInterfaceType::None || !FNetwork::IsCreated())
		//{
		//	BS_LOG(Warning, "Failed to update Networking, NetInterface isn't created, you can remove NetworkingSystem using");
		//}
		//
		//INetInterface* NetInterface = FNetwork::Instance->NetInterface;
		//
		//ENetEvent& NetEvent = NetInterface->Event;
		//
		//while (enet_host_service(NetInterface->Host, &NetEvent, 0) > 0)
		//{
		//	if (FNetwork::Instance->NetInterfaceType == ENetInterfaceType::Client)
		//	{
		//		switch (NetEvent.type)
		//		{
		//			case ENET_EVENT_TYPE_RECEIVE:
		//			{
		//				ENetPacket* NetPacket = NetEvent.packet;
		//
		//				FNetPacketHeader NetPacketHeader = FNetwork::GetNetPacketHeader(NetPacket);
		//
		//				//	FNetPacketTransform NetPacketTransform;
		//				//	memcpy(&NetPacketTransform, NetPacket->data, NetPacket->dataLength);
		//				//
		//				//	BS_LOG(Trace, "Received packet of length {0} containing Transform: a - {1}, b - {2}, c - {3} from {4}:{5} on channel {6}",
		//				//		NetEvent.packet->dataLength,
		//				//		NetPacketTransform.a,
		//				//		NetPacketTransform.b,
		//				//		NetPacketTransform.c,
		//				//		NetEvent.peer->address.host,
		//				//		NetEvent.peer->address.port,
		//				//		NetEvent.channelID);
		//				//
		//
		//				enet_packet_destroy(NetPacket);
		//
		//				break;
		//			}
		//		}
		//	}
		//	else
		//	{
		//		switch (NetEvent.type)
		//		{
		//			case ENET_EVENT_TYPE_CONNECT:
		//			{
		//				BS_LOG(Trace, "A new client connected from {0}:{1}",
		//					NetEvent.peer->address.host,
		//					NetEvent.peer->address.port);
		//
		//				//FNetwork::SendNetPacket<FNetPacketTransform>(NetEvent.peer, 0, ENetPacketTypeDefault::Transform, ENET_PACKET_FLAG_RELIABLE, 95, 104, 1);
		//
		//				break;
		//			}
		//			case ENET_EVENT_TYPE_DISCONNECT:
		//			{
		//				BS_LOG(Trace, "Client from {0}:{1} disconnected",
		//					NetEvent.peer->address.host,
		//					NetEvent.peer->address.port);
		//
		//				break;
		//			}
		//			case ENET_EVENT_TYPE_RECEIVE:
		//			{
		//				BS_LOG(Trace, "Recieved packet of length {0} containing {1} from {2} on channel {3}",
		//					NetEvent.packet->dataLength,
		//					(char*)NetEvent.packet->data,
		//					(char*)NetEvent.peer->data,
		//					NetEvent.channelID);
		//
		//				break;
		//			}
		//		}
		//	}
		//}
	}
}
#endif
