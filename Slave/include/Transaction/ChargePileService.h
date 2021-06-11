#pragma once

#include <vector>
#include "ChargePile.h"
#include <string>
#include "Driver/MySQLDriver.h"
#include "base/SingleTon.h"
#include<string>
using namespace std;

typedef Poco::Tuple<int, string, int, int, string, string, int> TupleChargePile;

class ChargePileService
{
public:
	ChargePileService();
	~ChargePileService();
	static ChargePileService *getInstance();
	void init();
	int queryAll(std::vector<ChargePile> &vChargePile);
	int queryByMapId(int mapId, std::vector<ChargePile> &vChargePile);

private:
	MySQLDriver*    m_pMySQLDriver;
};

