/* #include "base/ROSBase.h"
#include <std_msgs/String.h>

ROSBase::~ROSBase()
{
}

ROSBase *ROSBase::getInstance()
{
  return SingleTon<ROSBase>::GetInstance();
}

int ROSBase::initRos(std::string sMasterUrl, std::string sHostUrl, int nRobotNum)
{
  std::map<std::string,std::string> remappings;

  remappings["__master"] = sMasterUrl;
  remappings["__hostname"] = sHostUrl;

  printf("--- ROSBase::initRos %s, %s \n", sMasterUrl.c_str(), sHostUrl.c_str());

  char strNodeName[32] = {0};
  sprintf(strNodeName, "MotorControl%d", nRobotNum);

  ros::init(remappings, strNodeName);

  if ( !ros::master::check() ) {
    return -1;
  }

  //ros::start();

  m_publisherMap.clear();
  m_serviceClientMap.clear();
  m_serviceServerList.clear();

  return 0;
}

bool ROSBase::isRosAlive()
{
  return ros::master::check();
}

void ROSBase::heartBeat()
{
    ros::spinOnce();
} */
