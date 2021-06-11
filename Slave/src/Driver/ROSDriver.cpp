/*
 * @Author: your name
 * @Date: 2020-11-16 18:04:55
 * @LastEditTime: 2020-11-17 10:01:49
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \cod\NEW_SLAVE\Slave\src\Driver\ROSDriver.cpp
 */
/* #include "Driver/ROSDriver.h"
#include <std_msgs/String.h>

#include "Driver/MyLog4CXX.h"

const std::string LOG_TAG("root.ROSDriver::");

ROSDriver::~ROSDriver()
{
    if(ros::isStarted()) {
        printf("ros::shutdown\n");
        ros::shutdown();
        ros::waitForShutdown();
    }
}

ROSDriver *ROSDriver::getInstance()
{
    return SingleTon<ROSDriver>::GetInstance();
}

int ROSDriver::initRos(std::string sMasterUrl, std::string sHostUrl, int nRobotNum)
{
    auto logger = initLogger(LOG_TAG + std::string(__FUNCTION__));
    std::map<std::string,std::string> remappings;

    remappings["__master"] = sMasterUrl;
    remappings["__hostname"] = sHostUrl;

    printf("--- ROSDriver::initRos %s, %s\n", sMasterUrl.c_str(), sHostUrl.c_str());
    LOG4CPLUS_INFO(logger, sMasterUrl.c_str() << " " << sHostUrl.c_str());
    char strNodeName[32] = {0};
    sprintf(strNodeName, "OrbitalRobotSlave%d", nRobotNum);

    ros::init(remappings, strNodeName);

    if (!ros::master::check()) {    //检查是否可连接
        printf("--- ROSDriver::initRos %s, %s loss connection\n", sMasterUrl.c_str(), sHostUrl.c_str());
        LOG4CPLUS_INFO(logger, sMasterUrl.c_str() << " " << sHostUrl.c_str() << " loss connection");
        
        return -1;
    }

     if (ros::isStarted()) {     //ros断连自清除
        ros::shutdown();
        ros::waitForShutdown();
        printf("ros::shutdown\n");
    }

    ros::start();

    m_publisherMap.clear();
    m_serviceServerList.clear();
    m_subscriberMap.clear();
    m_serviceClientMap.clear();

    return 0;
}

bool ROSDriver::isRosAlive()
{
    return ros::master::check();
}

void ROSDriver::heartBeat()
{
    ros::spinOnce();
}

void ROSDriver::spin()
{
	ros::spin();
} */

