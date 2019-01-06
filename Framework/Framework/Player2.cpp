#include "Player2.h"

Player2::Player2()
{
	
}



void Player2::Update()
{
	// Getting the graphical postion for the sprite to be drawn in the future
	playerPosition = playerBody->GetPosition();
	playerImagePosition.x = (playerPosition.x - 0.1) * 200;
	playerImagePosition.y = (playerPosition.y + 0.16) * 200 * -1.0f;

	sprite.setPosition(playerImagePosition);


}
