#ifndef SETTINGSERVICE_H
#define SETTINGSERVICE_H

#include "base/SingleTon.h"
#include "Transaction/Setting.h"
#include "Driver/MySQLDriver.h"

class SettingService
{
public:
  ~SettingService();
  SettingService();
  static SettingService *getInstance();
  int init(); 
  int querySettingByRobotId(int robotId);
  Setting m_setting;
 
private:
  std::string getSettingParam(std::string strName, int nRobotID);

private:
  MySQLDriver*       m_pPocoBase;
};

#endif // SETTINGSERVICE_H
