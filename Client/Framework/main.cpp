#include <iostream>
#include <SFML\Graphics.hpp>
#include <enet\enet.h>

using namespace std;


// THE CLIENT WINDOW



struct Vector2 {
	float x;
	float y;
};

void main()
{
	Vector2 clientControlPos;
	clientControlPos.x = 0.0f;
	clientControlPos.y = 0.0f;

	sf::RenderWindow window(sf::VideoMode(800, 600), "Client Window");
	sf::Event e;

	sf::RectangleShape background;
	background.setSize(sf::Vector2f(800, 600));
	background.setPosition(0.0f, 0.0f);


	sf::Texture backgroundTexture;
	if (!backgroundTexture.loadFromFile("../Textures/background.png"))
	{
		cout << "Texture did not load!" << "\n";
	}

	background.setTexture(&backgroundTexture);	

	if (enet_initialize() != 0)
	{
		cout << "Enet failed to initialise!" << "\n\n";
	}

	/* Declairing the ENet variables. */

	ENetAddress address;
	ENetHost* client;
	ENetPeer* peer;
	ENetEvent enetEvent;
	ENetPacket* dataPacket;

	client = enet_host_create(NULL,	1, 2, 0, 0);

	if (client == NULL)
	{
		cout << "Client failed to initialise!" << "\n\n";
	}

	enet_address_set_host(&address, "localhost");
	address.port = 1234;


	/* Trying to connect to the server. Establish the connection to the address,
	set the number of channels.*/

	peer = enet_host_connect(client, &address, 2, 0);

	if (peer == NULL) {
		cout << "No available peers for initializing an ENet connection.\n";
	}

	/*  data from our received packet is going to be stored, if it reaches us. */

	Vector2* newPosition = new Vector2;

	while (window.isOpen()) {

		window.pollEvent(e);


		if (e.type == sf::Event::Closed) {
			
			if (peer != NULL)
			{
				enet_peer_disconnect_now(peer, 0);
			}

			window.close();
		}

		while (enet_host_service(client, &enetEvent, 0) > 0)
		{
			switch (enetEvent.type) {


			}
		}


		// Controlling Player2
		clientControlPos.x = 0;
		clientControlPos.y = 0;

		if (e.type == sf::Event::KeyPressed)
		{

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			{
				if (peer != NULL)
				{
					enet_peer_disconnect_now(peer, 0);
				}

				window.close();
			}
		}

		

		window.clear(sf::Color::Green);


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			clientControlPos.x = -1;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			clientControlPos.x = 1;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			clientControlPos.y = 1;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			clientControlPos.y = -1;
		}


		dataPacket = enet_packet_create(&clientControlPos, sizeof(Vector2), ENET_PACKET_FLAG_RELIABLE);
		enet_host_broadcast(client, 0, dataPacket);

		window.draw(background);
		window.display();

	}

	/* Delete newPosition, destroy the client instance, and deinitialise ENet. */

	delete newPosition;

	enet_host_destroy(client);
	atexit(enet_deinitialize);
}