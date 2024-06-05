#include<iostream>
#include<vector>
#include<string>
#include<random>
#include<time.h>
#include<SFML/Graphics/Texture.hpp>
#include<SFML/Graphics/Sprite.hpp>
#include<SFML/Graphics/Rect.hpp>
#include <SFML/Graphics.hpp>

using std::vector;
using std::string;

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
	Ship(int len, sf::Texture& text, position p, int rotation = 0) : _len(len), _len_l(len), _rotate(rotation), _rotate_l(rotation) { _sprite.setTexture(text); _sprite.setOrigin(sf::Vector2f(25, 25)); _pos.start_x = p.start_x; _pos.start_y = p.start_y; _sprite.setRotation(90 * rotation); changePos(p); }

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
	vector<vector<int>> _field = vector<vector<int>>(10, vector<int>(10, 100));
	vector<Ship> _ships;
	vector<sf::Texture> _texts;
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

	int getShip(position p) {
		for (int i = 0; i < 10; ++i) {
			return 0;
		}
	}

	Ship& getShip(int i) {
		return _ships[i];
	}

	bool isColide(int shipNum, position newPos, bool point_out = false)
	{
		int rotation_local = int(_ships[shipNum].getSprite().getRotation() / 90);
		position vector_ship(0, 0);
		position vector_border_ship(0, 0);
		if (rotation_local % 4 == 0) {
			vector_ship.start_x = 1;
			vector_border_ship.start_y = 1;
		}
		else if (rotation_local % 4 == 1)
		{
			vector_ship.start_y = 1;
			vector_border_ship.start_x = 1;

		}
		else if (rotation_local % 4 == 2)
		{
			vector_ship.start_x = -1;
			vector_border_ship.start_y = 1;
		}
		else {
			vector_ship.start_y = -1;
			vector_border_ship.start_x = 1;

		}

		position nextCell = newPos;
		position threeCell;
		nextCell += vector_ship * (-1);
		for (int i = 0; i <= _ships[shipNum].getLenLocal() + 1; ++i) {

			threeCell = nextCell + (vector_border_ship * (-1));
			for (int j = 0; j < 3; ++j) {
				if (threeCell.start_x >= 0 && threeCell.start_x < 10 && \
					threeCell.start_y >= 0 && threeCell.start_y < 10) {
					if (!(_field[threeCell.start_y][threeCell.start_x] == shipNum + 1 || \
						_field[threeCell.start_y][threeCell.start_x] == 100) && !point_out)
					{
						return false;
					}
					else if (point_out && _field[threeCell.start_y][threeCell.start_x] == 100) {
						_field[threeCell.start_y][threeCell.start_x] = 101;
					}
				}
				else if (!point_out && j == 1 && i != 0 && i != _ships[shipNum].getLen() + 1) {
					return false;

				}
				threeCell += vector_border_ship;

			}
			nextCell += vector_ship;
		}
		return true;
	}
	void setNew(int shipNum, position newPos) {
		for (int i = 0; i < _ships[shipNum].getLen(); ++i) {
			if ((int(_ships[shipNum].getSprite().getRotation() / 90) % 2) == 0) {
				_field[newPos.start_y][newPos.start_x + i * (1 - 2 * (int(_ships[shipNum].getSprite().getRotation() / 90) % 4 == 2))] = shipNum + 1;

			}
			else {
				_field[newPos.start_y + i * (1 - 2 * (int(_ships[shipNum].getSprite().getRotation() / 90) % 4 == 3))][newPos.start_x] = shipNum + 1;
			}
		}
	}
	bool isAllDestroyed()
	{
		return _count_of_destroyed == 10;
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
		if (_ships[shipNum].getPos().start_y != -4) {
			for (int i = 0; i < _ships[shipNum].getLen(); ++i) {
				if (_ships[shipNum].getRotate() % 2 == 0) {
					_field[_ships[shipNum].getPos().start_y][_ships[shipNum].getPos().start_x + i * (1 - 2 * (_ships[shipNum].getRotate() % 4 == 2))] = 100;
				}
				else {
					_field[_ships[shipNum].getPos().start_y + i * (1 - 2 * (_ships[shipNum].getRotate() % 4 == 3))][_ships[shipNum].getPos().start_x] = 100;
				}
			}
		}
		else {
			--_unused_ships;
		}
	}
};

class EnemyField : public Field {
private:
	vector<int> _usedShipsNums;
	int _rot = 0;
	bool _hited_search = false;
	position _heated_pos;
	bool _vec_founded = false;
	int _len_counter = 0;
public:
	EnemyField()
	{
		srand((unsigned)time(NULL));

		for (int i = 1; i <= 4; ++i) {
			for (int j = 1; j <= 4 - i + 1; ++j) {
				_ships.push_back(Ship(i, _texts[i - 1], position(0, 0)));
			}
		}

		for (int i = 0; i < 10; ++i) {
			bool c = false;
			while (!c) {
				int y = rand() % 10;
				int x = rand() % 10;
				int r = rand() % 4;
				for (int j = 0; j <= r; ++j) {
					_ships[i].changeRotation();
				}
				c = isColide(i, position(x, y));
				if (c) {
					setNew(i, position(x, y));
					_ships[i].setPosition(position(x, y), true);
				}
			}
		}
	}

	const vector<int>& getUsedShips() {
		return _usedShipsNums;
	}

	void addDefeatedShip(int i){
		_usedShipsNums.push_back(i);
	}

	void addRot(){
		++_rot;
		_len_counter = 1;
	}

	void clearRot(){
		_rot = 0;
		_len_counter = 0;
	}

	int EnemyFieldgetRot(){
		return _rot;
	}

	bool isSearch(){
		return _hited_search;
	}

	void swapSearch(bool h){
		_len_counter = 0;
		_hited_search = h;
	}

	const position getVecPos()
	{
		if (_rot % 4 == 0) {
			return position(_len_counter, 0);
		}
		else if (_rot % 4 == 1)
		{
			return position(0, _len_counter);
		}
		else if (_rot % 4 == 2)
		{
			return position(-_len_counter, 0);
		}
		else {
			return position(0, -_len_counter);
		}
	}

	bool isVecFounded(){
		return _vec_founded;
	}

	void add_len(){
		++_len_counter;
	}

	const position& getHitPos(){
		return _heated_pos;
	}

	void setHitPos(const position& p){
		_heated_pos.start_x = p.start_x;
		_heated_pos.start_y = p.start_y;
	}
};



sf::RenderWindow window(sf::VideoMode(1250, 900), "Battleship");
sf::Vector2i pixelPos = sf::Mouse::getPosition(window);//забираем коорд курсора
sf::Vector2f pos = window.mapPixelToCoords(pixelPos);//переводим их в игровые (уходим от коорд окна)

bool isMove = false;
int isMoveNum = 0;
bool isPlayerStep = true;
sf::Font main_font;
PlayerField my;
EnemyField enemy;
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

position getCellMouseOnEnemy() {
	return position(int((pos.x - 700) / 50), int((pos.y - 200) / 50));
}

void shipSettelmentMouseDown(const sf::Event& e, PlayerField& my) {

	if (e.key.code == sf::Mouse::Left) {
		position cur_ship = getCellMouseOn();
		if (cur_ship.start_y == -3) {
			if (cur_ship.start_x >= 0 && cur_ship.start_x <= 3) {
				isMoveNum = cur_ship.start_x;
				if (my.getShip(isMoveNum).getSprite().getGlobalBounds().contains(pos.x, pos.y)) {
					isMove = true;
				}
			}
			else if (cur_ship.start_x >= 4 && cur_ship.start_x <= 10) {
				isMoveNum = (cur_ship.start_x - 4) / 2 + 4;
				if (my.getShip(isMoveNum).getSprite().getGlobalBounds().contains(pos.x, pos.y)) {
					isMove = true;
				}
			}
			else if (cur_ship.start_x >= 10 && cur_ship.start_x <= 15) {
				isMoveNum = (cur_ship.start_x - 10) / 3 + 7;
				if (my.getShip(isMoveNum).getSprite().getGlobalBounds().contains(pos.x, pos.y)) {
					isMove = true;
				}
			}
			else if (cur_ship.start_x >= 16 && cur_ship.start_x <= 19) {
				isMoveNum = 9;
				if (my.getShip(isMoveNum).getSprite().getGlobalBounds().contains(pos.x, pos.y)) {
					isMove = true;
				}
			}
		}
		else if (cur_ship.start_x >= 0 && cur_ship.start_x < 10 && \
			cur_ship.start_y >= 0 && cur_ship.start_y < 10)
		{
			if (my.getState(cur_ship.start_y, cur_ship.start_x) >= 1 && my.getState(cur_ship.start_y, cur_ship.start_x) <= 10) {
				isMoveNum = my.getState(cur_ship.start_y, cur_ship.start_x) - 1;
				if (my.getShip(isMoveNum).getSprite().getGlobalBounds().contains(pos.x, pos.y)) {
					isMove = true;
				}
			}
		}
	}

	if (e.key.code == sf::Mouse::Right && isMove) {
		getCellMouseOn();
		my.getShip(isMoveNum).changeRotation();
	}
}

void drawShips(PlayerField& field) {
	for (int i = 0; i < 10; ++i) {
		window.draw(field.getShip(i).getSprite());
	}
}

void shipSettelmentMouseUp(const sf::Event& e, PlayerField& my) {
	if (e.key.code == sf::Mouse::Left) {
		if (isMove) {
			isMove = false;
			if (my.isColide(isMoveNum, getCellMouseOn())) {
				my.deleteOldPlace(isMoveNum, getCellMouseOn());
				my.setNew(isMoveNum, getCellMouseOn());
				my.getShip(isMoveNum).setPosition(getCellMouseOn(), true);
			}
			else {
				my.getShip(isMoveNum).setPosition(getCellMouseOn(), false);
			}

			std::cout << 1;
		}
	}
}

void drawFieldEnemy(EnemyField& field) {
	//position p = getCellMouseOnEnemy();
	sf::RectangleShape line(sf::Vector2f(64, 2.6f));
	line.setOrigin(sf::Vector2f(32, 0));
	line.setFillColor(sf::Color::Black);
	for (int r = 0; r < 10; ++r) {
		for (int c = 0; c < 10; ++c) {
			if (field.getState(r, c) < 0) {

				line.setRotation(45);

				line.setPosition(50 * c + 725, 50 * r + 225);

				window.draw(line);

				line.setRotation(-45);

				window.draw(line);

			}
			else if (field.getState(r, c) == 101) {
				sf::CircleShape cs(3);
				cs.setPosition(sf::Vector2f(50 * c + 700 + 22, 50 * r + 200 + 22));
				cs.setFillColor(sf::Color::Black);
				window.draw(cs);
			}
		}
	}
}

void drawFieldPlayer(PlayerField& field) {
	//position p = getCellMouseOnEnemy();
	sf::RectangleShape line(sf::Vector2f(64, 2.6f));
	line.setOrigin(sf::Vector2f(32, 0));
	line.setFillColor(sf::Color::Black);

	for (int r = 0; r < 10; ++r) {
		for (int c = 0; c < 10; ++c) {
			if (field.getState(r, c) < 0) {
				line.setRotation(45);

				line.setPosition(50 * c + 75, 50 * r + 225);

				window.draw(line);

				line.setRotation(-45);

				window.draw(line);
			}
			else if (field.getState(r, c) == 101) {
				sf::CircleShape cs(3);
				cs.setPosition(sf::Vector2f(50 * c + 50 + 22, 50 * r + 200 + 22));
				cs.setFillColor(sf::Color::Black);
				window.draw(cs);
			}
		}
	}
}

void displayAl() {
	window.clear(sf::Color::White);

	drawShips(my);
	drawFieldEnemy(enemy);
	drawFieldPlayer(my);
	drawFieldLines();
	if (my.isAllDestroyed() || enemy.isAllDestroyed()) {
		window.draw(re_sprite);

	}
	window.display();
}

void startGame() {
	my = PlayerField();
	enemy = EnemyField();
	isMove = false;
	isMoveNum = 0;
	isPlayerStep = true;
}

int main() {

	main_font.loadFromFile("./Fonts/PionerSans-Bold.ttf");
	while (window.isOpen())
	{

		pixelPos = sf::Mouse::getPosition(window);//забираем коорд курсора
		pos = window.mapPixelToCoords(pixelPos);//переводим их в игровые (уходим от коорд окна)
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (isPlayerStep) {
				if (event.type == sf::Event::MouseButtonPressed)
				{
					if (!my.isAllUser()) {
						shipSettelmentMouseDown(event, my);
					}
					else {
					}
				}
				if (event.type == sf::Event::MouseButtonReleased)
				{
					if (!my.isAllUser()) {
						shipSettelmentMouseUp(event, my);
					}
				}
			}

		}



		if (isMove) {
			my.getShip(isMoveNum).changePos(getCellMouseOn());
		}

		displayAl();
	}
	return 0;
}