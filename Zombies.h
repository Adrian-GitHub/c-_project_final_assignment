#pragma once

struct Position
{
	int x;
	int y;
} typedef Position;

class Zombie
{
public:
	Zombie(int, int);
	~Zombie();
	void setX(int);
	void setY(int);
	int getY();
	int getX();
	int getPos();
	void ResetPos();
private:
	Position _originalPos;
	Position _currentPos;
};

Zombie::Zombie(int x, int y)
{
	_originalPos = { x, y };
	_currentPos = _originalPos;
}

Zombie::~Zombie()
{

}
int Zombie::getY()
{
	return _currentPos.y;
}
int Zombie::getX()
{
	return _currentPos.x;
}
int Zombie::getPos()
{
	return _currentPos.y + _currentPos.x;
}
void Zombie::ResetPos()
{
	_currentPos = _originalPos;
}

void Zombie::setX(int x)
{
	_currentPos.x = x;
}

void Zombie::setY(int y)
{
	_currentPos.y = y;
}