#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "stdafx.h"

using namespace sf;
using namespace std;

const int window_width = 800, window_heigth = 600;
const int count_blocks_x = 10, count_blocks_y = 4;
const float ball_radius = 10, ball_speed = 6;
const float paddle_width = 120, paddle_height = 10, paddle_speed = 12;
const float block_width = 60, block_height = 20, range_beetwen_blocks = 5;
const float x_coefficient = 1.75, y_coefficient = 2;
const int button_number = 2, messages_number = 2;
const string play = "Play", win = "You win!", lost = "You Lost!", escape = "Exit";

struct ball {
	CircleShape shape;
	Vector2f ballspeed{ -ball_speed, -ball_speed };

	ball(float X, float Y) {
		shape.setPosition(X, Y);
		shape.setRadius(ball_radius);
		shape.setFillColor(Color::Red);
		shape.setOrigin(ball_radius, ball_radius);
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
			ballspeed.x = ball_speed;
		else if (right_side() > window_width)
			ballspeed.x = -ball_speed;
		if (top() < 0)
			ballspeed.y = ball_speed;
	}
};
struct paddle {
	RectangleShape shape;
	Vector2f paddlespeed;

	paddle(float X, float Y){
		shape.setPosition(X, Y);
		shape.setSize(Vector2f(paddle_width, paddle_height));
		shape.setFillColor(Color::Blue);
		shape.setOrigin(paddle_width / 2, paddle_height / 2);
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
			paddlespeed.x = -paddle_speed;
		else if ((Keyboard::isKeyPressed(Keyboard::Key::Right) || Keyboard::isKeyPressed(Keyboard::Key::D)) && right_side() < window_width)
			paddlespeed.x = paddle_speed;
		else
			paddlespeed.x = 0;
	}
};

template<class T1, class T2> bool checkCollision(T1& Obj1, T2& Obj2) {
	return Obj2.right_side() >= Obj1.left_side() && Obj2.left_side() <= Obj1.right_side()
		&& Obj2.bottom() >= Obj1.top() && Obj2.top() <= Obj1.bottom();
}
void setCollisionPaddle(ball& cB, paddle& cP) {
	if (!checkCollision(cB, cP)) 
		return;

	cB.ballspeed.y = -ball_speed;

	if (cB.x() < cP.x())
		cB.ballspeed.x = -ball_speed;
	else
		cB.ballspeed.x = ball_speed;
}

struct brick {
	RectangleShape shape;
	
	bool destroyed = false;

	brick(float X, float Y) {
		shape.setPosition(X, Y);
		shape.setSize(Vector2f(block_width, block_height));
		shape.setFillColor(Color::Green);
		shape.setOrigin(block_width / 2, block_height / 2);
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

vector<Vector2f> emplace_bricks(vector<Vector2f> bricks) {
	for (int i(0); i < count_blocks_x; ++i)
		for (int j(0); j < count_blocks_y; ++j)
			bricks.emplace_back((i + x_coefficient) * (block_width + range_beetwen_blocks), (j + y_coefficient) * (block_height + range_beetwen_blocks));
	return bricks;
} //функция не работает в релизной версии
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
		cB.ballspeed.x = collision_left ? -ball_speed : ball_speed;
	else
		cB.ballspeed.y = collision_top ? -ball_speed : ball_speed;
}

struct sound {
	SoundBuffer buffer;
	Sound sound1;

	sound() {
		buffer.loadFromFile("LASRHVY2.wav");
		sound1.setBuffer(buffer);
		sound1.setVolume(100);
	}
	void drawbricks(RenderWindow& window, ball& Ball, vector<Vector2f> & bricks) {

		for (int k(0); k < bricks.size(); ++k) {
			brick Brick(bricks[k].x, bricks[k].y);
			setCollisionBr(Ball, Brick);
			if (Brick.destroyed) {
				sound1.play();
				bricks.erase(bricks.begin() + k);
			} else
				window.draw(Brick.shape);
		}
	}
};


struct menu {
	int selected_item;
	Font font;
	Text text[button_number];
	
	menu() {
		font.loadFromFile("arialn.ttf");

		text[0].setFont(font);
		text[0].setColor(Color::Red);
		text[0].setString(play);
		text[0].setPosition(Vector2f(window_width / 2, window_heigth / 3));

		text[1].setFont(font);
		text[1].setColor(Color::White);
		text[1].setString(escape);
		text[1].setPosition(Vector2f(window_width / 2, window_heigth / 2));

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
		if (selected_item + 1 < button_number) {
			text[selected_item].setColor(Color::White);
			selected_item++;
			text[selected_item].setColor(Color::Red);
		}
	}
};

struct message {
	Font font;
	Text messages[messages_number];

	message() {
		font.loadFromFile("arialn.ttf");

		messages[0].setFont(font);
		messages[0].setColor(Color::White);
		messages[0].setString(win);
		messages[0].setCharacterSize(50);
		messages[0].setPosition(Vector2f(window_width / 2, window_heigth / 2));

		messages[1].setFont(font);
		messages[1].setColor(Color::Red);
		messages[1].setString(lost);
		messages[1].setCharacterSize(50);
		messages[1].setPosition(Vector2f(window_width / 2, window_heigth / 2));

	}
};

int main() {
	Music music;
	music.openFromFile("13401934206065.wav");
	music.play();

	ball Ball(window_width / 2.f, window_heigth / 2.f);
	paddle Paddle(window_width / 2.f, window_heigth - 10);
	sound Sound;
	menu Menu;
	message Message;
	//vector<Vector2f> bricks = emplace_bricks(bricks); для версии debug
	vector<Vector2f> bricks; 
		for (int i(0); i < count_blocks_x; ++i)
			for (int j(0); j < count_blocks_y; ++j)
				bricks.emplace_back((i + x_coefficient) * (block_width + range_beetwen_blocks), (j + y_coefficient) * (block_height + range_beetwen_blocks));
	//^для release
	bool pause = false;
	bool choose = true;

	RenderWindow window(VideoMode(window_width, window_heigth), "Arcanoid");
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
			for (int i(0); i < button_number; ++i)
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
				if ((Ball.y() < window_heigth) && (bricks.size() != 0)) {
					window.draw(Paddle.shape);
					window.draw(Ball.shape);
					Sound.drawbricks(window, Ball, bricks);
				}
				else if (bricks.size() == 0) window.draw(Message.messages[0]);
				else if (Ball.y() > window_heigth) window.draw(Message.messages[1]);
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

