#pragma once
#include "Entity.h"


class Wall : public Entity
{
public:
	float size = 1.0f;

	b2Body* wallBody;
	b2Vec2 wallPosition;

	sf::Vector2f wallImagePosition;

	Wall();
	void Update();
	
};


