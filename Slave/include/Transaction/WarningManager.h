#ifndef WarningManager_H
#define WarningManager_H

#include "base/SingleTon.h"
// #include "Driver/SensorDeviceDriver.h"
#include "Driver/SensorBoardEx.h"
// #include "Driver/BatteryDeviceDriver.h"
#include "Driver/BatteryDriver.h"
#include "Transaction/WarningService.h"
#include "Transaction/SettingService.h"
#include "Transaction/Motor.h"
#include "Transaction/ThermalImager.h"
#include "Transaction/IndustrialCamera.h"
#include "Transaction/MoveTaskManage/MoveException.h"
#include "Common/HIKClient/HIKClient.h"

#define HOMING_ERROR  0x0002
#define ALARM_BATTERY_TEMP_HIGH             "电池温度过高"
#define ALARM_BATTERY_QUANTITY_LOW          "电池电量过低"
#define ALARM_CH4_HIGH                      "甲烷浓度过高"
#define ALARM_SMOG_HIGH                     "烟雾浓度过高"
#define ALARM_CO_HIGH                       "一氧化碳浓度过高"
#define ALARM_SO2_HIGH                      "二氧化硫浓度过高"
#define ALARM_O2_LOW                        "氧气浓度过低"
#define ALARM_ILLUMINANCE_HIGH              "光照强度过高"
#define ALARM_USS                           "遇到障碍物"
#define ALARM_FIREDOOR                      "防火门未连接"
#define ALARM_MOTOR                         "运动告警"
#define ALARM_HOMING                        "回零告警"
#define ALARM_ROS_LOST                      "ROS连接失败"
#define ALARM_CAMERAM_LOST                  "工业相机连接失败"
#define ALARM_THERMEL_LOST                  "热像仪连接失败"
#define ALARM_THERMAL_TEMP_HIGH             "热像仪检测到温度过高"
#define ALARM_THERMAL_TEMP_LOW              "热像仪检测到温度过低"

#define ALARM_FOREWARD_INFRARED             "前红外有障碍物"
#define ALARM_BACK_INFRARED                 "后红外有障碍物"
#define ALARM_FOREWARD_USS                  "前超声波有障碍物"
#define ALARM_BACK_USS                      "后超声波有障碍物"
#define ALARM_DOWN_USS                      "下超声波有障碍物"

//plc告警信息
struct CallPlcFlag{
    bool gasFlag;
    bool tempFlag;
    bool breakdownFlag;
    bool divulgeFlag;
    bool stoneFlag;
};
//图像识别
struct ImageRecW{
   bool divulgeFlag;
   bool stoneFlag;
};
//环境信息告警
struct robotPlcFlag{
   bool gasFlag;
   bool tempFlag;
   bool breakdownFlag;  
};
struct EventPlcFlag{
    bool eventGasFlag;
    bool eventTempFlag;
    bool eventBreakdownFlag;
    bool eventDivulgeFlag;
    bool eventStoneFlag;
};


class WarningManager
{
public:
  ~WarningManager();
  static WarningManager *getInstance();

  int init(int nRobotID);
  static void run(WarningManager *pWarningManager);

  void addAlarm(AlarmInfo vAlarmInfo);

  int                   m_nRobotID;
 
  std::string float_to_string(const float &value);
  //2020-10-22 获取发送plc的状态位
  CallPlcFlag getCallPlcFlag();
  //设置PLC的标志位
  CallPlcFlag  callFlagresult;
  void setImageRecFlag(struct ImageRecW imageRecFlag);
  EventPlcFlag eventPlcFlag;
  EventPlcFlag getEventPlcFlag();
  robotPlcFlag plcCallType;

private:
  std::string getTime();
  void judgeWarn();

  void setMotorErrcode(int err) { m_nMotorErrcode = err; }
  int getMotorErrcode() { return m_nMotorErrcode; }

private:
  bool                  m_bThreadRun;

  // BatteryDeviceDriver*  m_pBatteryDeviceDriver;
  BatteryDriver*        m_pBatteryDriver;
  // SensorDeviceDriver*   m_pSensorDeviceDriver;
  SensorBoardEx::Ptr      m_pSensorBoardEx;   //传感器驱动，包括气体、环境、污染物信息
  Motor*          m_pMotor;
  // ROSDriver*            m_pROSDriver;
  WarningService*       m_pWarningService;
  IndustrialCamera*     m_pIndustrialCamera;
  ThermalImager*        m_pThermalImager;
  MoveException*        m_pMoveException;
  HIKClient*            m_pHik;

  ImageRecW imageRecFlagN;
  void setCallPlcFlag(std::vector<int> statusN);
  //机器人告警的状态位
  std::vector<int> checkStatus;
 
  int m_nMotorErrcode = 0;
 
};

#endif // WarningManager_H
