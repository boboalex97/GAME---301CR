#pragma once
#include <SFML\Graphics.hpp>
#include <Box2D/Box2D.h>
#include <iostream>
#include <stdlib.h>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>

using namespace std;




class Entity
{
public:
	
	float x = 0;
	float y = 0;
	sf::RectangleShape rect;
	sf::Sprite sprite;
	sf::Text text;


};

