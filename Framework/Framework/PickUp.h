#pragma once
#include "Entity.h"
class PickUp: public Entity
{
public:
	int rectWidth = 20;
	int rectHeight = 20;
	PickUp();
	~PickUp();
};

