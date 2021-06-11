#ifndef CONFIGSERVER_H
#define CONFIGSERVER_H

#include "base/SingleTon.h"
#include "base/INIParser.h"
#include <string>

struct TimeInfo {
  int    ArmDisable;
  int    PtzRotate;
  int    Recharge;
  int    VideoDuration;
};

//屏蔽后雷达距离值
struct LimitRadarback{
     int mixLimit;
     int maxLimit;
};


struct TcpIpInfo {
  std::string     sMainIp;
  int             nMainPort;
  std::string     sPTZIp;
  int             nPTZPort;
  std::string     sEnvIp;
  int             nEnvPort;
};

struct SerialDeviceInfo {
  std::string     sBatteryDevice;
  int             nBatteryBaud;
  std::string     sSensorDevice;
  int             nSensorBaud;
  int             nParity;
  int             nDataBit;
  int             nStopBit;
  std::string     sPddDevice;
  int             nPddBaud;
  int             nPddParity;
  int             nPddDataBit;
  int             nPddStopBit;
  int             nPddSlave_ID;
  std::string     sPddDecDevice;
  int             nPddDecBaud;
  int             nPddDecParity;
  int             nPddDecDataBit;
  int             nPddDecStopBit;
};


struct MySQLInfo
{
  std::string     sIp;
  std::string     sPort;
  std::string     sDataBase;
  std::string     sUser;
  std::string     sPasswd;
};

struct MotorCfgInfo
{
  int moveMotorId;
  int liftMotorId;
};

struct RadarInfo
{
  std::string      sRadarHardware;
};

class ConfigServer
{
public:
  ~ConfigServer();
  ConfigServer();
  static ConfigServer *getInstance();

  int init();
  int getRobotId();

  std::string getPhotoRootPath();
  std::string getVideoRootPath();

  std::string getRobotVersion();    //获取机器人型号
  std::string getSoftVersion();     //获取软件版本号
  std::string getCameraUserName();  //获取相机的用户名
  std::string getCameraPassword();  //获取相机的密码
  int getInitPosition();
  void setCurrPosition(int nPos);

  const TcpIpInfo *getTcpIpInfo();
  const SerialDeviceInfo *getSerialDeviceInfo();
  const MySQLInfo *getMySQLInfo();
  const MotorCfgInfo *getMotorInfo();
  const int getSensorBoardVer();
  const RadarInfo *getRadarInfo();
  bool isNewHope() { return m_bIsNewHope; }
  const string getFrontPoint();
  const TimeInfo *getTimeInfo();
  const std::string getCameraIp();
  //屏蔽后雷达
  const LimitRadarback *getLimitRadarback();
  int getSupportOnvif();

private:
  int loadParameters();

  int loadInitPos();
  void saveInitPos(int nPos);

private:
  int                     m_nRobotID;
  int                     m_nTodayPhotoIndex;
  int                     m_nCurrPosition;
  int                     m_nSensorBoardVer;
  std::string             m_sPhotoRootPath;
  std::string             m_sVideoRootPath;
  std::string			        m_sCameraUserName;
  std::string			        m_sCameraPassword;
  std::string             m_sRobotVersion;
  std::string             m_sSoftVersion;
  std::string             m_sCameraIp;
  int                     m_nOnvif;

  TcpIpInfo               m_TcpIpInfo;
  SerialDeviceInfo        m_SerialDeviceInfo;
  MySQLInfo               m_MySQLInfo;
  MotorCfgInfo            m_MotorInfo;
  RadarInfo               m_RadarInfo;
  TimeInfo                m_TimeInfo;

  INIParser*              m_pParamINIParser;
  INIParser*              m_pInitPosINIParser;
  bool                    m_bIsNewHope;
  std::string             m_sFrontPoint;
  //后雷达屏蔽屏蔽
  LimitRadarback limitRadarback;
};


#endif // CONFIGSERVER_H
