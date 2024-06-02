#include<iostream>
#include<vector>
#include<string>
#include<random>
#include<time.h>
#include<SFML/Graphics/Texture.hpp>
#include<SFML/Graphics/Sprite.hpp>
#include<SFML/Graphics/Texture.hpp>
#include<SFML/Graphics/Rect.hpp>
#include <SFML/Graphics.hpp>

struct position {
	int start_x;
	int start_y;
	position() { start_x = -20; start_y = -20; };
	position(int x, int y) : start_x(x), start_y(y) {};
	void addPos(position& p) {
		start_x += p.start_x;
		start_y += p.start_y;
	}
	position& operator=(const position& p) {
		start_x = p.start_x;
		start_y = p.start_y;
		return *this;
	}
	position& operator+=(const position& p) {
		start_x += p.start_x;
		start_y += p.start_y;
		return *this;
	}
	friend position operator*(position lhs, const int& rhs) 
	{

		lhs.start_x *= rhs;
		lhs.start_y *= rhs;
		return lhs;
	}
	friend position operator+(position lhs, const position& rhs)
	{
		lhs.start_x += rhs.start_x; 
		lhs.start_y += rhs.start_y;
		return lhs; 
	}
};

class Ship {
private:

	int _len = 1;
	int _len_l = 1;
	int _rotate = 0;
	int _rotate_l = 0;
	position _pos;

	sf::Sprite _sprite;

	bool isIn(position newPos) {
		if (newPos.start_x == -20 && newPos.start_y == -20) {
			newPos = _pos;

		}
		else {
			if (!(newPos.start_x < 10 && newPos.start_x >= 0 && newPos.start_y < 10 && newPos.start_y >= 0))
				return false;
		}
		newPos = shipEndPos(newPos);
		return newPos.start_x < 10 && newPos.start_x >= 0 && newPos.start_y < 10 && newPos.start_y >= 0;
	}


public:
	Ship(int len, sf::Texture& text, position p, int rotation = 0) : _len(len), _len_l(len), _rotate(rotation), _rotate_l(rotation) { _sprite.setTexture(text); _sprite.setOrigin(sf::Vector2f(25, 25)); _pos.start_x = p.start_x; _pos.start_y = p.start_y; _sprite.setRotation(90 * rotation); changePos(p); };

	void changeRotation() {
		++_rotate_l;
		_sprite.rotate(90);
	}

	int getLen() {
		return _len;
	}

	int getRotate() {
		return _rotate;
	}

	bool hit() {
		--_len;
		if (_len == 0)
			return 1;
		return 0;
	}

	position getPos() {
		return _pos;
	}

	void setPosition(position pos, bool checkOnEmptiness) {
		if (isIn(pos) && checkOnEmptiness) {
			_pos = pos;
			_rotate = _rotate_l;
		}
		else {
			_rotate_l = _rotate;
		}
		_sprite.setRotation(90 * _rotate);
		_sprite.setPosition(_pos.start_x * 50 + 50 + _sprite.getOrigin().x, _pos.start_y * 50 + 200 + _sprite.getOrigin().y);
	}

	const sf::Sprite& getSprite() {
		return _sprite;
	}

	position shipEndPos(position newPos) {
		if (_rotate_l % 4 == 0) {
			newPos.start_x += _len - 1;
		}
		else if (_rotate_l % 4 == 1) {
			newPos.start_y += _len - 1;
		}
		else if (_rotate_l % 4 == 2) {
			newPos.start_x -= _len - 1;
		}
		else {
			newPos.start_y -= _len - 1;
		}
		return newPos;
	}

	int getLenLocal() {
		return _len_l;
	}

	void changePos(position pos) {
		_sprite.setPosition(pos.start_x * 50 + 50 + _sprite.getOrigin().x, pos.start_y * 50 + 200 + _sprite.getOrigin().y);

	}
};

class Field {
protected:
	std::vector<std::vector<int>> _field = std::vector<std::vector<int>>(10, std::vector<int>(10, 100));
	std::vector<Ship> _ships;
	std::vector<sf::Texture> _texts;
	int _count_of_destroyed = 0;
public:
	Field() {
		for (int i = 1; i <= 4; ++i) {
			sf::Texture t;
			t.loadFromFile("./Sprites/ship" + std::to_string(i) + ".png");
			_texts.push_back(t);
		}
	}

	int getState(int r, int c) {
		return _field[r][c];
	}

	int setHit(int r, int c) {
		//1 - 10 - номера кораблей
	//100 - пустая клетка
	//101 - попал в пустую
	//<0 - попал в корабль
		if (_field[r][c] == 101) {
			return -1;
		}
		else if (_field[r][c] < 0) {
			return -2;
		}
		else if (_field[r][c] >= 1 && _field[r][c] < 100) {
			_field[r][c] = -_field[r][c];
			if (_ships[-_field[r][c] - 1].hit() + 1 == 2) {
				++_count_of_destroyed;
				return 2;
			}
			return 1;
		}
		else {
			_field[r][c] = 101;
			return 0;
		}
	}


};

class PlayerField : public Field {
private:
	int _unused_ships = 10;
public:
	PlayerField() {
		int last_pose = 0;
		for (int i = 1; i <= 4; ++i) {
			for (int j = 1; j <= 4 - i + 1; ++j) {
				_ships.push_back(Ship(i, _texts[i - 1], position(last_pose, -4)));
				last_pose += i;
			}
		}
	}

	bool isAllUser() {
		return _unused_ships == 0;
	}

	void deleteOldPlace(int shipNum, position newPos) {
		//?
	}
};

sf::RenderWindow window(sf::VideoMode(1250, 900), "Ship Fight");
sf::Vector2i pixelPos = sf::Mouse::getPosition(window);//забираем коорд курсора
sf::Vector2f pos = window.mapPixelToCoords(pixelPos);//переводим их в игровые (уходим от коорд окна)

bool isMove = false;
int isMoveNum = 0;
bool isPlayerStep = true;
sf::Font main_font;
sf::Texture re_text;
sf::Sprite re_sprite;

void drawFieldLines() {
	sf::Vertex line[2];
	sf::Text text("", main_font, 20);
	text.setFillColor(sf::Color::Black);
	text.setOrigin(sf::Vector2f(text.getCharacterSize() / 2, text.getCharacterSize() / 2));
	for (int i = 0; i <= 10; ++i) {
		if (i != 10) {
			//player field
			text.setString(std::to_string(i + 1));
			text.setPosition(sf::Vector2f(25, 50 * i + 200 + 25));
			window.draw(text);
			text.setString(char('A' + i));
			text.setPosition(sf::Vector2f(50 * i + 75, 200 - 25));
			window.draw(text);
			//enemy field
			text.setString(std::to_string(i + 1));
			text.setPosition(sf::Vector2f(700 - 25, 50 * i + 200 + 25));
			window.draw(text);
			text.setString(char('A' + i));
			text.setPosition(sf::Vector2f(50 * i + 700 + 25, 200 - 25));
			window.draw(text);
		}
		//player field
		line[0].position = sf::Vector2f(50, 50 * i + 200);
		line[0].color = sf::Color::Red;
		line[1].position = sf::Vector2f(550, 50 * i + 200);
		line[1].color = sf::Color::Red;
		window.draw(line, 2, sf::Lines);
		line[0].position = sf::Vector2f(50 * i + 50, 200);
		line[0].color = sf::Color::Red;
		line[1].position = sf::Vector2f(50 * i + 50, 700);
		line[1].color = sf::Color::Red;
		window.draw(line, 2, sf::Lines);
		//enemy field
		line[0].position = sf::Vector2f(700, 50 * i + 200);
		line[0].color = sf::Color::Red;
		line[1].position = sf::Vector2f(1200, 50 * i + 200);
		line[1].color = sf::Color::Red;
		window.draw(line, 2, sf::Lines);
		line[0].position = sf::Vector2f(50 * i + 700, 200);
		line[0].color = sf::Color::Red;
		line[1].position = sf::Vector2f(50 * i + 700, 700);
		line[1].color = sf::Color::Red;
		window.draw(line, 2, sf::Lines);
	}
}

position getCellMouseOn() {
	return position(int((pos.x - 50) / 50), int((pos.y - 200) / 50));
}


void displayAl() {
	window.clear(sf::Color::White);
	drawFieldLines();
	window.display();
}
int main() {
	while (window.isOpen()) {
		displayAl();
	}
	return 0;
}