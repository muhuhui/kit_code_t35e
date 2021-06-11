#ifndef MYSQLSERVER_H
#define MYSQLSERVER_H

#include "Common/SingleTon.h"
#include "Driver/MySQLDriver.h"
#include "Transaction/FireDoor.h"
#include "Transaction/Warning.h"
#include "Common/HttpPost/CHttpPost.hpp"
#include <set>

using namespace std;



#define camera_tcp_port "camera_tcp_port"
#define camera_udp_port "camera_udp_port"
#define thermal_imager_ip "thermal_imager_ip"
#define thermal_imager_tcp_port "thermal_imager_tcp_port"
#define thermal_imager_udp_port "thermal_imager_udp_port"
#define alarm_temperature "alarm_temperature"
#define warm_temperature "warm_temperature"
#define combustible_gas_alarm_concentration "combustible_gas_alarm_concentration"
#define co_alarm_concentration "co_alarm_concentration"
#define h2s_alarm_concentration "h2s_alarm_concentration"
#define o2_alarm_concentration "o2_alarm_concentration"
#define Illuminance_alarm_concentration "Illuminance_alarm_concentration"
#define Smog_alarm_concentration "Smog_alarm_concentration"
#define obstacle_alarm_distance "obstacle_alarm_distance"
#define fire_door_alarm_distance "fire_door_alarm_distance"
#define orbit_start_position "orbit_start_position"
#define orbit_end_position "orbit_end_position"
#define charging_pile_position "charging_pile_position"
#define battery_alarm_quantity "battery_alarm_quantity"
#define thermal_imager_tcp_port_converter "thermal_imager_tcp_port_converter"
#define thermal_imager_ip_converter "thermal_imager_ip_converter"
#define battery_alarm_temperature "battery_alarm_temperature"


#define ALARM_ROS_LOST                      "ROS连接失败"
#define ALARM_CAMERAM_LOST                  "工业相机连接失败"
#define ALARM_THERMEL_LOST                  "热像仪连接失败"
#define ALARM_OBSTACLE                      "遇到障碍物"
#define ALARM_BATTERY_TEMP_HIGH             "电池温度过高"
#define ALARM_BATTERY_QUANTITY_LOW          "电池电量过低"
#define ALARM_CH4_HIGH                      "甲烷浓度过高"
#define ALARM_CO_HIGH                       "一氧化碳浓度过高"
#define ALARM_SO2_HIGH                      "二氧化硫浓度过高"
#define ALARM_O2_LOW                        "氧气浓度过低"
#define ALARM_FIRE                          "发生火灾"
#define ALARM_TEMP_HIGH                     "温度过高"
#define ALARM_INFRARED                      "红外告警"
#define ALARM_ILLUMINANCE_HIGH              "光照强度过高"
#define ALARM_SMOG_HIGH                     "烟雾浓度过高"
#define ALARM_MOTOR_WARN                    "电机报警"
#define ALARM_FIREDOOR_OFFLINE              "防火门掉线"


#define ALARM_INTERVAL                      1800
#define ALARM_POSITION_DELTA                300

//#define ALARM_ACTION_SHOW_NOTICE           1
#define ALARM_ACTION_VOICE                 2
#define ALARM_ACTION_STOP_ROBOT             3
#define ALARM_ACTION_CLOSE_FIREDOOR         4


/*enum FireDoorStatus {
#  FIRE_DOOR_LOST = -2,
#  FIRE_DOOR_ERR = -1,
#  FIRE_DOOR_CLOSED = 0,
#  FIRE_DOOR_OPEN = 1
#};
*/

typedef Poco::Tuple<int, int> TupleAlarmAction;
typedef Poco::Tuple<int, string, double, string> TupleCharge;
typedef Poco::Tuple<string,string> TupleIsolatedZone;

struct AlarmCheckItems
{
  int     nRobotId;
  int     nMotorWarn;

  float   fCurPosition;

  float   fThermelMaxTemp;

  float   fCombustible;
  float   fCO;
  float   fH2S;
  float   fO2;

  float   fIlluminance;
  float   fSmog;

  float   fBatteryTemp;

  float   fObstacleDistance;
  
};

//struct AlarmInfo
//{
//  int       nAlarmID;
//  int       nItemID;
//  int       nRobotID;
//  double    dPositon;

//  std::string   strItemName;
//  std::string   strLevelName;
//  std::string   strReason;
//  std::string   strAlarmTime;

//  time_t        tAlarmTime;
//};

enum ExecResult
{
    EXEC_SLEEP = -1,
    EXEC_SUCCESS = 0,
    EXEC_FAIL,
    EXEC_RUNNING,
};

struct ViewPointInfo
{
  int     nViewPointId;
  float   fTemperature;
  float   fHumidity;
  float   fCombustible;
  float   fCO;
  float   fH2S;
  float   fO2;
  float   fThermelMaxTemp;
  std::string     strCameraPhotoPath;
  std::string     strThermelPhotoPath;
  std::string     strCameraVideoPath;
  std::string     strThermelVideoPath;
  int     nposition;
  int     TEV;
  int     UW;

  int     nTaskId;
  int     nExecId;
  int     nExecTime;
  int     nExecRes;
};

enum EquAlarmRules{
    EQU_ALARM_BT = 1,
    EQU_ALARM_LT,
    EQU_ALARM_EQ,
    EQU_ALARM_BE,
    EQU_ALARM_LE,
    EQU_ALARM_IN,
    EQU_ALARM_NOTIN,
    EQU_ALARM_RANGE
};

struct EquInfo
{
    int nEquId;
    int nInnerId;
    std::string sEquName;
    int nAlarmDType;
    std::string sAlarmMin;
    std::string sAlarmMax;
    int nAlarmRule;
    int nEquType; //数据类型
    std::string sMinRange;
    std::string sMaxRange;
    Poco::JSON::Object valueMap;
    int ntypeId;
    std::string sitemUnit;
    std::string sbBox;
    std::set<int> objectSet;
};

struct EqusInfo
{
    std::string srefImgPath;
    std::string sworkImgPath;
    int nworkFlag;
    std::list<EquInfo> meterInfo;
    int transId;
    int nworkId;
};

typedef Poco::Tuple<int, int, string, int, string, string, int, int, string, string, int, string, string> TupleEquInfo;
typedef Poco::Tuple<int, string> ValueMapInfo;
typedef Poco::Tuple<int, string, string> PicPathInfo;

struct PictureInfo
{
    time_t createTime;
    float position;
    std::string path;
    int nRobotId;
};

class MySQLServer
{
public:
  ~MySQLServer();
  static MySQLServer *getInstance();

  int init(std::string sServerIp, std::string sServerPort, std::string sDataBase,
           std::string sUserName, std::string sPasswd);

  //
  std::string getSettingParam(std::string strName,int robortId);
  bool setSettingParam(std::string strName, std::string strValue,int robort );

  //
  int recordPictureInfo(PictureInfo ePictureInfo);
  int recordViewPointInfo(ViewPointInfo eViewPointInfo);
  int recordImageIdentifyResult(ViewPointInfo viewPointInfo, EquInfo equInfo, CHttpPost::IdentifyResult result);
  int insertExecResult(ViewPointInfo &eViewPointInfo);
  int changeExecResult(ViewPointInfo eViewPointInfo);
  int getCurExecId(ViewPointInfo &eViewPointInfo);
  int queryPointEqusInfo(ViewPointInfo eViewPointInfo, EqusInfo &equsInfo);
  int queryPointEquInfo(ViewPointInfo eViewPointInfo, std::list<EquInfo> &equInfoList);
  int queryValueMap(int dataType, Poco::JSON::Object &valueMap);
  int equsInfo2Json(EqusInfo equsInfo, Poco::JSON::Object &json);
  int querySet(int equId, std::set<int>& objectSet);

  //
  std::string getSpeed() { return getSettingParam(default_speed,m_nRobotID); }

  std::string getCameraIp() { return getSettingParam(camera_ip, m_nRobotID); }
  std::string getThermelIp() { return getSettingParam(thermal_imager_ip, m_nRobotID); }

  //
  std::string getThermelAlarmTemp() { return getSettingParam(alarm_temperature, m_nRobotID); }
  std::string getThermelWarmTemp() { return getSettingParam(warm_temperature,m_nRobotID); }

  std::string getCombustibleAlarmValue() { return getSettingParam(combustible_gas_alarm_concentration,m_nRobotID); }
  std::string getCOAlarmValue() { return getSettingParam(co_alarm_concentration,m_nRobotID); }
  std::string getH2SAlarmValue() { return getSettingParam(h2s_alarm_concentration,m_nRobotID); }
  std::string getO2AlarmValue() { return getSettingParam(o2_alarm_concentration,m_nRobotID); }

  std::string getIlluminanceAlarmValue() { return getSettingParam(Illuminance_alarm_concentration,m_nRobotID); }
  std::string getSmogAlarmValue() { return getSettingParam(Smog_alarm_concentration,m_nRobotID); }

  std::string getObstacleAlarmValue() { return getSettingParam(obstacle_alarm_distance,m_nRobotID); }

  std::string getBatteryAlarmValue() { return getSettingParam(battery_alarm_temperature,m_nRobotID); }

  //
  bool queryAlarmNearby(AlarmInfo vAlarmInfo, int nIntervalSeconds);
  float queryAlarmNearbydis(AlarmInfo vAlarmInfo);
  //void getAlarmAction(std::vector<TupleAlarmAction> &vTupleAlarmAction);
  void getAlarmAction(std::string strItemName, std::vector<int>& actionid);
  void getAvoidObstacleByID(std::vector<TupleIsolatedZone> &vTupleIsolatedZone);
  void storeRfidInfo(int rfid,int rfidtype,int rfidpos);
  int insertAlarm(AlarmInfo vAlarmInfo);
  void getCharge(std::vector<TupleCharge> &vTupleCharge);
  void modifyCharge(string qIP, double dPos, string nStatus, int nID);
  int getFireDoorIp(int fireID, std::string &sfireIp);

private:
  MySQLDriver*    m_pMySQLDriver;
public:

  int             m_nRobotID;
};

#endif // MYSQLSERVER_H
