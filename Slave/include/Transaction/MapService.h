#ifndef MAPSERVICE_H
#define MAPSERVICE_H

#include "RobotMap.h"

class MapService
{
public:
    // SQL operations:
    static uint32_t insertNewMapHeader(RobotMap::Header _header);
    static uint32_t queryMapIdbyRobotId(int robotId);
    static RobotMap::Ptr queryMapbyId(uint32_t mapId);
    static bool updateRobotbyMapId(int robotId, uint32_t mapId);

};

#endif