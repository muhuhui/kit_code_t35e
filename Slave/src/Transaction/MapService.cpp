#include "MapService.h"
#include "Driver/MySQLDriver.h"

//get mapId from database;
//mapId>=1 if success while mapId==0 means failure.
uint32_t
MapService::insertNewMapHeader(const RobotMap::Header _header)
{
    try
    {
        MySQLDriver *mySQLDriver = MySQLDriver::getInstance();
        std::string sqlStr_insert = Poco::format("INSERT INTO map (name, map_start, map_end) VALUES (\'%s\', %d, %d);",
                                                 _header.name, _header.minPosition, _header.maxPosition);
        std::string sqlStr_fetchId = Poco::format("SELECT max(id) FROM map WHERE name=\'%s\';",
                                                  _header.name);
        printf("sqlStr_insert: %s\n", sqlStr_insert.c_str());
        printf("sqlStr_fetchId: %s\n", sqlStr_fetchId.c_str());
        uint32_t tmpMapId;
        Session ses = mySQLDriver->getSession();
        if (ses.isConnected())
        {
            Statement statement(ses);
            statement << sqlStr_insert, now;
        }

        if (ses.isConnected())
        {
            try {
                Statement statement(ses);
                statement << sqlStr_fetchId, into(tmpMapId), now;
            } catch(Poco::Exception& exc) {
              std::cerr << "MapService::insertNewMapHeader " << exc.displayText() << std::endl;
            }
        }
        ses.close();
        return tmpMapId;
    }
    catch (Poco::Exception &exc)
    {
        std::cerr << "MapService::insertNewMapHeader " << exc.displayText() << std::endl;
        return 0;
    }
}

//Query the Map Id which is associated with the robot.
uint32_t
MapService::queryMapIdbyRobotId(int robotId)
{
    try
    {
        MySQLDriver *mySQLDriver = MySQLDriver::getInstance();
        std::string sqlStr = Poco::format("SELECT map_id FROM robot WHERE robot_id= %d;", robotId);
        uint32_t tmpMapId;
        Session ses = mySQLDriver->getSession();
        if (ses.isConnected())
        {
            try {
                Statement statement(ses);
                statement << sqlStr, into(tmpMapId), now;
            } catch(Poco::Exception& exc) {
              std::cerr << "MapService::queryMapIdbyRobotId " << exc.displayText() << std::endl;
            }
        }
        ses.close();
        return tmpMapId;
    }
    catch (Poco::Exception &exc)
    {
        std::cerr << "MapService::queryMapIdbyRobotId " << exc.displayText() << std::endl;
        return 0;
    }
}

bool 
MapService::updateRobotbyMapId(int robotId, uint32_t mapId)
{
    try
    {
        MySQLDriver *mySQLDriver = MySQLDriver::getInstance();
        std::string sqlStr = Poco::format("UPDATE robot SET map_id= %u WHERE robot_id= %d;", mapId, robotId);
        Session ses = mySQLDriver->getSession();
        if (ses.isConnected())
        {
            try {
                Statement statement(ses);
                statement << sqlStr, now;
            } catch(Poco::Exception& exc) {
              std::cerr << "MapService::updateRobotbyMapId " << exc.displayText() << std::endl;
            }
        }
        ses.close();
        return true;
    }
    catch (Poco::Exception &exc)
    {
        std::cerr << "MapService::updateRobotbyMapId " << exc.displayText() << std::endl;
        return false;
    }
}

RobotMap::Ptr
MapService::queryMapbyId(uint32_t mapId)
{
    try
    {
        MySQLDriver *mySQLDriver = MySQLDriver::getInstance();
        std::string sqlStr = Poco::format("SELECT name, map_start, map_end FROM map WHERE id= %u;", mapId);
        RobotMap::Header tmpHeader;
        Session ses = mySQLDriver->getSession();
        if (ses.isConnected())
        {
            try {
                Statement statement(ses);
                statement << sqlStr, into(tmpHeader.name), into(tmpHeader.minPosition), into(tmpHeader.maxPosition), now;
            } catch(Poco::Exception& exc) {
              std::cerr << "MapService::queryMapbyId " << exc.displayText() << std::endl;
            }
        }
        ses.close();
        tmpHeader.id = mapId;
        auto new_map = RobotMap::createNewMap();
        new_map->setMapHeader(tmpHeader);
        return new_map;
    }
    catch (Poco::Exception &exc)
    {
        std::cerr << "MapService::queryMapbyId " << exc.displayText() << std::endl;
        return nullptr;
    }
}

