#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "stdafx.h"

#include "Messages.h"
#include "Sound.h"

using namespace sf;
using namespace std;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int COUNT_BLOCKS_X = 10;
const int COUNT_BLOCKS_Y = 4;
const float BALL_RADIUS = 10;
const float BALL_SPEED = 6;
const float PADDLE_WIDTH = 120;
const float PADDLE_HEIGHT = 10;
const float PADDLE_SPEED = 12;
const float BLOCK_WIDTH = 60;
const float BLOCK_HEIGHT = 20;
const float RANGE_BEETWEN_BLOCKS = 5;
const float X_COEFFICIENT = 1.75;
const float Y_COEFFICIENT = 2;
const int BUTTONS_NUMBER = 2;
const string PLAY_BUTTON = "Play";
const string ESCAPE_BUTTON = "Exit";

struct ball {
	CircleShape shape;
	Vector2f ballspeed{ -BALL_SPEED, -BALL_SPEED };

	ball(float X, float Y) {
		shape.setPosition(X, Y);
		shape.setRadius(BALL_RADIUS);
		shape.setFillColor(Color::Red);
		shape.setOrigin(BALL_RADIUS, BALL_RADIUS);
	}

	float x() {
		return shape.getPosition().x;
	}
	float y() {
		return shape.getPosition().y;
	}
	float left_side() {
		return x() - shape.getRadius();
	}
	float right_side() {
		return x() + shape.getRadius();
	};
	float top() {
		return y() - shape.getRadius();
	}
	float bottom() {
		return y() + shape.getRadius();
	}

	void update() {
		shape.move(ballspeed);
		
		if (left_side() < 0)
			ballspeed.x = BALL_SPEED;
		else if (right_side() > WINDOW_WIDTH)
			ballspeed.x = -BALL_SPEED;
		if (top() < 0)
			ballspeed.y = BALL_SPEED;
	}
};

struct paddle {
	RectangleShape shape;
	Vector2f paddlespeed;

	paddle(float X, float Y){
		shape.setPosition(X, Y);
		shape.setSize(Vector2f(PADDLE_WIDTH, PADDLE_HEIGHT));
		shape.setFillColor(Color::Blue);
		shape.setOrigin(PADDLE_WIDTH / 2, PADDLE_HEIGHT / 2);
	}

	float x() {
		return shape.getPosition().x;
	}
	float y() {
		return shape.getPosition().y;
	}
	float left_side() {
		return x() - shape.getSize().x / 2;
	}
	float right_side() {
		return x() + shape.getSize().x / 2;
	};
	float top() {
		return y() - shape.getSize().y / 2;
	}
	float bottom() {
		return y() + shape.getSize().y / 2;
	}

	void update() {
		shape.move(paddlespeed);

		if ((Keyboard::isKeyPressed(Keyboard::Key::Left) || Keyboard::isKeyPressed(Keyboard::Key::A)) && left_side() > 0)
			paddlespeed.x = -PADDLE_SPEED;
		else if ((Keyboard::isKeyPressed(Keyboard::Key::Right) || Keyboard::isKeyPressed(Keyboard::Key::D)) && right_side() < WINDOW_WIDTH)
			paddlespeed.x = PADDLE_SPEED;
		else
			paddlespeed.x = 0;
	}
};

template<class T1, class T2>  bool checkCollision(T1& Obj1, T2& Obj2) {
	return Obj2.right_side() >= Obj1.left_side() && Obj2.left_side() <= Obj1.right_side()
		&& Obj2.bottom() >= Obj1.top() && Obj2.top() <= Obj1.bottom();
}

void setCollisionPaddle(ball& cB, paddle& cP) {
	if (!checkCollision(cB, cP)) 
		return;

	cB.ballspeed.y = -BALL_SPEED;

	if (cB.x() < cP.x())
		cB.ballspeed.x = -BALL_SPEED;
	else
		cB.ballspeed.x = BALL_SPEED;
}
struct brick {
	RectangleShape shape;
	
	bool destroyed = false;

	brick(float X, float Y) {
		shape.setPosition(X, Y);
		shape.setSize(Vector2f(BLOCK_WIDTH, BLOCK_HEIGHT));
		shape.setFillColor(Color::Green);
		shape.setOrigin(BLOCK_WIDTH / 2, BLOCK_HEIGHT / 2);
	}


	float x() {
		return shape.getPosition().x;
	}
	float y() {
		return shape.getPosition().y;
	}
	float left_side() {
		return x() - shape.getSize().x / 2;
	}
	float right_side() {
		return x() + shape.getSize().x / 2;
	};
	float top() {
		return y() - shape.getSize().y / 2;
	}
	float bottom() {
		return y() + shape.getSize().y / 2;
	}
};

void emplace_coordinates(vector<Vector2f>& bricks)
{
	for (int i(0); i < COUNT_BLOCKS_X; ++i)
		for (int j(0); j < COUNT_BLOCKS_Y; ++j)
			bricks.emplace_back((i + X_COEFFICIENT) * (BLOCK_WIDTH + RANGE_BEETWEN_BLOCKS), (j + Y_COEFFICIENT) * (BLOCK_HEIGHT + RANGE_BEETWEN_BLOCKS));
}
void setCollisionBr(ball& cB, brick& cBr) {
	if (!checkCollision(cB, cBr))
		return;

	cBr.destroyed = true;

	float coord_diff_left = abs(cB.right_side() - cBr.left_side());
	float coord_diff_right = abs(cBr.right_side() - cB.left_side());
	float coord_diff_top = abs(cB.bottom() - cBr.top());
	float coord_diff_bottom = abs(cBr.bottom() - cB.top());

	bool collision_left = coord_diff_left < coord_diff_right;
	bool collision_top = coord_diff_top < coord_diff_bottom;

	float overlap_x = collision_left ? coord_diff_left : coord_diff_right;
	float overlap_y = collision_top ? coord_diff_top : coord_diff_bottom;

	if (overlap_x < overlap_y)
		cB.ballspeed.x = collision_left ? -BALL_SPEED : BALL_SPEED;
	else
		cB.ballspeed.y = collision_top ? -BALL_SPEED : BALL_SPEED;
}
void drawbricks(RenderWindow& window, ball& Ball, vector<Vector2f> & bricks, sf::Sound& sound) {
	for (int k(0); k < bricks.size(); ++k) {
		brick Brick(bricks[k].x, bricks[k].y);
		setCollisionBr(Ball, Brick);
		if (Brick.destroyed) {
			sound.play();
			bricks.erase(bricks.begin() + k);
		}
		else
			window.draw(Brick.shape);
	}
}

struct menu {
	int selected_item;
	Font font;
	Text text[BUTTONS_NUMBER];
	
	menu() {
		font.loadFromFile("arialn.ttf");

		text[0].setFont(font);
		text[0].setColor(Color::Red);
		text[0].setString(PLAY_BUTTON);
		text[0].setPosition(Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 3));

		text[1].setFont(font);
		text[1].setColor(Color::White);
		text[1].setString(ESCAPE_BUTTON);
		text[1].setPosition(Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));

		selected_item = 0;
	}

	void MoveUp() {
		if (selected_item - 1 >= 0) {
			text[selected_item].setColor(Color::White);
			selected_item--;
			text[selected_item].setColor(Color::Red);
		}
	}
	void MoveDown() {
		if (selected_item + 1 < BUTTONS_NUMBER) {
			text[selected_item].setColor(Color::White);
			selected_item++;
			text[selected_item].setColor(Color::Red);
		}
	}
};

int main() {
	Music music;
	music.openFromFile("13401934206065.wav");
	music.play();

	ball Ball(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);
	paddle Paddle(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 10);
	sound Sound;
	menu Menu;

	vector<Vector2f> bricks;
	emplace_coordinates(bricks);

	bool pause = false;
	bool choose = true;

	RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Arcanoid", sf::Style::Close);
	Messages messages(window);
	window.setFramerateLimit(50);
	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			switch (choose) {
			case true:
				if (event.type == Event::KeyReleased) {
					switch (event.key.code) {
					case Keyboard::Up:
						Menu.MoveUp();
						break;

					case Keyboard::Down:
						Menu.MoveDown();
						break;

					case Keyboard::Return:
						choose = false;
						break;
					}
				}
				break;

			case false:
				if (event.type == Event::KeyReleased) {
					switch (event.key.code) {

					case Keyboard::Space:
						pause = !pause;
						break;

					case Keyboard::Escape:
						window.close();
						music.pause();
						main();
						break;
					}
				}
				break;
			}
			if (event.type == Event::Closed) window.close();
		}
		window.clear(Color::Black);
		if (choose == true) {
			for (int i(0); i < BUTTONS_NUMBER; ++i)
				window.draw(Menu.text[i]);
		}
		else {
			switch (Menu.selected_item) {

			case 0:
				if (!pause) {
					Ball.update();
					Paddle.update();
				}
				setCollisionPaddle(Ball, Paddle);
				if ((Ball.y() < WINDOW_HEIGHT) && (bricks.size() != 0)) {
					window.draw(Paddle.shape);
					window.draw(Ball.shape);
					drawbricks(window, Ball, bricks, Sound.sound_effect);
				}
				else if (bricks.size() == 0)
					window.draw(messages.message[0]);
				else if (Ball.y() > WINDOW_HEIGHT)
					window.draw(messages.message[1]);
				break;

			case 1:
				window.close();
				break;
			}
		}
		window.display();
	}
	return 0;
}

