#pragma once

#ifdef BS_NETWORKING_ON
#include <Networking.h>

#include <stdio.h>

void EnetTemplate()
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet!\n");
		return;
	}

	atexit(enet_deinitialize);

	if (getchar() == 'C')
	{
		ENetHost* client;
		client = enet_host_create(NULL, 1, 1, 0, 0);

		if (client == NULL)
		{
			fprintf(stderr, "An error occurred while trying to create an ENet client host!\n");
			return;
		}

		ENetAddress address;
		ENetEvent event;
		ENetPeer* peer;

		enet_address_set_host(&address, "127.0.0.1");
		address.port = 7777;

		peer = enet_host_connect(client, &address, 1, 0);
		if (peer == NULL)
		{
			fprintf(stderr, "No available peers for initiating an ENet connection!\n");
			return;
		}

		if (enet_host_service(client, &event, 5000) > 0 &&
			event.type == ENET_EVENT_TYPE_CONNECT)
		{
			puts("Connection to 127.0.0.1:7777 succeeded.");
		}
		else
		{
			enet_peer_reset(peer);
			puts("Connection to 127.0.0.1:7777 failed.");
			return;
		}

		while (enet_host_service(client, &event, 1000) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_RECEIVE:
					printf("A packet of length %u containing %s was received from %x:%u on channel %u.\n",
						event.packet->dataLength,
						event.packet->data,
						event.peer->address.host,
						event.peer->address.port,
						event.channelID);
					break;
			}
		}

		enet_peer_disconnect(peer, 0);

		while (enet_host_service(client, &event, 3000) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_RECEIVE:
					enet_packet_destroy(event.packet);
					break;
				case ENET_EVENT_TYPE_DISCONNECT:
					puts("Disconnection succeeded.");
					break;
			}
		}

		return;
	}
	else
	{
		ENetEvent event;
		ENetAddress address;
		ENetHost* server;

		/* Bind the server to the default localhost.     */
		/* A specific host address can be specified by   */
		/* enet_address_set_host (& address, "x.x.x.x"); */
		address.host = ENET_HOST_ANY; // This allows
		/* Bind the server to port 7777. */
		address.port = 7777;

		server = enet_host_create(&address	/* the address to bind the server host to */,
			32	/* allow up to 32 clients and/or outgoing connections */,
			1	/* allow up to 1 channel to be used, 0. */,
			0	/* assume any amount of incoming bandwidth */,
			0	/* assume any amount of outgoing bandwidth */);

		if (server == NULL)
		{
			printf("An error occurred while trying to create an ENet server host.");
			return;
		}

		// gameloop
		while (true)
		{
			ENetEvent event;
			/* Wait up to 1000 milliseconds for an event. */
			while (enet_host_service(server, &event, 1000) > 0)
			{
				switch (event.type)
				{
					case ENET_EVENT_TYPE_CONNECT:
						printf("A new client connected from %x:%u.\n",
							event.peer->address.host,
							event.peer->address.port);
						break;

					case ENET_EVENT_TYPE_RECEIVE:
						printf("A packet of length %u containing %s was received from %s on channel %u.\n",
							event.packet->dataLength,
							event.packet->data,
							event.peer->data,
							event.channelID);
						/* Clean up the packet now that we're done using it. */
						enet_packet_destroy(event.packet);
						break;

					case ENET_EVENT_TYPE_DISCONNECT:
						printf("%s disconnected.\n", event.peer->data);
						/* Reset the peer's client information. */
						event.peer->data = NULL;
				}
			}
		}

		enet_host_destroy(server);

		return;
	}
}
#endif
