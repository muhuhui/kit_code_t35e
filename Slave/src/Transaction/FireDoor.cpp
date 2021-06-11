#include "FireDoor.h"


FireDoor::FireDoor()
{
}


FireDoor::~FireDoor()
{
}

void FireDoor::setId(int id)
{
	this->id = id;
}

int FireDoor::getId()
{
	return id;
}

void FireDoor::setPosition(int position)
{
	this->position = position;
}

int FireDoor::getPosition()
{
	return position;
}

void FireDoor::setStatus(int     status)
{
	this->status = status;
}

int FireDoor::getStatus()
{
	return status;
}

void FireDoor::setIp(std::string ip)
{
	this->ip = ip;
}

std::string FireDoor::getIp()
{
	return ip;
}

void FireDoor::setName(std::string name)
{
	this->name = name;
}
std::string FireDoor::getName()
{
	return name;
}

