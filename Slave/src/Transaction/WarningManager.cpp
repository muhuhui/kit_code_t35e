#include "Transaction/WarningManager.h"
#include <thread>
#include <stdio.h>
#include <unistd.h>    
#include "Driver/MyLog4CXX.h"
#include "sys/time.h"

const std::string LOG_TAG("root.WarningManager::");


#define ALARM_POSITION_DELTA                1000         //1m不重复告警
#define MIN_ALARM_INTERVEL                  300          //至少5分钟告警一次

WarningManager::~WarningManager()
{
  m_bThreadRun = false;
}

WarningManager *WarningManager::getInstance()
{
  return SingleTon<WarningManager>::GetInstance();
}

int WarningManager::init(int nRobotID)
{
  m_bThreadRun = true;
  m_nRobotID = nRobotID;

  // m_pBatteryDeviceDriver = BatteryDeviceDriver::getInstance();
  m_pBatteryDriver = BatteryDriver::getInstance();
  // m_pSensorDeviceDriver = SensorDeviceDriver::getInstance();
  m_pSensorBoardEx = SensorBoardEx::getInstance();
  m_pMotor = Motor::getInstance();
  // m_pROSDriver = ROSDriver::getInstance();

  m_pWarningService = WarningService::getInstance();
  m_pWarningService->init();

  m_pMoveException = MoveException::getInstance();

  m_pIndustrialCamera = IndustrialCamera::getInstance();
  m_pThermalImager = ThermalImager::getInstance();
  m_pHik = HIKClient::getInstance();
  plcCallType={0};
  for(int i=0; i<14; i++)
  {
     checkStatus.push_back(0);
  }
  std::thread runThread;
  runThread = std::thread(WarningManager::run, this);
  runThread.detach(); 
  return 0;
}

enum WarnNum{
   ILLUMINANCE = 0,
   BATTERY_TEMP,
   BATTERY_QUANTITY,
   CH4,
   CO,
   SO2,
   O2,
   SMOG,
   ULTRA_FRONT,
   ULTRA_BACK,

   MOTOR_DISCONNECT,
   ROS_DISCONNECT,
   CAMERA_DISCONNECT,
   THERMAL_DISCONNECT,

   TOTAL_WARN_NUM
};
enum InstantWarnNum{
  THERMAL_HIGH = 0,
  THERMAL_LOW,
  INSTANT_TOTAL_WARN_NUM
};
const bool needPositionCheck[TOTAL_WARN_NUM] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0};

//告警:每10s统计一次
//环境告警:电池、气体、光照、温度、避障等告警，1m内每5min告警一次，过1m每10s告警一次
//系统告警:电机、云台、通信未连接告警，每5min告警一次
void WarningManager::judgeWarn()
{
  std::string sCurTime = getTime();
  time_t curTime = time(0);
  static bool warnFlag[TOTAL_WARN_NUM] = {0};
  static int count[TOTAL_WARN_NUM] = {0};
  static bool instantwarnFlag[INSTANT_TOTAL_WARN_NUM] = {0};
  static time_t lastWarnTime[TOTAL_WARN_NUM] = {0};
  std::string logname = LOG_TAG + std::string(__FUNCTION__);
  auto logger = initLogger(logname);
  static int secondCnt = 0;
  static float pre_mintemp = 0.0;
  static float pre_maxtemp = 0.0;
  Warning warn;
  warn.setRobotID(m_nRobotID);
  
  m_pWarningService->getThreshold(warn);

  AlarmInfo vAlarmInfo;
  vAlarmInfo.strAlarmTime = sCurTime;
  vAlarmInfo.dPosition = m_pMotor->getCurPosition();
  vAlarmInfo.nRobotID = m_nRobotID;
  //vAlarmInfo.strValue = "";
  vAlarmInfo.strValue = "0";

  //最高最低温度不是初始值，也不是上一次的值
  if(0 == m_pThermalImager->detectDevice()){
    if(m_pHik->getMaxTemp() - 300.0 < -1.0 && m_pHik->getMinTemp() + 50.0 > 1.0 &&
      ((m_pHik->getMaxTemp() -  pre_maxtemp > 0.01 || m_pHik->getMaxTemp() - pre_maxtemp < -0.01) || (m_pHik->getMinTemp() - pre_mintemp > 0.01 || m_pHik->getMinTemp() - pre_mintemp < -0.01)))
    {
      pre_maxtemp = m_pHik->getMaxTemp();
      pre_mintemp = m_pHik->getMinTemp();
      if(m_pHik->getMinTemp() < warn.getThermalCameraLowThreshold()) {
          instantwarnFlag[THERMAL_LOW] = true;
      }
      if(m_pHik->getMaxTemp() > warn.getThermalCameraHighThreshold()) {
          instantwarnFlag[THERMAL_HIGH] = true;
      }
    }
  }
//   if(m_pSensorDeviceDriver->getEnvInfo()->fIlluminance > warn.getIlluminanceThreshold()) {
  if(m_pSensorBoardEx->getEnvInfo().fIlluminance > warn.getIlluminanceThreshold()) {
      warnFlag[ILLUMINANCE] = true;
  }
  //printf("illuminance threshold: %f\n", warn.getIlluminanceThreshold());

  if(m_pBatteryDriver->getTemperature() > warn.getBatteryTempThreshold()) {
      warnFlag[BATTERY_TEMP] = true;
  }
  // printf("battery temp threshold: %f\n", warn.getBatteryTempThreshold());
  if(m_pBatteryDriver->getQuantity() < SettingService::getInstance()->m_setting.getBatteryQuantityMin() && m_pBatteryDriver->getQuantity() != 0) {
      warnFlag[BATTERY_QUANTITY] = true;
  }

//   if(m_pSensorDeviceDriver->getGasInfo()->fCH4 > warn.getCH4Threshold()) {
  if(m_pSensorBoardEx->getGasInfo().fCH4 > warn.getCH4Threshold()) {
      warnFlag[CH4] = true;
  }
  // printf("ch4 threshold: %f\n", warn.getCH4Threshold());

//   if(m_pSensorDeviceDriver->getGasInfo()->fCO > warn.getCOThreshold()) {
  if(m_pSensorBoardEx->getGasInfo().fCO > warn.getCOThreshold()) {
      warnFlag[CO] = true;
  }
  // printf("CO threshold: %f\n", warn.getCOThreshold());

//   if(m_pSensorDeviceDriver->getGasInfo()->fSO2 > warn.getSO2Threshold()) {
  if(m_pSensorBoardEx->getGasInfo().fSO2 > warn.getSO2Threshold()) {
      warnFlag[SO2] = true;
  }
  // printf("SO2 threshold: %f\n", warn.getSO2Threshold());

//   if(m_pSensorDeviceDriver->getGasInfo()->fO2 < warn.getO2Threshold()) {
  if(m_pSensorBoardEx->getGasInfo().fO2 < warn.getO2Threshold()) {
      warnFlag[O2] = true;
  }
  // printf("O2 threshold: %f\n", warn.getO2Threshold());

//   if(m_pSensorDeviceDriver->getGasInfo()->fSmog) {
  if(m_pSensorBoardEx->getGasInfo().fSmog) {
      warnFlag[SMOG] = true;
  }
  // printf("smog: %f\n", m_pSensorDeviceDriver->getGasInfo()->fSmog);

  if(m_pMoveException->getForewardUSSExcep()) {
      warnFlag[ULTRA_FRONT] = true;
      printf("warning manager: ultra front\n");
      LOG4CPLUS_WARN(logger, "warning manager: ultra front\n");
  }

  if(m_pMoveException->getBackUSSExcep()) {
      warnFlag[ULTRA_BACK] = true;
      printf("warning manager: ultra back\n");
      LOG4CPLUS_WARN(logger, "warning manager: ultra back\n");

  }

  //if (m_pMotor->getMotorInfo().nErrNum == 99) {
  if (m_pMotor->getMotorInfo().nErrNum) {
	  setMotorErrcode(m_pMotor->getMotorInfo().nErrNum);
      warnFlag[MOTOR_DISCONNECT] = true;
      printf("warning manager: motor error num %d\n", m_pMotor->getMotorInfo().nErrNum);
      LOG4CPLUS_WARN(logger, "warning manager: motor error num " << m_pMotor->getMotorInfo().nErrNum);

  }

/*   if(!m_pROSDriver->isRosAlive()) {
      warnFlag[ROS_DISCONNECT] = true;
      printf("warning manager: ros disconnect\n");
      LOG4CPLUS_WARN(logger, "warning manager: ros disconnect");
  } */
  LOG4CPLUS_INFO(logger, "tag=warming pIndustrialCamera status: " << m_pIndustrialCamera->detectDevice());
  secondCnt++;
  if (secondCnt % 60 == 0) {
    if(0 != m_pIndustrialCamera->detectDevice()) {
      warnFlag[CAMERA_DISCONNECT] = true;
      printf("warning manager: camera disconnect\n");
      vAlarmInfo.strReason = "工业相机连接失败";
      vAlarmInfo.strItemName = ALARM_CAMERAM_LOST;
      m_pWarningService->insertAlarm(vAlarmInfo);
    }
  }

  if (secondCnt % 60 == 30) {
    if(0!= m_pThermalImager->detectDevice()) {
      warnFlag[THERMAL_DISCONNECT] = true;
      printf("warning manager: thermal disconnect\n");
      vAlarmInfo.strReason = "热像仪连接失败";
      vAlarmInfo.strItemName = ALARM_THERMEL_LOST;
      m_pWarningService->insertAlarm(vAlarmInfo);
    }
  }

  for(int instantwarnNum=0; instantwarnNum<INSTANT_TOTAL_WARN_NUM; instantwarnNum++) {
  if(instantwarnFlag[instantwarnNum] == true) {
      instantwarnFlag[instantwarnNum] = false;
      switch(instantwarnNum) {
        case THERMAL_HIGH:
        {
          if(!m_pThermalImager->detectDevice()){
              string str = float_to_string(m_pHik->getMaxTemp());
              string x_pos = float_to_string(m_pMotor->getCurPosition(0)/1000);
              string y_pos = float_to_string(m_pMotor->getCurPosition(1)/1000);
              vAlarmInfo.strReason = "x:" + x_pos + "y:" + y_pos + "当前热像仪检测到最高温度为" + str + "度";
              vAlarmInfo.strValue = str;
              vAlarmInfo.strItemName = ALARM_THERMAL_TEMP_HIGH;
              m_pWarningService->insertAlarm(vAlarmInfo);
          }
              break;
        }
        case THERMAL_LOW:
        {
          if(!m_pThermalImager->detectDevice()){
            string str = float_to_string(m_pHik->getMinTemp());
            string x_pos = float_to_string(m_pMotor->getCurPosition(0)/1000);
            string y_pos = float_to_string(m_pMotor->getCurPosition(1)/1000);
            vAlarmInfo.strReason = "x:" + x_pos + "y:" + y_pos + "当前热像仪检测到最低温度为" + str + "度";
            vAlarmInfo.strValue = str;
            vAlarmInfo.strItemName = ALARM_THERMAL_TEMP_LOW;
            m_pWarningService->insertAlarm(vAlarmInfo);
          }
            break;
        }
        default:
            break;
      }
    }
  }
  
  for(int warnNum=0; warnNum<TOTAL_WARN_NUM; warnNum++) {
      if(warnFlag[warnNum] == true) {
          warnFlag[warnNum] = false;
          count[warnNum]++;
          if(count[warnNum] < 10) {           //统计连续10次异常后判断一次
			  continue;
          }
          count[warnNum] = 0;
          switch(warnNum) {
              case ILLUMINANCE:
              {
                //   vAlarmInfo.strReason = "当前光照强度为" + std::to_string(m_pSensorDeviceDriver->getEnvInfo()->fIlluminance);
                  vAlarmInfo.strReason = "当前光照强度为" + float_to_string(m_pSensorBoardEx->getEnvInfo().fIlluminance)+"lx";
                  vAlarmInfo.strValue = float_to_string(m_pSensorBoardEx->getEnvInfo().fIlluminance);
                  vAlarmInfo.strItemName = ALARM_ILLUMINANCE_HIGH;
                  checkStatus[warnNum]=1;
                  break;
              }
              case BATTERY_TEMP:
              {
                  vAlarmInfo.strReason = "当前电池温度为" + float_to_string(m_pBatteryDriver->getTemperature()) + "度";
                  vAlarmInfo.strValue = float_to_string(m_pBatteryDriver->getTemperature());
                  vAlarmInfo.strItemName = ALARM_BATTERY_TEMP_HIGH;
                  checkStatus[warnNum]=1;
				  break;
              }
              case BATTERY_QUANTITY:
              {
                  vAlarmInfo.strReason = "当前电池电量为" + float_to_string(m_pBatteryDriver->getQuantity()) + "%";
                  vAlarmInfo.strValue = float_to_string(m_pBatteryDriver->getQuantity());
                  vAlarmInfo.strItemName = ALARM_BATTERY_QUANTITY_LOW;
                  checkStatus[warnNum]=1;
                  break;
              }
              case CH4:
              {
                //   vAlarmInfo.strReason = "当前甲烷为" + std::to_string(m_pSensorDeviceDriver->getGasInfo()->fCH4);
                  vAlarmInfo.strReason = "当前甲烷为" + float_to_string(m_pSensorBoardEx->getGasInfo().fCH4)+"ppm";
                  vAlarmInfo.strValue = float_to_string(m_pSensorBoardEx->getGasInfo().fCH4);
                  vAlarmInfo.strItemName = ALARM_CH4_HIGH;
				  checkStatus[warnNum]=1;
                  break;
              }
              case CO:
              {
                //   vAlarmInfo.strReason = "当前一氧化碳为" + std::to_string(m_pSensorDeviceDriver->getGasInfo()->fCO);
                  vAlarmInfo.strReason = "当前一氧化碳为" + float_to_string(m_pSensorBoardEx->getGasInfo().fCO)+"ppm";
                  vAlarmInfo.strValue = float_to_string(m_pSensorBoardEx->getGasInfo().fCO);
                  vAlarmInfo.strItemName = ALARM_CO_HIGH;
				  checkStatus[warnNum]=1;
                  break;
              }
              case SO2:
              {
                //   vAlarmInfo.strReason = "当前二氧化硫为" + std::to_string(m_pSensorDeviceDriver->getGasInfo()->fSO2);
                  vAlarmInfo.strReason = "当前二氧化硫为" + float_to_string(m_pSensorBoardEx->getGasInfo().fSO2)+"ppm";
                  vAlarmInfo.strValue = float_to_string(m_pSensorBoardEx->getGasInfo().fSO2);
                  vAlarmInfo.strItemName = ALARM_SO2_HIGH;
				  checkStatus[warnNum]=1;
                  break;
              }
              case O2:
              {
                //   vAlarmInfo.strReason = "当前氧气为" + std::to_string(m_pSensorDeviceDriver->getGasInfo()->fO2);
                  vAlarmInfo.strReason = "当前氧气为" + float_to_string(m_pSensorBoardEx->getGasInfo().fO2)+"%";
                  vAlarmInfo.strValue = float_to_string(m_pSensorBoardEx->getGasInfo().fO2);
                  vAlarmInfo.strItemName = ALARM_O2_LOW;
				  checkStatus[warnNum]=1;
                  break;
              }
              case SMOG:
              {
                  vAlarmInfo.strReason = "烟雾浓度过高";
                  vAlarmInfo.strItemName = ALARM_SMOG_HIGH;
				  checkStatus[warnNum]=1;
                  break;
              }
              case ULTRA_FRONT:
              {
                  vAlarmInfo.strReason = "前超声波有障碍物";
                  vAlarmInfo.strItemName = ALARM_USS;
                  checkStatus[warnNum]=1;
                  break;
              }
              case ULTRA_BACK:
              {
                  vAlarmInfo.strReason = "后超声波有障碍物";
                  vAlarmInfo.strItemName = ALARM_USS;
                  checkStatus[warnNum]=1;
                  break;
              }
              case MOTOR_DISCONNECT:
              {
                  // vAlarmInfo.strReason = m_pMotor->getMotorInfo().strErrMsg;
				  char str[100] = {};
				  sprintf(str, "电机错误码为:%d", getMotorErrcode());
				  vAlarmInfo.strReason = std::string(str);
                  vAlarmInfo.strItemName = ALARM_MOTOR;
				  checkStatus[warnNum]=1;
                  break;
              }
              case ROS_DISCONNECT:
              {
                  vAlarmInfo.strReason = "ROS连接失败";
                  vAlarmInfo.strItemName = ALARM_ROS_LOST;
				  checkStatus[warnNum]=1;
                  break;
              }
              case CAMERA_DISCONNECT:
              {
                  vAlarmInfo.strReason = "工业相机连接失败";
                  vAlarmInfo.strItemName = ALARM_CAMERAM_LOST;
				  checkStatus[warnNum]=1;
                  break;
              }
              case THERMAL_DISCONNECT:
              {
                  vAlarmInfo.strReason = "热像仪连接失败";
                  vAlarmInfo.strItemName = ALARM_THERMEL_LOST;
				  checkStatus[warnNum]=1;
                  break;
              }
              default:
                  break;
          }
          if( needPositionCheck[warnNum] ) {                    //更改位置后告警
              float pos = m_pWarningService->queryAlarmNearbydis(vAlarmInfo);       //查找最近时间的同类型告警位置信息
              if( abs(pos - vAlarmInfo.dPosition) > ALARM_POSITION_DELTA && 
                  vAlarmInfo.dPosition > 0) {                    //1m范围内不重复告警
                  lastWarnTime[warnNum] = curTime;              //告警后更新时间
                  m_pWarningService->insertAlarm(vAlarmInfo);
                  continue;
              }
          }
          if( abs(curTime - lastWarnTime[warnNum]) > MIN_ALARM_INTERVEL ) {    //5min内不重复告警
              lastWarnTime[warnNum] = curTime;                  //告警后更新时间
              m_pWarningService->insertAlarm(vAlarmInfo);
              continue;
          }
      }
      else {
          checkStatus[warnNum]=0;
          count[warnNum] = 0;
      }
  }

   setCallPlcFlag(checkStatus);  
}

void WarningManager::addAlarm(AlarmInfo vAlarmInfo)
{
  //m_pWarningService->queryAlarmNearby(vAlarmInfo, ALARM_INTERVAL);
  float pos = m_pWarningService->queryAlarmNearbydis(vAlarmInfo);       //查找最近时间的同类型告警位置信息

  //当前位置和上次告警位置差距1m以上时再次告警
  if(abs(pos - vAlarmInfo.dPosition) > ALARM_POSITION_DELTA && vAlarmInfo.dPosition > 0)
  {
    m_pWarningService->insertAlarm(vAlarmInfo);
  }
}

std::string WarningManager::getTime()
{
  setenv("TZ","GMT-8",1);

  time_t timep;
  time(&timep);

  char tmp[128];
  strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));

  return tmp;
}

void WarningManager::run(WarningManager *pWarningManager)
{
  printf("--- WarningManager run Start...\n");
  std::string logname = LOG_TAG + std::string(__FUNCTION__);
  auto logger = initLogger(logname);
  LOG4CPLUS_INFO(logger, "--- WarningManager run Start...\n");
  while (pWarningManager->m_bThreadRun) {
    sleep(1);
    pWarningManager->judgeWarn();
  }
  printf("--- WarningManager run Over...\n");
  LOG4CPLUS_INFO(logger, "--- WarningManager run Over...\n");

}
//浮点型数保留两位小数
std::string WarningManager::float_to_string(const float &value)
{
  char *chCode;
  chCode = new char[20];
  sprintf(chCode,"%.2f",value);
  std::string str(chCode);
  delete[]chCode;
  return str;

}

//设置告警PLC的设置函数
void WarningManager::setCallPlcFlag(std::vector<int> statusN)
{
 //2020-10-26事件标志位
 eventPlcFlag={0};
 static CallPlcFlag lastcallFlag={0};
 static time_t lastWarntime[2]={0};
 callFlagresult={0};
 for(int i=0;i<14;i++)
 {
     if(1==statusN[CH4]||1==statusN[CO]||1==statusN[SO2]||1==statusN[O2]||1==statusN[SMOG])
     {
       plcCallType.gasFlag=true;
     }
     if(1==statusN[BATTERY_TEMP])
     {
       plcCallType.tempFlag=true;
     }
     if(1==statusN[MOTOR_DISCONNECT]||1==statusN[ROS_DISCONNECT]||1==statusN[CAMERA_DISCONNECT]||1==statusN[THERMAL_DISCONNECT])
     {
        plcCallType.breakdownFlag=true;
     }

     if(0==statusN[CH4]&&0==statusN[CO]&&0==statusN[SO2]&&0==statusN[O2]&&0==statusN[SMOG])
     {
       plcCallType.gasFlag=false;
     }
     if(0==statusN[BATTERY_TEMP])
     {
       plcCallType.tempFlag=false;
     }
     if(0==statusN[MOTOR_DISCONNECT]&&0==statusN[ROS_DISCONNECT]&&0==statusN[CAMERA_DISCONNECT]&&0==statusN[THERMAL_DISCONNECT])
     {
        plcCallType.breakdownFlag=false;
     }     
 }
//机器人故障告警
   if(lastcallFlag.breakdownFlag!=plcCallType.breakdownFlag)
   {
       if(plcCallType.breakdownFlag)
         callFlagresult.breakdownFlag=true;
       else
        callFlagresult.breakdownFlag=false;
       //2020-10-26事件标志位
       eventPlcFlag.eventBreakdownFlag=true;
   }
   //气体超标告警
   if(lastcallFlag.gasFlag!=plcCallType.gasFlag)
   {
       if(plcCallType.gasFlag)
        callFlagresult.gasFlag=true;
       else
        callFlagresult.gasFlag=false;
       //2020-10-26事件标志位
       eventPlcFlag.eventGasFlag=true;
   }
   //超温告警
   if(lastcallFlag.tempFlag!=plcCallType.tempFlag)
   {
      if(plcCallType.tempFlag)
        callFlagresult.tempFlag=true;
      else
        callFlagresult.tempFlag=false;
      //2020-10-26事件标志位
      eventPlcFlag.eventTempFlag=true;
   }
   time_t curTime=time(0);
   //图像识别
   //撒煤楼告警
   if(lastcallFlag.divulgeFlag!=imageRecFlagN.divulgeFlag)
   {
      if(imageRecFlagN.divulgeFlag)
        callFlagresult.divulgeFlag=true;
     lastWarntime[0]=curTime;
     eventPlcFlag.eventDivulgeFlag=true;
   }
   //大石块检测
   if(lastcallFlag.stoneFlag!=imageRecFlagN.stoneFlag)
   {
      if(imageRecFlagN.stoneFlag)
        callFlagresult.stoneFlag=true;
      lastWarntime[1]=curTime;
      eventPlcFlag.eventStoneFlag=true;
   }
   //图像识别消告警
   if(imageRecFlagN.divulgeFlag)
   {
     if(abs(curTime-lastWarntime[0])>8)
     {
        callFlagresult.divulgeFlag=false;
        eventPlcFlag.eventDivulgeFlag=true;
        imageRecFlagN.divulgeFlag=false;
        lastcallFlag.divulgeFlag=false;
       }
   }
   if(imageRecFlagN.stoneFlag)
   {
     if(abs(curTime-lastWarntime[1])>8)
     {
        callFlagresult.stoneFlag=false;
        eventPlcFlag.eventStoneFlag=true;
        imageRecFlagN.stoneFlag=false;
        lastcallFlag.stoneFlag=false;
     }
   }
   //保留上一次的值信息
   lastcallFlag.breakdownFlag=plcCallType.breakdownFlag;
   lastcallFlag.gasFlag=plcCallType.gasFlag;
   lastcallFlag.tempFlag=plcCallType.tempFlag;
   lastcallFlag.divulgeFlag=imageRecFlagN.divulgeFlag;
   lastcallFlag.stoneFlag=imageRecFlagN.stoneFlag;

}

//获取告警PLC的标志位
CallPlcFlag WarningManager::getCallPlcFlag()
{ 
    return callFlagresult;
}
void WarningManager::setImageRecFlag(struct ImageRecW imageRecFlag)
{   //图像识别 
    imageRecFlagN.stoneFlag=imageRecFlag.stoneFlag;
    imageRecFlagN.divulgeFlag=imageRecFlag.divulgeFlag;
        
}
EventPlcFlag WarningManager::getEventPlcFlag()
{
   return eventPlcFlag;
}


