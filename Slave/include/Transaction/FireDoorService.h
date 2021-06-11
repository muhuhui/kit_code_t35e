#pragma once

#include <vector>
#include "FireDoor.h"
#include <string>
#include "Driver/MySQLDriver.h"

#include<string>
using namespace std;

typedef Poco::Tuple<int, string, int, int, string> TupleFire;

class FireDoorService
{
public:
	FireDoorService();
	~FireDoorService();
	int queryAll( std::vector<FireDoor> &vFireDoor);

private:
	MySQLDriver*    m_pMySQLDriver;
};

