#pragma once
#include "Entity.h"


class Player: public Entity
{
public:
	float spriteWidth = 24.0f;
	float spriteHeight = 36.0f;
	float characterMovementSpeed = 1.0f;
	int counterWalk = 0;
	float size = 1.0f;
	int coinAmount = 0;

	b2Body* playerBody;
	b2Vec2 playerPosition;

	sf::Vector2f playerImagePosition;

	Player();
	void Update();
	void UpdateMove();
};

