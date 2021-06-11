#ifndef ROBOTMAP_H
#define ROBOTMAP_H

#include <memory>
#include <mutex>
#include <string>

class RobotMap
{
public:
    typedef std::shared_ptr<RobotMap> Ptr;
    struct Header
    {
        /* data */
        std::string name;
        uint32_t id; // Negtive value means wait for database assignment
        int minPosition;
        int maxPosition;
    };

    RobotMap(/* args */);
    // RobotMap(RobotMap::Header _header);
    ~RobotMap();
    int setMapHeader(RobotMap::Header _header);
    RobotMap::Header &mapHeader(void);
    RobotMap::Header mapHeader(void) const;

    // Factory
    static RobotMap::Ptr createNewMap(void);
    static RobotMap::Ptr createNewMapbyName(std::string mapName);

private:
    /* data */
    std::mutex m_DataMutex;
    RobotMap::Header m_MapHeader;
};  

#endif