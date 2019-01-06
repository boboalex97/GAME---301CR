#include "Player.h"

Player::Player()
{

}



void Player::Update()
{
	// Getting the graphical postion for the sprite to be drawn in the future
	playerPosition = playerBody->GetPosition();
	playerImagePosition.x = (playerPosition.x - 0.1) * 200;
	playerImagePosition.y = (playerPosition.y + 0.16) * 200 * -1.0f;

	sprite.setPosition(playerImagePosition);
	// Setting up the player with Box2D
	playerBody->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
	
	sprite.setTextureRect(sf::IntRect(0, 0, 24, 32));

	// If statements for the movement of the Player1
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		playerBody->SetLinearVelocity(b2Vec2(-characterMovementSpeed, 0.0f));
		sprite.setTextureRect(sf::IntRect(counterWalk * 24, 32 * 2, 24, 32));
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		playerBody->SetLinearVelocity(b2Vec2(characterMovementSpeed, 0.0f));
		sprite.setTextureRect(sf::IntRect(counterWalk * 24, 32 * 3, 24, 32));

	}
		 
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		playerBody->SetLinearVelocity(b2Vec2(0.0f, characterMovementSpeed));
		sprite.setTextureRect(sf::IntRect(counterWalk * 24, 32, 24, 32));
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		playerBody->SetLinearVelocity(b2Vec2(0.0f, -characterMovementSpeed));
		sprite.setTextureRect(sf::IntRect(counterWalk * 24, 0, 24, 32));

	}
	
	counterWalk++;
	// counterWalk is used for the animation, in this way the if statement will go through the whole row of the spritesheet so the animation will of the walking will be complete
	if (counterWalk == 7)
	{
		counterWalk = 0;
	}
		
	

}







