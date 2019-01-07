// Headers and SFML libraries
#include <iostream>
#include <SFML\Graphics.hpp>
#include <stdlib.h>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include "Player.h"
#include "Player2.h"
#include "Entity.h"
#include "Wall.h"
#include "PickUp.h"
#include <vector>
#include <string>

// FMOD libraries
#include <fmod.hpp>
#include <fmod_studio.hpp>


// ENet - Multiplayer libraries
#include <enet/enet.h>

// LUA libraries
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include <LuaBridge\LuaBridge.h>


using namespace luabridge;
using namespace std;


// Setting the Box2D World
b2Vec2 gravity(0.0f, 0.0f);
b2World world(gravity);
vector<Wall>::const_iterator iter;
vector<Wall> wallArray;
vector<PickUp>::const_iterator iter2;
vector<PickUp> pickUpArray;
vector<Entity>::const_iterator iter3;
vector<Entity> groundArray;
//Defining the textures
sf::Texture textureRoom;
sf::Texture textureGrass;
sf::Texture textureWall;
sf::Texture textureCoin;


// Funtion which creates the tiles for the ground and the floor in the custom rooms. 
// The if statements being used to differentiate between the types of tiles being used in creating the rooms and the ground
void createFloorTile(int x, int y, int tileType)
{
	// Wall Game Object
	x -= 42;
	y -= 42;
	Entity Floor1;
	if (tileType == 0)
	{
		sf::Sprite spriteFloor(textureGrass);
		Floor1.sprite = spriteFloor;
		Floor1.sprite.setPosition(x, y);
	}

	if (tileType == 1)
	{
		sf::Sprite spriteFloor(textureRoom);
		Floor1.sprite = spriteFloor;
		Floor1.sprite.setPosition(x, y);
	}

	// Wall Array
	groundArray.push_back(Floor1);
}

// Funtion for Coin Spawning
void spawnCoin(float x, float y)
{
	PickUp coin1;
	sf::Sprite spriteCoin(textureCoin);
	coin1.sprite = spriteCoin;
	coin1.rect.setPosition(sf::Vector2f(x, y));
	coin1.sprite.setPosition(coin1.rect.getPosition());
	pickUpArray.push_back(coin1);


}

// Function which creates the wall in the Box2D world
void createWallObject(float x, float y)
{
	// Box2D Wall Object
	float wallWidth = 0.09f;
	float wallHeight = 0.09f;
	b2PolygonShape wallBox;
	wallBox.SetAsBox(wallWidth, wallHeight);

	b2BodyDef wallBodyDef; // Setting the X and Y position
	wallBodyDef.position.Set(x, y);

	b2Body* wallBody = world.CreateBody(&wallBodyDef); // Declaring Box Position
	wallBody->CreateFixture(&wallBox, 0.0f); // Declaring Box Width & Height


	b2Vec2 wallPosition = wallBody->GetPosition();


	// Wall Game Object
	Wall Wall1;
	Wall1.wallBody = wallBody;
	sf::Sprite spriteWall(textureWall);
	Wall1.sprite = spriteWall;


	// Wall Array
	wallArray.push_back(Wall1);
}

// Function which allows the user to create and customize the rooms
void createCustomRoom(int roomStartXPosition, int roomStartYPosition, int roomXWidth, int roomYHeight, bool hasRightExit, int rightExitYPosition, bool hasLeftExit, int leftExitYPosition, bool hasUpperExit, int upperExitXPosition, bool hasLowerExit, int lowerExitXPosition, int numberCoinsRoom)
{
	// Spawn Coins
	for (int w = 0; w < numberCoinsRoom; w++)
	{
		int randX = (((roomStartXPosition + 0) * 64) + 15) + ((rand() % ((roomXWidth - 2) * 64)));
		int randY = (((roomStartYPosition + 0) * 64) + 15) + ((rand() % ((roomYHeight - 2) * 64)));
		spawnCoin(randX, randY);
	}


	for (int x = roomStartXPosition; x < roomStartXPosition + roomXWidth; x++)
	{
		for (int y = roomStartYPosition; y < roomStartYPosition + roomYHeight; y++)
		{

			createFloorTile((x * 64), (y * 64), 1);

			if ((x == roomStartXPosition) || (y == roomStartYPosition) || (x == roomStartXPosition + roomXWidth - 1) || (y == roomStartYPosition + roomYHeight - 1))
			{
				bool hadExit = false;
				if (hasRightExit)
				{
					if ((x == roomStartXPosition + roomXWidth - 1) && (y == roomStartYPosition + rightExitYPosition))
					{
						hadExit = true;
					}
				}
				if (hasLeftExit)
				{
					if ((x == roomStartXPosition) && (y == roomStartYPosition + leftExitYPosition))
					{
						hadExit = true;
					}
				}
				if (hasUpperExit)
				{
					if ((x == roomStartXPosition + upperExitXPosition) && (y == roomStartYPosition))
					{
						hadExit = true;
					}
				}
				if (hasLowerExit)
				{
					if ((x == roomStartXPosition + lowerExitXPosition) && (y == roomStartYPosition + roomYHeight - 1))
					{
						hadExit = true;
					}
				}
				if (!hadExit)
				{
					createWallObject(x * 0.32f, -y * 0.32f);
				}




			}


		}
	}
}


// Function which creates the map boundry
void createMapBoundary(int roomStartXPosition, int roomStartYPosition, int roomXWidth, int roomYHeight, int numberCoinsRoom)
{
	// Spawn Coins
	for (int w = 0; w < numberCoinsRoom; w++)
	{
		int randX = (((roomStartXPosition + 0) * 64) + 15) + ((rand() % ((roomXWidth - 2) * 64)));
		int randY = (((roomStartYPosition + 0) * 64) + 15) + ((rand() % ((roomYHeight - 2) * 64)));
		spawnCoin(randX, randY);
	}


	for (int x = roomStartXPosition; x < roomStartXPosition + roomXWidth; x++)
	{
		for (int y = roomStartYPosition; y < roomStartYPosition + roomYHeight; y++)
		{

		

			if ((x == roomStartXPosition) || (y == roomStartYPosition) || (x == roomStartXPosition + roomXWidth - 1) || (y == roomStartYPosition + roomYHeight - 1))
			{
				createWallObject(x * 0.32f, -y * 0.32f);
			}


		}
	}
}


struct Vector2 {
	float x;
	float y;
};

int main()
{

	Vector2 position;

	if (enet_initialize() != 0)
	{
		cout << "Enet failed to initialise!" << "\n\n";
	}

	/* Declairing the ENet variables for creating a server, one of them being a peer
	which is ENet's way of tracking what you're connected to. */

	ENetAddress address;
	ENetHost* server;
	ENetEvent enetEvent;
	ENetPacket* dataPacket;

	address.host = ENET_HOST_ANY;
	address.port = 1234;
	server = enet_host_create(&address, 32, 2, 0, 0);

	if (server == NULL)
	{
		cout << "Server failed to initialise!" << "\n\n";
	}
	position.x = 600.0f;
	position.y = 300.0f;



	lua_State* F = luaL_newstate();
	luaL_dofile(F, "level.lua");
	luaL_openlibs(F);
	lua_pcall(F, 0, 0, 0);



	// SFML Varibales
	sf::Clock clock;
	sf::Font font;
	sf::Text text;
	sf::Text text2;

	// Box2D Variables
	float scalingFactor = 200.0f;
	float timeStep = 1.0f / 120.0f;
	int velocityIterations = 8;
	int positionIterations = 3;
	int counter = 0;
	

	// LUA Variables for RenderWindow
	LuaRef t = getGlobal(F, "window");
	LuaRef title = t["title"];
	LuaRef w = t["width"];
	LuaRef h = t["height"];
	std::string titleString = title.cast<std::string>();
	int width = w.cast<int>();
	int height = h.cast<int>();

	// SFML Window Setup
	sf::RenderWindow window(sf::VideoMode(width, height), titleString);

	// Camera Following the Player1
	sf::View view1(sf::FloatRect(200, 200, 300, 200));
	view1.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
	view1.setCenter(sf::Vector2f(view1.getSize().x / 2, view1.getSize().y / 2));
	window.setView(view1);

	// Loading the textures for the Walls, Room's Floor, Groud, and Coins
	if (!textureWall.loadFromFile("../Textures/bricksx64.png"))
	{
		return EXIT_FAILURE;
	}

	if (!textureCoin.loadFromFile("../Textures/Coin1.png"))
	{
		return EXIT_FAILURE;
	}

	if (!textureRoom.loadFromFile("../Textures/woodFloor.png"))
	{
		return EXIT_FAILURE;
	}

	if (!textureGrass.loadFromFile("../Textures/grassTexture1.png"))
	{
		return EXIT_FAILURE;
	}


	// Setting the Sound with FMOD 
	FMOD::Studio::System* system = NULL;
	FMOD::Studio::System::create(&system);

	FMOD::System* lowLevelSystem = NULL;
	system->getLowLevelSystem(&lowLevelSystem);

	lowLevelSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_STEREO, 0);
	system->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, NULL);

	FMOD::Channel* soundsChannel = NULL;

	// Setting Sound Pickup
	FMOD::Sound* pickupCoin = NULL;
	lowLevelSystem->createSound("../Sounds/Pickup_Coin8.wav", FMOD_LOOP_OFF, NULL, &pickupCoin);
	pickupCoin->setDefaults(32800 * 2, 0);


	// Setting  Background Music
	FMOD::Sound* music = NULL;
	lowLevelSystem->createSound("../Sounds/backgroundSound.ogg", FMOD_LOOP_NORMAL, NULL, &music);
	music->setDefaults(40000, 0);
	lowLevelSystem->playSound(music, NULL, false, &soundsChannel);

	// Setting Footstep Sound
	FMOD::Sound* footstep = NULL;
	lowLevelSystem->createSound("../Sounds/footstep.ogg", FMOD_LOOP_OFF, NULL, &footstep);
	footstep->setDefaults(40000, 0);

	
	// Box2D Player Object
	float playerWidth = 0.09f;
	float playerHeight = 0.16f;
	b2PolygonShape playerBox;
	playerBox.SetAsBox(playerWidth, playerHeight);

	b2BodyDef playerBodyDef; // Setting the X and Y position
	playerBodyDef.type = b2_dynamicBody; // The playerBodyDef is set to a b2_dynamicBody because in this way Box2D knows that this object moves
	playerBodyDef.position.Set(2.0f, -1.75f);

	b2Body* playerBody = world.CreateBody(&playerBodyDef); // Declaring Box Position
	playerBody->CreateFixture(&playerBox, 0.0f); // Declaring Box Width & Height

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &playerBox;
	fixtureDef.density = 0.3f;
	fixtureDef.friction = 0.3f;

	playerBody->CreateFixture(&fixtureDef);


	// Player Game Object
	class Player Character;
	Character.playerBody = playerBody;
	sf::Texture textureCharacter;
	if (!textureCharacter.loadFromFile("../Textures/spritewalk.png"))
	{
		return EXIT_FAILURE;
	}
	sf::Sprite spriteCharacter(textureCharacter);
	Character.sprite = spriteCharacter;


	// Box2D Player2 Object
	float player2Width = 0.09f;
	float player2Height = 0.16f;
	b2PolygonShape player2Box;
	player2Box.SetAsBox(player2Width, player2Height);

	b2BodyDef player2BodyDef; // Setting the X and Y position
	player2BodyDef.type = b2_dynamicBody; // The player2BodyDef is set to a b2_dynamicBody because in this way Box2D knows that this object moves
	player2BodyDef.position.Set(1.0f, -1.0f);

	b2Body* player2Body = world.CreateBody(&player2BodyDef); // Declaring Box Position
	player2Body->CreateFixture(&player2Box, 0.0f); // Declaring Box Width & Height

	b2FixtureDef fixtureDef2;
	fixtureDef2.shape = &player2Box;
	fixtureDef2.density = 0.3f;
	fixtureDef2.friction = 0.3f;

	player2Body->CreateFixture(&fixtureDef2);


	// Player2 Game Object
	class Player2 Character2;
	Character2.playerBody = player2Body;
	Character2.sprite = spriteCharacter;
	Character2.sprite.setTextureRect(sf::IntRect(0, 0, 24, 32));



	// Creating the Background
	int grassSize = 25;
	for (int x = -grassSize; x < grassSize; x++)
	{
		for (int y = -grassSize; y < grassSize; y++)
		{
			createFloorTile(x * 64, y * 64, 0);
		}
	}

	// Drawing the Map Boundary
	createMapBoundary(-grassSize, -grassSize, grassSize * 2, grassSize * 2, 50);


	// Create Custom Rooms 
	for (int i = 1; i <= 10; i++)
	{
		string temp = "room";
		temp += to_string(i);

		if (getGlobal(F, temp.c_str()))
		{
			cout << "Room Created:    " << temp.c_str() << endl;
			t = getGlobal(F, temp.c_str());
			LuaRef roomStartXPosition = t["roomStartXPosition"];
			LuaRef roomStartYPosition = t["roomStartYPosition"];
			LuaRef roomXWidth = t["roomXWidth"];
			LuaRef roomYHeight = t["roomYHeight"];
			LuaRef hasRightExit = t["hasRightExit"];
			LuaRef rightExitYPosition = t["rightExitYPosition"];
			LuaRef hasLeftExit = t["hasLeftExit"];
			LuaRef leftExitYPosition = t["leftExitYPosition"];
			LuaRef hasUpperExit = t["hasUpperExit"];
			LuaRef upperExitXPosition = t["upperExitXPosition"];
			LuaRef hasLowerExit = t["hasLowerExit"];
			LuaRef lowerExitXPosition = t["lowerExitXPosition"];
			LuaRef numberCoinsRoom = t["numberCoinsRoom"];

			createCustomRoom(roomStartXPosition.cast<int>(), roomStartYPosition.cast<int>(), roomXWidth.cast<int>(), roomYHeight.cast<int>(), hasRightExit.cast<bool>(), rightExitYPosition.cast<int>(), hasLeftExit.cast<bool>(), leftExitYPosition.cast<int>(), hasUpperExit.cast<bool>(), upperExitXPosition.cast<int>(), hasLowerExit.cast<bool>(), lowerExitXPosition.cast<int>(), numberCoinsRoom.cast<int>());
		}

	}


	// Coins Text Display
	if (!font.loadFromFile("../Fonts/font.ttf"))
	{
		return EXIT_FAILURE;
	}	
	// select the font
	text.setFont(font); // font is a sf::Font												 
	text.setCharacterSize(24); // set the character size in pixels, not points!	   
	text.setFillColor(sf::Color::Yellow); // set the color
	text.setPosition(25, 25);							  
	text.setStyle(sf::Text::Bold);// set the text style


	text2.setFont(font); // font is a sf::Font												 
	text2.setCharacterSize(24); // set the character size in pixels, not points!	   
	text2.setFillColor(sf::Color::Yellow); // set the color
	text2.setPosition(25, 50);
	text2.setStyle(sf::Text::Bold);// set the text style


	Vector2* newPosition = new Vector2;

	// Start the game loop
	while (window.isOpen())
	{
		system->update();
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Close window: exit
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
				
			// Esc button pressed: exit
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
			{
				window.close();
			}


		}


		while (enet_host_service(server, &enetEvent, 0) > 0)
		{
			switch (enetEvent.type)
			{
			case ENET_EVENT_TYPE_CONNECT:

				cout << "A client connected from address " << enetEvent.peer->address.host << ":" << enetEvent.peer->address.port << ".\n";


				enetEvent.peer->data = "This is a client";

				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				cout << "The client from address " << enetEvent.peer->address.host << ":" << enetEvent.peer->address.port << " disconnected \n";

				enetEvent.peer->data = NULL;

				break;

			case ENET_EVENT_TYPE_RECEIVE:
			
				memcpy(newPosition, enetEvent.packet->data, enetEvent.packet->dataLength);


				if (newPosition->x == 1)
				{
					Character2.sprite.setTextureRect(sf::IntRect(Character2.counterWalk * 24, 32 * 3, 24, 32));
					
				}
				else if (newPosition->x == -1)
				{
					Character2.sprite.setTextureRect(sf::IntRect(Character2.counterWalk * 24, 32 * 2, 24, 32));

				}
				if (newPosition->y == 1)
				{
					Character2.sprite.setTextureRect(sf::IntRect(Character2.counterWalk * 24, 32 * 1, 24, 32));

				}
				else if (newPosition->y == -1)
				{
					Character2.sprite.setTextureRect(sf::IntRect(Character2.counterWalk * 24, 32 * 0, 24, 32));
	
				}
				if (newPosition->x == 0 && newPosition->y == 0)
				{
					Character2.sprite.setTextureRect(sf::IntRect(0, 0, 24, 32));
				}
				else
				{
					Character2.counterWalk++;
				}
				if (Character2.counterWalk >= 7)
				{
					Character2.counterWalk = 0;
				}

				Character2.playerBody->SetLinearVelocity(b2Vec2(newPosition->x, newPosition->y));

				
				break;
			}
		}

		// Box2D Update
		world.Step(timeStep, velocityIterations, positionIterations);


		// Clear Screen
		window.clear();

		//
		text.setPosition(Character.playerImagePosition.x - window.getSize().x / 2, Character.playerImagePosition.y - window.getSize().y / 2);
		text2.setPosition(Character.playerImagePosition.x - window.getSize().x / 2, (Character.playerImagePosition.y - window.getSize().y / 2) + 25);


		// Update player
		Character.Update();
		Character2.Update();


		// 
		lowLevelSystem->update();
		sf::Time elapsedTime = clock.getElapsedTime();
		
		if (elapsedTime.asSeconds() > 0.2)
		{
			clock.restart();
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				lowLevelSystem->playSound(footstep, NULL, false, &soundsChannel);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				lowLevelSystem->playSound(footstep, NULL, false, &soundsChannel);

			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				lowLevelSystem->playSound(footstep, NULL, false, &soundsChannel);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				lowLevelSystem->playSound(footstep, NULL, false, &soundsChannel);
			}
		}
		
		// Camera Following Player
		window.setView(view1);
		view1.setCenter(Character.playerImagePosition);

		// Draw Ground
		counter = 0;
		for (iter3 = groundArray.begin(); iter3 != groundArray.end(); iter3++)
		{
			window.draw(groundArray[counter].sprite);

			counter++;
		}

		// Draw PickUps
		counter = 0;
		for (iter2 = pickUpArray.begin(); iter2 != pickUpArray.end(); iter2++)
		{
			window.draw(pickUpArray[counter].sprite);
			counter++;
		}

		// Draw Wall
		counter = 0;
		for (iter = wallArray.begin(); iter != wallArray.end(); iter++)
		{
			wallArray[counter].Update();
			window.draw(wallArray[counter].sprite);

			counter++;
		}

		// Checking for Player Picking up Coins
		counter = 0;
		for (iter2 = pickUpArray.begin(); iter2 != pickUpArray.end(); iter2++)
		{
			
			if (((Character.playerImagePosition.x + Character.spriteWidth / 2.0f) >= pickUpArray[counter].rect.getPosition().x) && ((Character.playerImagePosition.x + Character.spriteWidth / 2.0f) <= (pickUpArray[counter].rect.getPosition().x + pickUpArray[counter].rect.getSize().x)))
			{
				if (((Character.playerImagePosition.y + Character.spriteHeight / 2.0f) >= pickUpArray[counter].rect.getPosition().y) && ((Character.playerImagePosition.y + Character.spriteHeight / 2.0f) <= (pickUpArray[counter].rect.getPosition().y + pickUpArray[counter].rect.getSize().y)))
				{
					lowLevelSystem->playSound(pickupCoin, NULL, false, &soundsChannel);
					Character.coinAmount++;
					pickUpArray.erase(iter2);	
					break;
				}		
			}
			counter++;
		}


		// Checking for Player2 Picking up Coins
		counter = 0;
		for (iter2 = pickUpArray.begin(); iter2 != pickUpArray.end(); iter2++)
		{

			if (((Character2.playerImagePosition.x + Character2.spriteWidth / 2.0f) >= pickUpArray[counter].rect.getPosition().x) && ((Character2.playerImagePosition.x + Character2.spriteWidth / 2.0f) <= (pickUpArray[counter].rect.getPosition().x + pickUpArray[counter].rect.getSize().x)))
			{
				if (((Character2.playerImagePosition.y + Character2.spriteHeight / 2.0f) >= pickUpArray[counter].rect.getPosition().y) && ((Character2.playerImagePosition.y + Character2.spriteHeight / 2.0f) <= (pickUpArray[counter].rect.getPosition().y + pickUpArray[counter].rect.getSize().y)))
				{
					lowLevelSystem->playSound(pickupCoin, NULL, false, &soundsChannel);
					Character2.coinAmount++;
					pickUpArray.erase(iter2);
					break;
				}
			}
			counter++;
		}


		// Draw Players
		window.draw(Character.sprite);
		window.draw(Character2.sprite);
		
		// Draw Coin Text
		text.setString("Player1 Coins  " + std::to_string(Character.coinAmount));
		window.draw(text);
		text2.setString("Player2 Coins  " + std::to_string(Character2.coinAmount));
		window.draw(text2);


		window.display();
	}


	/* Destroy the ENet Host instance, and deinitialize the library on shutdown. */

	enet_host_destroy(server);
	atexit(enet_deinitialize);

	return 0;
}
