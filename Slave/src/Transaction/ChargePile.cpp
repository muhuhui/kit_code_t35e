#include "ChargePile.h"


ChargePile::ChargePile()
{
}


ChargePile::~ChargePile()
{
}

void ChargePile::setId(int id)
{
	this->id = id;
}

int ChargePile::getId()
{
	return id;
}


void ChargePile::setPosition(int position)
{
	this->position = position;
}

int ChargePile::getPosition()
{
	return position;
}

void ChargePile::setStatus(int     status)
{
	this->status = status;
}

int ChargePile::getStatus()
{
	return status;
}

void ChargePile::setIp(std::string ip)
{
	this->ip = ip;
}

std::string ChargePile::getIp()
{
	return ip;
}

void ChargePile::setName(std::string name)
{
	this->name = name;
}
std::string ChargePile::getName()
{
	return name;
}

void ChargePile::setInstanceName(std::string name)
{
	this->instance_name = name;
}
std::string ChargePile::getInstanceName()
{
	return instance_name;
}

void ChargePile::setMapId(int mapId)
{
    this->mapId = mapId;
}

int ChargePile::getMapId()
{
    return mapId;
}

