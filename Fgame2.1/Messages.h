#pragma once
#include <SFML/Graphics.hpp>

const std::string WIN_BUTTON = "You WIN!";
const std::string LOST_BUTTON = "You Lost!";
const int MESSAGES_NUMBER = 2;

struct Messages
{
	Messages(sf::RenderWindow& window);

	sf::Font font;
	sf::Text message[MESSAGES_NUMBER];
};
