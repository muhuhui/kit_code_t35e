#ifndef WARNINGSERVICE_H
#define WARNINGSERVICE_H

#include "base/SingleTon.h"
#include "Transaction/Warning.h"
#include "Driver/MySQLDriver.h"


#define battery_alarm_temperature "battery_alarm_temperature"
#define ch4_alarm_concentration "ch4_alarm_concentration"
#define co_alarm_concentration "co_alarm_concentration"
#define so2_alarm_concentration "so2_alarm_concentration"
#define o2_alarm_concentration "o2_alarm_concentration"
#define Illuminance_alarm_concentration "Illuminance_alarm_concentration"
#define Smog_alarm_concentration "Smog_alarm_concentration"
#define alarm_temperature "alarm_temperature"
#define warn_temperature "warm_temperature"


class WarningService
{
public:
  ~WarningService();
  static WarningService *getInstance();

  int init();
  void getThreshold(Warning &warn);

  bool queryAlarmNearby(AlarmInfo vAlarmInfo, int nIntervalSeconds);
  float queryAlarmNearbydis(AlarmInfo vAlarmInfo);
  int insertAlarm(AlarmInfo vAlarmInfo);

private:
  std::string getSettingParam(std::string strName, int nRobotID);

  std::string getBatteryAlarmValue(int nRobotID) { return getSettingParam(battery_alarm_temperature, nRobotID); }

  std::string getCH4AlarmValue(int nRobotID) { return getSettingParam(ch4_alarm_concentration, nRobotID); }
  std::string getCOAlarmValue(int nRobotID) { return getSettingParam(co_alarm_concentration, nRobotID); }
  std::string getSO2AlarmValue(int nRobotID) { return getSettingParam(so2_alarm_concentration, nRobotID); }
  std::string getO2AlarmValue(int nRobotID) { return getSettingParam(o2_alarm_concentration, nRobotID); }

  std::string getIlluminanceAlarmValue(int nRobotID) { return getSettingParam(Illuminance_alarm_concentration, nRobotID); }
  std::string getSmogAlarmValue(int nRobotID) { return getSettingParam(Smog_alarm_concentration, nRobotID); }
  std::string getHighAlarmValue(int nRobotID) { return getSettingParam(alarm_temperature, nRobotID); }
  std::string getLowAlarmValue(int nRobotID) { return getSettingParam(warn_temperature, nRobotID); }

private:
  MySQLDriver*       m_pPocoBase;
};

#endif // WARNINGSERVICE_H
