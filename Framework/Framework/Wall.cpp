#include "Wall.h"



Wall::Wall()
{

}



void Wall::Update()
{
	// Getting the graphical postion for the sprite to be drawn in the future
	wallPosition = wallBody->GetPosition();
	wallImagePosition.x = (wallPosition.x - 0.2) * 200;
	wallImagePosition.y = (wallPosition.y + 0.2) * 200 * -1.0f;

	sprite.setPosition(wallImagePosition);



}
