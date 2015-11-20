#include "Messages.h"

Messages::Messages(sf::RenderWindow& window)
{
	font.loadFromFile("arialn.ttf");

	message[0].setFont(font);
	message[0].setColor(sf::Color::White);
	message[0].setString(WIN_BUTTON);
	message[0].setCharacterSize(50);
	message[0].setPosition(sf::Vector2f(window.getSize().x / 2, window.getSize().y / 2));

	message[1].setFont(font);
	message[1].setColor(sf::Color::Red);
	message[1].setString(LOST_BUTTON);
	message[1].setCharacterSize(50);
	message[1].setPosition(sf::Vector2f(window.getSize().x / 2, window.getSize().y / 2));
}

