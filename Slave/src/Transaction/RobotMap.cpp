#include "RobotMap.h"
RobotMap::RobotMap(/* args */)
{
}
// RobotMap::RobotMap(RobotMap::Header _header)
//     : m_MapHeader{_header}
// {
// }
RobotMap::~RobotMap()
{
}

int 
RobotMap::setMapHeader(RobotMap::Header _header)
{
    std::unique_lock<std::mutex> lck(m_DataMutex);
    m_MapHeader = _header;
    return 0;
}
RobotMap::Header& 
RobotMap::mapHeader(void)
{
    std::unique_lock<std::mutex> lck(m_DataMutex);
    return m_MapHeader;
}
RobotMap::Header 
RobotMap::mapHeader(void) const
{
    return m_MapHeader;
}


RobotMap::Ptr 
RobotMap::createNewMap(void)
{
    RobotMap::Ptr new_map(new RobotMap);
    return new_map;
}

RobotMap::Ptr 
RobotMap::createNewMapbyName(std::string mapName)
{
    auto new_map = createNewMap();
    (new_map->mapHeader()).name = mapName;
    return new_map;
}
