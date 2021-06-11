#include "Transaction/ConfigServer.h"


ConfigServer::~ConfigServer()
{

}

ConfigServer::ConfigServer()
{
//	init();
}


ConfigServer *ConfigServer::getInstance()
{
  return SingleTon<ConfigServer>::GetInstance();
}

int ConfigServer::init()
{
  int nRet = 0;
  std::string sConfigPath = "./Config/ParametersConfig.ini";
  std::string sInitPosPath = "./Config/initPos.ini";

  m_nRobotID = 0;
  m_nTodayPhotoIndex = 0;
  m_nCurrPosition = 0;

  m_pParamINIParser = new INIParser();
  nRet = m_pParamINIParser->ReadINI(sConfigPath);
  if(nRet != 1) {
    printf("--- ConfigServer::init ConfigPath Error \n");
    return -1;
  }
  loadParameters();
  m_pInitPosINIParser = new INIParser();
  nRet = m_pParamINIParser->ReadINI(sInitPosPath);
  if(nRet != 1) {
    printf("--- ConfigServer::init ConfigPath Error \n");
    return -2;
  }
  loadInitPos();
  return 0;
}

int ConfigServer::loadParameters()
{

  m_nRobotID = atoi(m_pParamINIParser->GetValue("Robot", "RobotId").c_str());
  m_nTodayPhotoIndex = atoi(m_pParamINIParser->GetValue("Robot", "PhotoIndex").c_str());
  m_sPhotoRootPath = m_pParamINIParser->GetValue("Robot", "PhotoRootPath");
  m_sVideoRootPath = m_pParamINIParser->GetValue("Robot", "VideoRootPath");
  //获取相机的用户名和密码
  m_sCameraUserName = m_pParamINIParser->GetValue("Robot", "CameraUserName");
  m_sCameraPassword = m_pParamINIParser->GetValue("Robot", "CameraPassword");
  m_sCameraIp = m_pParamINIParser->GetValue("Robot", "CameraIp");
  m_sRobotVersion = m_pParamINIParser->GetValue("Robot", "Version");
  m_sSoftVersion = m_pParamINIParser->GetValue("Robot", "SoftVersion");
  
  m_TcpIpInfo.sMainIp = m_pParamINIParser->GetValue("TcpIp", "MainBoardIp");
  m_TcpIpInfo.nMainPort = atoi(m_pParamINIParser->GetValue("TcpIp", "MainBoardPort").c_str());
  m_TcpIpInfo.sPTZIp = m_pParamINIParser->GetValue("TcpIp", "PTZBoardIp");
  m_TcpIpInfo.nPTZPort = atoi(m_pParamINIParser->GetValue("TcpIp", "PTZBoardPort").c_str());
  m_TcpIpInfo.sEnvIp = m_pParamINIParser->GetValue("TcpIp", "EnvBoardIp");
  m_TcpIpInfo.nEnvPort = atoi(m_pParamINIParser->GetValue("TcpIp", "EnvBoardPort").c_str());

  m_SerialDeviceInfo.sSensorDevice = m_pParamINIParser->GetValue("SerialPort", "SensorDevice");
  m_SerialDeviceInfo.nSensorBaud = atoi(m_pParamINIParser->GetValue("SerialPort", "SensorBaud").c_str());
  m_SerialDeviceInfo.sBatteryDevice = m_pParamINIParser->GetValue("SerialPort", "BatteryDevice");
  m_SerialDeviceInfo.nBatteryBaud = atoi(m_pParamINIParser->GetValue("SerialPort", "BatteryBaud").c_str());
  m_SerialDeviceInfo.nParity = atoi(m_pParamINIParser->GetValue("SerialPort", "Parity").c_str());
  m_SerialDeviceInfo.nDataBit = atoi(m_pParamINIParser->GetValue("SerialPort", "DataBit").c_str());
  m_SerialDeviceInfo.nStopBit = atoi(m_pParamINIParser->GetValue("SerialPort", "StopBit").c_str());

  m_SerialDeviceInfo.sPddDevice = m_pParamINIParser->GetValue("SerialPort", "PddDevice").c_str();
  m_SerialDeviceInfo.nPddBaud = atoi(m_pParamINIParser->GetValue("SerialPort", "PddBaud").c_str());
  m_SerialDeviceInfo.nPddParity = atoi(m_pParamINIParser->GetValue("SerialPort", "PddParity").c_str());
  m_SerialDeviceInfo.nPddDataBit = atoi(m_pParamINIParser->GetValue("SerialPort", "PddDataBit").c_str());
  m_SerialDeviceInfo.nPddStopBit = atoi(m_pParamINIParser->GetValue("SerialPort", "PddStopBit").c_str());
  m_SerialDeviceInfo.nPddSlave_ID = atoi(m_pParamINIParser->GetValue("SerialPort", "PddSlave_ID").c_str());

  m_SerialDeviceInfo.sPddDecDevice = m_pParamINIParser->GetValue("SerialPort", "PddDecDevice").c_str();
  m_SerialDeviceInfo.nPddDecBaud = atoi(m_pParamINIParser->GetValue("SerialPort", "PddDecBaud").c_str());
  m_SerialDeviceInfo.nPddDecParity = atoi(m_pParamINIParser->GetValue("SerialPort", "PddDecParity").c_str());
  m_SerialDeviceInfo.nPddDecDataBit = atoi(m_pParamINIParser->GetValue("SerialPort", "PddDecDataBit").c_str());
  m_SerialDeviceInfo.nPddDecStopBit = atoi(m_pParamINIParser->GetValue("SerialPort", "PddDecStopBit").c_str());

  m_RadarInfo.sRadarHardware = m_pParamINIParser->GetValue("SerialPort", "RadarHardware");
  
  //添加后雷达屏蔽最小距离和最大距离
  limitRadarback.mixLimit = atoi(m_pParamINIParser->GetValue("SerialPort", "MIXRADAR_BACK").c_str());
  limitRadarback.maxLimit = atoi(m_pParamINIParser->GetValue("SerialPort", "MAXRADAR_BACK").c_str());
  // In future: using one-hot key addition means SensorBoardVer.
  m_nSensorBoardVer = atoi(m_pParamINIParser->GetValue("SensorBoard", "HasPollution").c_str());

  m_MySQLInfo.sIp = m_pParamINIParser->GetValue("MySQL", "Ip");
  m_MySQLInfo.sPort = m_pParamINIParser->GetValue("MySQL", "Port");
  m_MySQLInfo.sDataBase = m_pParamINIParser->GetValue("MySQL", "DataBase");
  m_MySQLInfo.sUser = m_pParamINIParser->GetValue("MySQL", "User");
  m_MySQLInfo.sPasswd = m_pParamINIParser->GetValue("MySQL", "Passwd");

  m_MotorInfo.moveMotorId = atoi(m_pParamINIParser->GetValue("Motor", "MoveMotorId").c_str());
  m_MotorInfo.liftMotorId = atoi(m_pParamINIParser->GetValue("Motor", "LiftMotorId").c_str());

  m_bIsNewHope = atoi(m_pParamINIParser->GetValue("Robot", "IsNewHope").c_str()); 
  m_sFrontPoint = m_pParamINIParser->GetValue("Map", "FrontPoint").c_str();

  m_TimeInfo.ArmDisable = atoi(m_pParamINIParser->GetValue("Time", "ArmDisable").c_str());
  m_TimeInfo.PtzRotate = atoi(m_pParamINIParser->GetValue("Time", "PtzRotate").c_str());
  m_TimeInfo.Recharge = atoi(m_pParamINIParser->GetValue("Time", "Recharge").c_str());
  m_TimeInfo.VideoDuration = atoi(m_pParamINIParser->GetValue("Time", "VideoDuration").c_str());

  m_nOnvif = atoi(m_pParamINIParser->GetValue("Camera", "IsSupportOnvif").c_str());

  return 0;
}

int ConfigServer::loadInitPos()
{
  m_nCurrPosition = atoi(m_pParamINIParser->GetValue("InitPos", "Pos").c_str());
}

void ConfigServer::saveInitPos(int nPos)
{
  m_pParamINIParser->SetValue("InitPos", "Pos", std::to_string(nPos));
  m_pParamINIParser->saveINI();
}

int ConfigServer::getRobotId()
{
  return m_nRobotID;
}

std::string ConfigServer::getRobotVersion()
{
  return m_sRobotVersion;
}

std::string ConfigServer::getSoftVersion()
{
  return m_sSoftVersion;
}

std::string ConfigServer::getPhotoRootPath()
{
  return m_sPhotoRootPath;
}

std::string ConfigServer::getVideoRootPath()
{
  return m_sVideoRootPath;
}

//获取相机的用户名
std::string ConfigServer::getCameraUserName()
{
  return m_sCameraUserName;
}
//获取相机的密码
std::string ConfigServer::getCameraPassword()
{
  return m_sCameraPassword;
}


int ConfigServer::getInitPosition()
{
  return m_nCurrPosition;
}

void ConfigServer::setCurrPosition(int nPos)
{
  m_nCurrPosition = nPos;
}

const TcpIpInfo *ConfigServer::getTcpIpInfo()
{
  return &m_TcpIpInfo;
}

const SerialDeviceInfo *ConfigServer::getSerialDeviceInfo()
{
  return &m_SerialDeviceInfo;
}

const int 
ConfigServer::getSensorBoardVer()
{
  return m_nSensorBoardVer;
}

const MySQLInfo *ConfigServer::getMySQLInfo()
{
  return &m_MySQLInfo;
}

const MotorCfgInfo *ConfigServer::getMotorInfo()
{
  return &m_MotorInfo;
}

const RadarInfo *ConfigServer::getRadarInfo()
{
  return &m_RadarInfo;
}

const string ConfigServer::getFrontPoint()
{
  return m_sFrontPoint;
}

const TimeInfo *ConfigServer::getTimeInfo()
{
  return &m_TimeInfo;
}

const std::string ConfigServer::getCameraIp()
{
  return m_sCameraIp;
}
//屏蔽后雷达
const LimitRadarback *ConfigServer::getLimitRadarback()
{
    return &limitRadarback;
}

int ConfigServer::getSupportOnvif()
{
    return m_nOnvif;
}
