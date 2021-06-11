#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include "Transaction/Robot.h"
#include "Transaction/MoveTaskManage/MoveTaskManage.h"
#include "Transaction/Motor.h"
#include "Driver/PowerDriver.h"
#include "Driver/MotorDriver.h"
#include "Driver/AudioDriver.h"
#include "Transaction/ChargePile.h"
#include "Transaction/ChargePileService.h"
#include "Common/HttpPost/CHttpPost.hpp"
#include "Driver/LowConsumeManage.h"
#include "Poco/Base64Encoder.h"
#include "Poco/Base64Decoder.h"
#include "Common/HIKClient/HIKClient.h"
#include "Common/TcpClient/TcpCltSession.h"
#include "Common/TcpClient/TcpClient.h"
#include <zmqpp/zmqpp.hpp>
#include "Transaction/MoveTaskManage/RFIDService.h"
#include "Driver/BusinessInformation.h"
#include "Common/IRAYClient/IRAYClient.h"

const std::string LOG_TAG("root.Robot::");

#define openfireDoorDis 200

#define EXECUTE_TASK_NONE       -1
#define EXECUTE_TASK_CANCEL     -2


#define RECHARGE_MONITOR_TIMEOUT        200  // 200*0.2s=40s
#define RECHARGE_MONITOR_OFFSET_MAX     15
#define RECHARGE_MONITOR_OFFSET_MIN     -RECHARGE_MONITOR_OFFSET_MAX

#define CHECK_HALL_TIMEOUT              10     //
#define CHECK_CURRENT_TIMEOUT           150   //0.2*100=30s

#define SLEEP_TIME    9000  //30*60*5

#define CHANGE_MODE_TIME_LIMIT 1800          //n秒无手动操作切换到任务模式

float g_fNearestChargingPilePos = -1.0;
pthread_mutex_t g_chargePileMutex;
bool ptz_status = true;

//析构函数
Robot::~Robot()
{

}

Robot *Robot::getInstance()
{
  return SingleTon<Robot>::GetInstance();
}

//初始化
int Robot::init()
{
  // log initialize
  std::string logname = LOG_TAG + std::string(__FUNCTION__);
  auto logger = initLogger(logname);
  printf("--- Robot::init start\n");
  LOG4CPLUS_INFO(logger, " start");

  pthread_mutex_init(&mutex,0);
  pthread_mutex_init(&g_chargePileMutex,0);

  int nRet = 0;
  m_nRobotID = 0;
  m_nPhotoIndex = 0;
  m_bReconnectCamera  = true;
  m_bReconnectThermel = true;
  m_taskList.clear();
  m_chargingPilesPos.clear();
  m_chargingPilesName.clear();
  m_saving = 0;

  //读取配置文件
  m_pConfigServer = ConfigServer::getInstance();
  nRet = m_pConfigServer->init();
  printf("--- Robot::init 0 ConfigServer->init: %d\n", nRet);
  LOG4CPLUS_INFO(logger, " 0 ConfigServer->init: " << nRet);

  std::string robotVersion = m_pConfigServer->getRobotVersion();
  m_pCapabilities = Capabilities::getInstance();
  if (robotVersion == "t7a") {              //t7a
      m_pCapabilities->setSupportPtzMotor(true);
      m_pCapabilities->setSupportArmMotor(true);
      m_pCapabilities->setSupportArmReverse(true);
      m_pCapabilities->setSupportPDDetect(true);
      m_pCapabilities->setSupportRadarManage(false);
      m_pCapabilities->setSupportBatteryCharge(true);
      m_pCapabilities->setSupportChargeControl(false);
      m_pCapabilities->setSupport2DCoordinate(true);
      m_pCapabilities->setSupportSeerBattery(true);
  }
  else {                                    //t3c
      m_pCapabilities->setSupportPtzMotor(true);
      m_pCapabilities->setSupportArmMotor(false);
      m_pCapabilities->setSupportArmReverse(false);
      m_pCapabilities->setSupportPDDetect(false);
      m_pCapabilities->setSupportBatteryCharge(true);
      m_pCapabilities->setSupportChargeControl(true);
      m_pCapabilities->setSupportRadarManage(true);
      m_pCapabilities->setSupportRadarBelow(false);
      m_pCapabilities->setSupport2DCoordinate(false);
      std::string radar = m_pConfigServer->getRadarInfo()->sRadarHardware;
      if (radar == "T30UX") {
          m_pCapabilities->setSupportRadarType(RADAR_T30UX);
      }
      else if (radar == "KS104") {
          m_pCapabilities->setSupportRadarType(RADAR_KS104);
      }
      if (robotVersion == "t3s01" || robotVersion == "t5e") {        //t3s01
          m_pCapabilities->setSupportPDDetect(true);
          m_pCapabilities->setSupportArmMotor(true);
          m_pCapabilities->setSupportRadarBelow(true);
      }
      if (robotVersion == "t5a") {          //t5a
          m_pCapabilities->setSupportPDDetect(true);
          m_pCapabilities->setSupportArmMotor(true);
          m_pCapabilities->setSupportBatteryCharge(false);
          m_pCapabilities->setSupportChargeControl(false);
      }
      if (robotVersion == "t3e") {
          m_pCapabilities->setSupportPtzMotor(false);
      }
  }

  m_nRobotID = m_pConfigServer->getRobotId();
  m_sPhotoRootPath = m_pConfigServer->getPhotoRootPath();
  m_sVideoRootPath = m_pConfigServer->getVideoRootPath();
  
  //初始化数据库
  m_pMySQLServer = MySQLServer::getInstance();
  m_pMySQLServer->m_nRobotID = m_nRobotID;
  const MySQLInfo *pMySQLInfo = m_pConfigServer->getMySQLInfo();
  printf("__FILE__ =%s, __LINE__=%d\n",__FILE__,__LINE__);
  nRet = m_pMySQLServer->init(pMySQLInfo->sIp, pMySQLInfo->sPort, pMySQLInfo->sDataBase,
                              pMySQLInfo->sUser, pMySQLInfo->sPasswd);
  printf("--- Robot::init 1 MySQLDriver->init:%d\n", nRet);
  LOG4CPLUS_INFO(logger, " 1 MySQLDriver->init: " << nRet);
  if (nRet != 0) {
      return -1;
  }

  //获取数据库配置参数
  m_pSettingService = SettingService::getInstance();
  m_pSettingService->init();
  m_pSettingService->querySettingByRobotId(m_nRobotID);
  printf("__FILE__ =%s, __LINE__=%d\n",__FILE__,__LINE__);

  //IRAY
#if 1
  //printf("++++ IRAYClient start ++++\n");
        IRAYClient * client = IRAYClient::getInstance();
  //printf("++++ IP: %s \n Port: %d \n User: %s \n Pwd: %s ++++\n",
 //       m_pSettingService->m_setting.getThermalImagerIp().c_str(),
 //       m_pSettingService->m_setting.getThermalImagerPort(),
 //       m_pSettingService->m_setting.getCameraUserName(),
 //       m_pSettingService->m_setting.getCameraPassword());

        if(client) {
                client->client_init(m_pSettingService->m_setting.getThermalImagerIp().c_str(), "3000", "888888", "888888");
/*                 client->client_get_temp_raw_dat();
                std::vector<float> a = {0.5,0.5,0.6,0.6}; 
                client->get_local_temp(a); */
        }
  //printf("++++ IRAYClient end   ++++\n");
#endif

  //tcp初始化
  TcpCltSession* cltSession = TcpCltSession::getInstance();

  char* slaveIp = (char*)m_pConfigServer->getTcpIpInfo()->sMainIp.c_str();
  int slavePort = m_pConfigServer->getTcpIpInfo()->nMainPort;
  cltSession->createSession(TCP_MAIN_BOARD, slaveIp, slavePort);
  
  slaveIp = (char*)m_pConfigServer->getTcpIpInfo()->sPTZIp.c_str();
  slavePort = m_pConfigServer->getTcpIpInfo()->nPTZPort;
  cltSession->createSession(TCP_PTZ_BOARD, slaveIp, slavePort);

  slaveIp = (char*)m_pConfigServer->getTcpIpInfo()->sEnvIp.c_str();
  slavePort = m_pConfigServer->getTcpIpInfo()->nEnvPort;
  cltSession->createSession(TCP_ENV_BOARD, slaveIp, slavePort);

  //Tcp Switch
  std::string strResult = m_pMySQLServer->getSettingParam(driver_msg_switch, m_nRobotID);
  //printf("++++ __FILE__ =%s, __LINE__=%d, Get TCP MSG switch from DB: %s ++++\n", __FILE__, __LINE__, strResult);
  bool switch_on = atoi(strResult.c_str());
  //printf("++++ __FILE__ =%s, __LINE__=%d, Get TCP MSG switch from DB: %d ++++\n", __FILE__, __LINE__, switch_on);
  SettingService::getInstance()->m_setting.setDriverMsgSwitch(switch_on);

  //相机初始化
  printf("__FILE__ =%s, __LINE__=%d\n",__FILE__,__LINE__);
  m_pIndustrialCamera = IndustrialCamera::getInstance();
  m_pIndustrialCamera->init();
  nRet = m_pIndustrialCamera->connect();
  if (nRet == 0) { m_bReconnectCamera  = false; }
  printf("--- Robot::init 4 IndustrialCamera->connect:%d\n", nRet);
  LOG4CPLUS_INFO(logger, " 4 IndustrialCamera->connect: " << nRet);
            
  //热像仪初始化
  m_pThermalImager = ThermalImager::getInstance();
  m_pThermalImager->init();
  nRet = m_pThermalImager->connect();
  if (nRet == 0) { m_bReconnectThermel = false; }
  printf("--- Robot::init 5 ThermalImager->connect:%d\n", nRet);
  LOG4CPLUS_INFO(logger, " 5 ThermalImager->connect: " << nRet);

  //云台电机初始化
  m_pPTZDriver = PTZDriver::getInstance();
  m_pPTZDriver->init();

  //获取robot绑定地图Id
  uint32_t mapId = MapService::queryMapIdbyRobotId(m_nRobotID);
  if (mapId > 0) {
      printf("--- Robot::init, bind map id: %d\n", mapId);
      LOG4CPLUS_INFO(logger, " bind map id: " << mapId);
      m_pRobotCurMap = MapService::queryMapbyId(mapId);
      if (m_pRobotCurMap != NULL) {
          RobotMap::Header mh = m_pRobotCurMap->mapHeader();
          printf("--- Robot::init, get map success, mapId: %d, map name: %s, min pos: %d, max pos: %d\n", 
                  mh.id, mh.name.c_str(), mh.minPosition, mh.maxPosition);
          LOG4CPLUS_INFO(logger, " get map success, map id: " << mh.id << 
                                 ", map name: " << mh.name.c_str() <<
                                 ", min pos: " << mh.minPosition << 
                                 ", max pos: " << mh.maxPosition);
      }
  }

  //数据库获取当前所有充电桩位置
  chargePileService = ChargePileService::getInstance(); 
  chargePileService->init();
  if (m_pRobotCurMap != NULL) {
      std::vector<ChargePile> vChargePile;
      chargePileService->queryByMapId(m_pRobotCurMap->mapHeader().id, vChargePile);
      std::map<int, char*> chargePilesMap;
      chargePilesMap.clear();
      
      printf("%s(), get chargepile position: ", __FUNCTION__);
      for(int i = 0; i < vChargePile.size(); ++i)
      {
        printf("%d ", vChargePile[i].getPosition());
        LOG4CPLUS_INFO(logger, " get chargepile position: " << vChargePile[i].getPosition());
        m_chargingPilesPos.push_back(vChargePile[i].getPosition());

        std::cout << "Charge pile name: "<< vChargePile[i].getInstanceName() << endl;
        m_chargingPilesName.push_back(vChargePile[i].getInstanceName());
        
        chargePilesMap.insert(std::pair<int, char*>(vChargePile[i].getPosition(), const_cast<char*>(vChargePile[i].getIp().c_str())));
      }
      printf("\n");

      //充电桩位置、状态检测初始化
      m_pChargePileDriver = ChargePileDriver::getInstance();
      m_pChargePileDriver->init(chargePilesMap);
  }

  m_pTaskService = TaskService::getInstance();
  m_pTaskService->init();

  //运动管理初始化
  m_pMoveTaskManage = MoveTaskManage::getInstance();
  m_pMoveTaskManage->init();

  //告警初始化
  m_pWarningManager = WarningManager::getInstance();
  m_pWarningManager->init(m_nRobotID);
  printf("__FILE__ =%s, __LINE__=%d\n",__FILE__,__LINE__);

  //运动初始化
  m_pMotor = Motor::getInstance();
  m_pMotor->init();
  printf("__FILE__ =%s, __LINE__=%d\n",__FILE__,__LINE__);

/*   if (m_pCapabilities->isSupportPDDetect()) {
    // 局放运动初始化
    m_pPDD = PDDMotorDriver::getInstance();
    printf("__FILE__ =%s, __LINE__=%d\n",__FILE__,__LINE__);
    // m_pPDD->init(); 
    printf("__FILE__ =%s, __LINE__=%d\n",__FILE__,__LINE__);
    // 局放初始化
    m_pDec = PartialDischargeDetectorDriver::getInstance();
    printf("__FILE__ =%s, __LINE__=%d\n",__FILE__,__LINE__);
    // m_pDec->init(); 
    printf("__FILE__ =%s, __LINE__=%d\n",__FILE__,__LINE__);
  } */

  // 超声波初始化
  if (m_pCapabilities->isSupportRadarManage()) {
      m_pRadarManager = RadarManager::getInstance();
      m_pRadarManager->addRadar(RADAR_FRONT);
      m_pRadarManager->addRadar(RADAR_BACK);
      if (m_pCapabilities->isSupportRadarBelow()) {
          m_pRadarManager->addRadar(RADAR_DOWN);
      }
      m_pRadarManager->init();
      printf("__FILE__ =%s, __LINE__=%d\n",__FILE__,__LINE__);
  }

  //音频驱动初始化
  AudioDriver *pAudioDriver = AudioDriver::getInstance();
  nRet = pAudioDriver->init();
  if (nRet < 0) {
      printf("--- Robot::init 3 pAudioDriver->init failed! nRet: %d\n", nRet);
  }

/*   //防火门初始化
  printf("__FILE__ =%s, __LINE__=%d\n",__FILE__,__LINE__);
  m_pFireDoorDriver = FireDoorDriver::getInstance();
  m_pFireDoorDriver->init(m_nRobotID); */

  //任务列表更新
  tasksRefresh();

  //霍尔传感器初始化
  m_pHallDriver = HallDriver::getInstance();
  m_pHallDriver->init();

  //电池
  m_pBatteryDriver = BatteryDriver::getInstance();
  m_pBatteryDriver->init();
  const SerialDeviceInfo *serialDeviceInfo = m_pConfigServer->getSerialDeviceInfo();
  //传感器驱动初始化
  m_pSensorBoardEx = SensorBoardEx::getInstance();
  m_pSensorBoardEx->init(serialDeviceInfo->sSensorDevice.c_str(), 
                        serialDeviceInfo->nSensorBaud, 
                        serialDeviceInfo->nDataBit, 
                        serialDeviceInfo->nParity, 
                        serialDeviceInfo->nStopBit,
                        m_pConfigServer->getSensorBoardVer());

  //创建低电量检测线程
  if (m_pCapabilities->isSupportBatteryCharge()) {
      std::thread runThread;
      runThread = std::thread(Robot::lowBatteryDetectThread, this);
      runThread.detach();
  }

  //图像处理模块
  m_pImageProcessManager = ImageProcessManager::getInstance();
  m_pImageProcessManager->init();

  //任务轮询
  std::thread rotationThread;
  rotationThread = std::thread(Robot::taskRotation, this);
  rotationThread.detach();

  //30分钟无操作切换到任务模式检测
  std::thread noOperationDetectThread;
  noOperationDetectThread = std::thread(Robot::noOperationDetectThread, this);
  noOperationDetectThread.detach();

/*   if (m_pCapabilities->isSupportArmMotor()) {
      //升降电机到位检测
      std::thread armStandStillDetectThread;
      armStandStillDetectThread = std::thread(Robot::armStandStillDetectThread, this);
      armStandStillDetectThread.detach();
  }  */

  std::thread motorCurrentProtectThread;
  motorCurrentProtectThread = std::thread(Robot::motorCurrentProtectThread, this);
  motorCurrentProtectThread.detach();

  if (m_pCapabilities->isSupportChargeControl()) {
    //电池充电完防掉电
    std::thread antiBatDischargeDetectThread;
    antiBatDischargeDetectThread = std::thread(Robot::antiBatDischargeDetectThread, this);
    antiBatDischargeDetectThread.detach();
  }

  //省电模式
  std::thread powerSavingThread;
  powerSavingThread = std::thread(Robot::powerSavingThread, this);
  powerSavingThread.detach();
  
  HIKClient *pClient = HIKClient::getInstance();
  Setting setting = m_pSettingService->m_setting;
  char port[32] = "";
  snprintf(port, sizeof(port), "%d", setting.getThermalImagerPort());
  pClient->init(setting.getThermalImagerIp().c_str(), port,
      setting.getCameraUserName().c_str(),
      setting.getCameraPassword().c_str());
      
  if (ConfigServer::getInstance()->isNewHope()) {
      pClient->pixelParamConfig();

      std::thread heartBeatThread;
      heartBeatThread = std::thread(Robot::heartBeatThread, this);
      heartBeatThread.detach();
  }

  m_pZmqDriver = ZMQDriver::getInstance();
  m_pZmqDriver->registerCallBack(Robot::responseCommand);
  m_pZmqDriver->initZMQ();

  int cnt = 0;
  InspectStatus initStatus = {0,0,0,0,0};
  m_pBusinessInfor = BusinessInfor::getInstance();
  m_pBusinessInfor->init();

  //开机升降臂homing
  if(m_pCapabilities->isSupportArmMotor() && m_pCapabilities->isSupportPtzMotor() && m_pSettingService->m_setting.getHomeSwitch()) {
      m_pMoveTaskManage->createPtzArmHomeTaskItem();
  }

  //云台心跳
/*   if(m_pCapabilities->isSupportPtzMotor()) {
      std::thread ptzheartBeatThread;
      ptzheartBeatThread = std::thread(Robot::ptzheartBeatThread, this);
      ptzheartBeatThread.detach();
  } */
/*   if(0 != m_pBusinessInfor->getInspectStatus(initStatus)) {
      if(cnt < 3) {
          printf("--- Robot::init failed, can not connect slave board, try again ---\n");
          LOG4CPLUS_INFO(logger, " --- Robot::init failed, can not connect slave board, try again ---\n " );
          sleep(1);
      }
      else {
          cnt = 0;
          printf("--- Robot::init failed, can not connect slave board, aborted ---\n");
          LOG4CPLUS_INFO(logger, " --- Robot::init failed, can not connect slave board, aborted ---\n " );
          return -1;
      }
  } */
  //todo 初始化异常处理 

  printf("--- Robot::init End\n");
  LOG4CPLUS_INFO(logger, " End " );

  return 0;
}

void Robot::heartBeatThread(Robot* pRobot)
{
    std::string logname = LOG_TAG + std::string(__FUNCTION__);
    auto logger = initLogger(logname);
    printf("--- Robot::%s() start\n", __FUNCTION__);
    LOG4CPLUS_INFO(logger, " start ");
    while(1) 
    {
        int pos = (int)pRobot->m_pMotor->getCurPosition();
        CHttpPost::sendHeartBeat(pRobot->m_nRobotID, std::to_string(pos));
        sleep(300);
    }
    
    printf("--- Robot::%s() end\n", __FUNCTION__);
    LOG4CPLUS_INFO(logger, " end ");
}

void Robot::ptzheartBeatThread(Robot* pRobot)
{
    std::string logname = LOG_TAG + std::string(__FUNCTION__);
    auto logger = initLogger(logname);
    printf("--- Robot::%s() start\n", __FUNCTION__);
    LOG4CPLUS_INFO(logger, " start ");
    PTZDriver::Ptr pPtzMotor = PTZDriver::getInstance();
    std::vector<PTZMotorStat> ptzState;
    while(1) 
    {
        pPtzMotor->getPTZStatus(ptzState);
        ptzState.clear();
        sleep(5);
    }
    
    printf("--- Robot::%s() end\n", __FUNCTION__);
    LOG4CPLUS_INFO(logger, " end ");
}

void Robot::lowBatteryDetectThread(Robot* pRobot)
{
    std::string logname = LOG_TAG + std::string(__FUNCTION__);
    auto logger = initLogger(logname);
    printf("--- Robot::%s() start\n", __FUNCTION__);
    LOG4CPLUS_INFO(logger, " start ");
    static int timeCnt = 0;
    float batQuantity = 0, batCurr = 0;
    while (1)
    {
        sleep(10);
        SettingService *settingService = SettingService::getInstance();
        int quantityMinLimit = settingService->m_setting.getBatteryQuantityMin();

        batQuantity = pRobot->m_pBatteryDriver->getQuantity();
        batCurr = pRobot->m_pBatteryDriver->getCurrent();
        if (!pRobot->m_pCapabilities->isSupportSeerBattery()){
            if((batQuantity <= quantityMinLimit) && (batQuantity > 0) && (batCurr <= 0.5)) {
                pRobot->m_pMoveTaskManage->lowBatteryCharge();
            }
        }
        else{
            if((batQuantity <= quantityMinLimit) && (batQuantity > 0) && !pRobot->m_pBatteryDriver->isCharge()) {
                pRobot->m_pMoveTaskManage->lowBatteryCharge();
            }
        }

        /* if (timeCnt <= 60) {
            timeCnt++;
            continue;
        }
        if (batQuantity <= 20 && batQuantity > 0) {
            printf("--- Robot::%s(), low battery %f%%! power off!\n", __FUNCTION__, batQuantity);
            LOG4CPLUS_INFO(logger, " low battery " << batQuantity << "%! power off!");
            if(pRobot->m_pCapabilities->isSupport2DCoordinate())
            {
              // pRobot->m_pMotor->poweroff();
            }
            else{
              pRobot->m_pPowerDriver->switchPowerBattery(0);
            }
        } */
    }
    printf("--- Robot::%s() end\n", __FUNCTION__);
    LOG4CPLUS_INFO(logger, " end ");

}

void Robot::noOperationDetectThread(Robot* pRobot)
{
    std::string logname = LOG_TAG + std::string(__FUNCTION__);
    auto logger = initLogger(logname);
    int timeCnt = 0;
    Setting *pSetting = &pRobot->m_pSettingService->m_setting;
    printf("--- Robot::%s(), start\n", __FUNCTION__);
    LOG4CPLUS_INFO(logger, " start ");

    while(1) 
    {
        if(pSetting->getRunMode() == REMOTE_CONTROL_MODE) {         //只在远程控制模式有检测
            timeCnt++;
            if(pRobot->m_pMoveTaskManage->getManualItemSize()) {    //用户有操作，不切模式
                timeCnt = 0;
            }
            if(timeCnt > CHANGE_MODE_TIME_LIMIT) {      //连续一段时间没有操作，切到任务模式
                printf("Robot::%s(), no operation, change to task mode!\n", __FUNCTION__);
                LOG4CPLUS_INFO(logger, " no operation, change to task mode! ");

                char strValue[20] = "";
                snprintf(strValue, sizeof(strValue), "%d", TASK_MODE);
                int nRet = pRobot->m_pMySQLServer->setSettingParam(run_mode, strValue, pRobot->m_nRobotID);
                if( nRet == true ) {
                    pSetting->setRunMode(TASK_MODE);
                    printf("Robot::%s(), run mode change success\n", __FUNCTION__);
                    LOG4CPLUS_INFO(logger, " run mode change success ");

                }
            }
        }
        else {
            timeCnt = 0;
        }
        sleep(1);
    }
    printf("--- Robot::%s(), end\n", __FUNCTION__);
    LOG4CPLUS_INFO(logger, " end ");

}

void Robot::armStandStillDetectThread(Robot *pRobot)
{
    sleep(60); //开机homing 60s
    std::string logname = LOG_TAG + std::string(__FUNCTION__);
    auto logger = initLogger(logname);
    printf("--- Robot::%s(), start\n", __FUNCTION__);
    LOG4CPLUS_INFO(logger, " start ");
    int timeCnt = 0;
    int Cnt = 0;
    int MaxCnt = pRobot->m_pConfigServer->getTimeInfo()->ArmDisable;

    while(1){
      if(true == pRobot->m_pMotor->isInState(LIFT_MOTOR,STOP_STATE) && NULL == pRobot->m_pMoveTaskManage->getCurTaskItem())
      {
        ++Cnt;
        if((Cnt >= MaxCnt) && (0 == timeCnt))
        {
          int nRet = pRobot->m_pMotor->armDisable();
          ++timeCnt;
          if(nRet == 0){
            printf("Robot::%s(), arm disable success\n", __FUNCTION__);
            LOG4CPLUS_INFO(logger, " arm disable success ");
          }
        }
      }
      else{
        timeCnt = 0;
        Cnt = 0;
      }
      sleep(1);
    }
}

//静止时电机电流持续较大时处理
void Robot::motorCurrentProtectThread(Robot *pRobot)
{
    std::string logname = LOG_TAG + std::string(__FUNCTION__);
    auto logger = initLogger(logname);

    const float OVER_CURRENT = 2000; 
    const float MM_PER_CNT = 1;
    int timeCnt = 0;
    printf("--- Robot::%s(), start\n", __FUNCTION__);
    LOG4CPLUS_INFO(logger, " start");
    while(1) {
        float current = pRobot->m_pMotor->getCurrent();
        if ((pRobot->m_pMotor->getCurStatus() == STOP_STATE) &&
            (fabs(current) > OVER_CURRENT) &&
            (pRobot->m_pMoveTaskManage->getCurTaskItem() == NULL)) {
            usleep(100000);
            if (pRobot->m_pMoveTaskManage->getCurTaskItem() == NULL) {
                timeCnt++;
                if (timeCnt > 60) {
                    float curPos = pRobot->m_pMotor->getCurPosition();
                    int dirt = ((current > 0) ? -1 : 1);
                    float offtPos = curPos + dirt * MM_PER_CNT;
                    printf("--- Robot::%s(), current: %f, now pos: %f, moving to %f\n", __FUNCTION__, current, curPos, offtPos);
                    LOG4CPLUS_INFO(logger, " current" << current << ", now pos " << curPos << "moving to " << offtPos);
                    pRobot->m_pMotor->goTo(offtPos);
                    timeCnt = 0;
                }
            }
        }
        else {
            timeCnt = 0;
        }
        sleep(1);
    }
}

//10分钟检查一次
void Robot::antiBatDischargeDetectThread(Robot *pRobot)
{
  while(1)
  {
    if(true == pRobot->m_pHallDriver->getHall() && STOP_STATE == pRobot->m_pMotor->getCurStatus() && pRobot->m_pBatteryDriver->getCurrent() < 0.5 && pRobot->m_pBatteryDriver->getQuantity() < 80
      && pRobot->m_pMoveTaskManage->getCurTaskItem() == NULL)
    {
      pRobot->m_pMoveTaskManage->manulCharge();
    }
    sleep(pRobot->m_pConfigServer->getTimeInfo()->Recharge);
  }
}

 

//省电模式
void Robot::powerSavingThread(Robot *pRobot)
{
  std::string curDate = pRobot->getDate();
  static std::string lastDate=curDate;
  Setting *pSetting = &pRobot->m_pSettingService->m_setting;
  //sleep(60);
  
  while(1)
  {
    #if 0
    if(pSetting->getRunMode() == TASK_MODE && pRobot->m_pMoveTaskManage->getCurTaskItem() == NULL && pRobot->m_saving == 0 && pRobot->m_pSettingService->m_setting.getPtzSleepSwitch() && ptz_status && pRobot->m_pSettingService->m_setting.getPtzPowerSwitch())
    {
      pRobot->m_pPowerDriver->switchPowerPtz(false);
      ptz_status = false;
      std::cout << "云台关闭" << std::endl;
    }
    else if ((pSetting->getRunMode() == REMOTE_CONTROL_MODE || pRobot->m_saving != 0 || pRobot->m_pMoveTaskManage->getCurTaskItem()) && !ptz_status && pRobot->m_pSettingService->m_setting.getPtzPowerSwitch())
    {
      pRobot->m_pPowerDriver->switchPowerPtz(true);
      ptz_status = true;
      std::cout << "云台开启" << std::endl;
    }
    #endif
    /*凌晨0点onvif重启可见光相机*/
     curDate = pRobot->getDate();
     
    if(curDate != lastDate)
    {
		pRobot->m_pIndustrialCamera->rebootDevice();
		lastDate = curDate;
    }
    sleep(1);
  }
}

void Robot::responseCommand(std::string sReq, std::string &sRes)
{
    auto logger = initLogger(LOG_TAG + std::string(__FUNCTION__));
    int nRet = 0;
    Robot *pRobot = Robot::getInstance();
    COMParse *pParse = COMParse::getInstance();

    COMPkg::Control::Request ctrlReq;
    COMPkg::Control::Response ctrlRes;

    COMPkg::Setting::Request settingReq;
    COMPkg::Setting::Response settingRes;

    COMPkg::SyncDown::Request syncDownReq;
    COMPkg::SyncDown::Response syncDownRes;

    std::string sFeedback = "";
    std::string transId;
    std::vector<int> status(3,0);
    int type = 0;
    std::string sReqName = "";

    transId = pParse->getCommandTransId(sReq);
    type = pParse->getCommandType(sReq);
    sReqName = pParse->getReqName(sReq);
    printf("Robot::%s(), receive command: %s, type: %d, sTransId: %s\n", __FUNCTION__, sReq.c_str(), type, transId.c_str());
    LOG4CPLUS_INFO(logger, " receive command: " << sReq.c_str() << ", type: " << type << "sTransId: " << transId.c_str());

    if(sReqName != "") {
        UpGrade(pRobot, sReqName, sReq, sRes);
    }
    pRobot->m_pMoveTaskManage->setVideoflag(0);
    switch (type) {
        case CONTROL_TYPE:
            nRet = pParse->parseControlCommand(sReq, ctrlReq);
            if (nRet == 0) {
                pRobot->responseControl(ctrlReq, ctrlRes);
                nRet = ctrlRes.nErrcode;
                sFeedback = ctrlRes.sFeedback;
            }
            break;
        case SETTING_TYPE:
            nRet = pParse->parseSettingCommand(sReq, settingReq);
            if (nRet == 0) {
                pRobot->responseSetting(settingReq, settingRes);
                nRet = settingRes.nErrcode;
                sFeedback = settingRes.sFeedback;
            }
            break;
        case SYNCDOWN_TYPE:
            nRet = pParse->parseSyncDownCommand(sReq, syncDownReq);
            if (nRet == 0) {
                pRobot->responseTasksSyncDown(syncDownReq, syncDownRes);
                nRet = syncDownRes.nErrcode;
            }
            break;
        case PTZ_CONTROL_TYPE:
            nRet = pParse->parseControlCommand(sReq, ctrlReq);
            if (nRet == 0) {
                pRobot->responsePTZControl(ctrlReq, ctrlRes);
                nRet = ctrlRes.nErrcode;
                sFeedback = ctrlRes.sFeedback;
            }
            break;
        default:
            nRet = -1;
            break;
    }

    if(sReqName == "") {
        pParse->spliceCommandRes(transId, nRet, sFeedback, sRes);
    }
    printf("Robot::%s(), response command: %s\n", __FUNCTION__, sRes.c_str());
    LOG4CPLUS_INFO(logger, " response command: " << sRes.c_str());
}

void Robot::UpGrade(Robot *pRobot, std::string sReqName, std::string sReq, std::string& sRes) {
    TcpCltSession* cltSession = TcpCltSession::getInstance();
    UpGradeDriver* pUpGradeDriver = UpGradeDriver::getInstance();
    static int nTry = 0;
    if(sReqName == "reqMotionUpdate" || sReqName == "reqPTZUpdate" || sReqName == "reqSensorUpdate" || sReqName == "reqPMUUpdate") {
        if(nTry == 0) {
            nTry++;
            char* slaveIp = (char*)pRobot->m_pConfigServer->getTcpIpInfo()->sMainIp.c_str();
            int slavePort = pRobot->m_pConfigServer->getTcpIpInfo()->nMainPort;
            cltSession->createSession(TCP_MAIN_BOARD_UPGRADE, slaveIp, slavePort, 10);
            
            slaveIp = (char*)pRobot->m_pConfigServer->getTcpIpInfo()->sPTZIp.c_str();
            slavePort = pRobot->m_pConfigServer->getTcpIpInfo()->nPTZPort;
            cltSession->createSession(TCP_PTZ_BOARD_UPGRADE, slaveIp, slavePort, 10);

            slaveIp = (char*)pRobot->m_pConfigServer->getTcpIpInfo()->sEnvIp.c_str();
            slavePort = pRobot->m_pConfigServer->getTcpIpInfo()->nEnvPort;
            cltSession->createSession(TCP_ENV_BOARD_UPGRADE, slaveIp, slavePort, 10);

            //单片机板升级初始化
            pUpGradeDriver->init();

            cltSession->closeSession(TCP_MAIN_BOARD);
            cltSession->closeSession(TCP_PTZ_BOARD);
            cltSession->closeSession(TCP_ENV_BOARD);
            sleep(1); //等待发送结束
        }
        else{
           nTry++;
        }
        int nRet = pUpGradeDriver->request(sReqName, sReq, sRes);
        if(nRet != 0) {
            sRes = "-1";
        }
    }
    else if(sReqName == "quitMotionUpdate" || sReqName == "quitPTZUpdate" || sReqName == "quitSesorUpdate" || sReqName == "quitPMUUpdate") {
        int nRet = pUpGradeDriver->request(sReqName, sReq, sRes);
        if(nRet != 0) {
            sRes = "-1";
        }
    }
    else if(sReqName == "finishUpdate") {
          nTry = 0;
          cltSession->realcloseSession(TCP_MAIN_BOARD_UPGRADE);
          cltSession->realcloseSession(TCP_PTZ_BOARD_UPGRADE);
          cltSession->realcloseSession(TCP_ENV_BOARD_UPGRADE);
          sleep(1); //等待单片机启动
          cltSession->openSession(TCP_MAIN_BOARD);
          cltSession->openSession(TCP_PTZ_BOARD);
          cltSession->openSession(TCP_ENV_BOARD);
    }
    else {
        int nRet = pUpGradeDriver->request(sReqName, sReq, sRes);
        if(nRet != 0) {
            sRes = "-1";
        }
    }
}

bool Robot::responsePTZControl(COMPkg::Control::Request &control, COMPkg::Control::Response &res)
{
  int nRet = -1;
  int nCmd = 0;
  int nValue = 0;
  float fValue = 0.0;
  res.sFeedback = "";

  nCmd = control.nCommand;
  nValue = control.nValue;
  fValue = control.fValue;
  SettingService *settingService = SettingService::getInstance();
  printf("--- Robot:%s(), nCmd = %d, nValue = %d, fValue = %f\n", __FUNCTION__, nCmd, nValue, fValue);

  if(!m_pCapabilities->isSupportPtzMotor()) {
      Json::Value val;
      std::vector<int> status = {0, 0, 0, 0, 0};
      if (status.size() >= 4) {
          val["xPos"] = status[0];
          val["yPos"] = status[1];
          val["zPos"] = status[2];
          val["pddDis"] = status[3];
      }
      if (status.size() > 4) {
          val["zoom"] = status[4];
      }
      res.sFeedback = val.toStyledString();
      res.sFeedback.erase(std::remove_if(res.sFeedback.begin(), res.sFeedback.end(), 
                  [](char c) -> bool { return (c == '\n' || c == '\t' || c == '\r' || c == ' ');}), res.sFeedback.end());
      return 0;
  }

  switch(nCmd)
  {
	  case PTZ_COORD_CMD:
	  {
      std::vector<int> status;
      nRet = m_pPTZDriver->getPTZCoordinate(status);
      if(nRet != 0) {
          printf("--- Robot:%s(), read ptz coordinate failed\n",__FUNCTION__);
      }
      Json::Value val;
      if (status.size() >= 4) {
          val["xPos"] = status[0];
          val["yPos"] = status[1];
          val["zPos"] = status[2];
          val["pddDis"] = status[3];
      }
      if (status.size() > 4) {
          val["zoom"] = status[4];
      }
      res.sFeedback = val.toStyledString();
      res.sFeedback.erase(std::remove_if(res.sFeedback.begin(), res.sFeedback.end(), 
                  [](char c) -> bool { return (c == '\n' || c == '\t' || c == '\r' || c == ' '); }), res.sFeedback.end());
	    break;
	  }

    case VISUAL_CMD:
	  {
      if(m_pMotor->getArmCurPosition() <= settingService->m_setting.getArmBeginPos()) {
        nValue = VISUAL_STOP_CMD;
      }

      switch(nValue)
      {

        case VISUAL_STOP_CMD:
        {
          nRet = m_pPTZDriver->ctrlPTZ(CAM_MOTOR,TURN_STOP,0);
          printf("--- Robot:%s(), ctrlPTZ CAM_MOTOR STOP, nRet: %d\n",__FUNCTION__, nRet);
        }
        break;

        case VISUAL_UP_CMD:
        {
          nRet = m_pPTZDriver->ctrlPTZ(CAM_MOTOR,TURN_UP,0);
          printf("--- Robot:%s(), ctrlPTZ CAM_MOTOR TURN_UP, nRet: %d\n",__FUNCTION__, nRet);
        }
        break;

        case VISUAL_DOWN_CMD:
        {
          nRet = m_pPTZDriver->ctrlPTZ(CAM_MOTOR,TURN_DOWN,0);
          printf("--- Robot:%s(), ctrlPTZ CAM_MOTOR TURN_DOWN, nRet: %d\n",__FUNCTION__, nRet);
        }
        break;

        default:
        break;
      }
	  }
    break;

    case HORIZON_CMD:
	  {
      //小于软限位不允许旋转
      if(m_pMotor->getArmCurPosition() <= settingService->m_setting.getArmBeginPos()) {
        nValue = HORIZON_STOP_CMD;
      }

      switch(nValue)
      {
        case HORIZON_STOP_CMD:
        {
          nRet = m_pPTZDriver->ctrlPTZ(HORIZ_MOTOR,TURN_STOP,0);
          printf("--- Robot:%s(), ctrlPTZ HORIZ_MOTOR STOP, nRet: %d\n",__FUNCTION__, nRet);
        }
        break;

        case HORIZON_LEFT_CMD:
        {
          nRet = m_pPTZDriver->ctrlPTZ(HORIZ_MOTOR,TURN_UP,0);
          printf("--- Robot:%s(), ctrlPTZ HORIZ_MOTOR TURN_LEFT, nRet: %d\n",__FUNCTION__, nRet);
        }
        break;

        case HORIZON_RIGHT_CMD:
        {
          nRet = m_pPTZDriver->ctrlPTZ(HORIZ_MOTOR,TURN_DOWN,0);
          printf("--- Robot:%s(), ctrlPTZ HORIZ_MOTOR TURN_RIGHT, nRet: %d\n",__FUNCTION__, nRet);
        }
        break;

        default:
        break;
      }
	  }
    break;

    case INFRADE_CMD:
	  {
      if(m_pMotor->getArmCurPosition() <= settingService->m_setting.getArmBeginPos()) {
        nValue = INFRADE_STOP_CMD;
      }

      switch(nValue)
      {
        case INFRADE_STOP_CMD:
        {
          nRet = m_pPTZDriver->ctrlPTZ(INFRA_MOTOR,TURN_STOP,0);
          printf("--- Robot:%s(), ctrlPTZ INFRA_MOTOR STOP, nRet: %d\n",__FUNCTION__, nRet);
        }
        break;

        case INFRADE_UP_CMD:
        {
          nRet = m_pPTZDriver->ctrlPTZ(INFRA_MOTOR,TURN_DOWN,0);
          printf("--- Robot:%s(), ctrlPTZ INFRA_MOTOR TURN_UP, nRet: %d\n",__FUNCTION__, nRet);
        }
        break;

        case INFRADE_DOWN_CMD:
        {
          nRet = m_pPTZDriver->ctrlPTZ(INFRA_MOTOR,TURN_UP,0);
          printf("--- Robot:%s(), ctrlPTZ INFRA_MOTOR TURN_DOWN, nRet: %d\n",__FUNCTION__, nRet);
        }
        break;

        default:
        break;
      }
	  }
    break;

    case DISCHARGE_CMD:
	  {
      switch(nValue)
      {
        case DISCHARGE_STOP_CMD:
        {
          nRet = m_pPTZDriver->ctrlPTZ(DISCHARGE_MOTOR,TURN_STOP,0);
          printf("--- Robot:%s(), ctrlPTZ DISCHARGE_MOTOR STOP, nRet: %d\n",__FUNCTION__, nRet);
        }
        break;

        case DISCHARGE_FORWARD_CMD:
        {
          nRet = m_pPTZDriver->ctrlPTZ(DISCHARGE_MOTOR,TURN_UP,0);
          printf("--- Robot:%s(), ctrlPTZ DISCHARGE_MOTOR TURN_UP, nRet: %d\n",__FUNCTION__, nRet);
        }
        break;

        case DISCHARGE_BACKWARD_CMD:
        {
          nRet = m_pPTZDriver->ctrlPTZ(DISCHARGE_MOTOR,TURN_DOWN,0);
          printf("--- Robot:%s(), ctrlPTZ DISCHARGE_MOTOR TURN_DOWN, nRet: %d\n",__FUNCTION__, nRet);
        }
        break;

        default:
        break;
      }
	  }
    break;

    default:
    break;
  }
  res.nErrcode = (0 == nRet) ? 0 : nRet;
  res.sTransId = control.sTransId;

  return nRet;
}

bool Robot::responseSetting(COMPkg::Setting::Request &setting, COMPkg::Setting::Response &res)
{
  int nRet = 0;
  PowerDriver *powerDriver = PowerDriver::getInstance();
  SettingService *settingService = SettingService::getInstance();
  RadarManager *pRadar = RadarManager::getInstance();

  res.nErrcode = 0;
  res.sFeedback = "";
  std::list<float> chargingPiples;
  chargingPiples.clear();
  std::string logname = LOG_TAG + std::string(__FUNCTION__);
  auto logger = initLogger(logname);

  for(int i = 0; i < setting.generalList.size(); ++i)
  {
    int nKey = setting.generalList[i].nKey;
    COMPkg::Mode mode = setting.generalList[i].mode;
    std::string strValue = setting.generalList[i].strValue;
    int nValue = atoi(strValue.c_str());
    float fValue = atof(strValue.c_str());
    printf("--- Robot::%s(), nKey: %d, strValue: %s, nValue: %d, fValue: %f\n", __FUNCTION__, nKey, strValue.c_str(), nValue, fValue);
    LOG4CPLUS_INFO(logger, " nKey: " << nKey << ", strValue: " << strValue.c_str() << ", nValue: " << nValue << ", fValue: " << fValue);

    switch(nKey) {
      case CAMERA_IP:
      {
        m_bReconnectCamera = true;
        settingService->m_setting.setIndustrialCameraIp(strValue);
      }
      break;
      case CAMERA_PORT:
      {
        settingService->m_setting.setIndustrialCameraPort(nValue);
      }
      break;
      case THERMAL_IP:
      {
        m_bReconnectThermel = true;
        settingService->m_setting.setThermalImagerIp(strValue);
      }
      break;
      case THERMAL_PORT:
      {
        settingService->m_setting.setThermalImagerPort(nValue);
      }
      break;
      case DEFAULT_SPEED:
      {
        float defaultSpeed = fValue;
        if(defaultSpeed <= 10) { defaultSpeed = 10; }
        else if(defaultSpeed >= 2000) { defaultSpeed = 2000; }
        nRet += m_pMotor->setDefaultSpeed(defaultSpeed);
        settingService->m_setting.setDefaultSpeed(defaultSpeed);
        printf("%s(), set default speed: %fmm/s\n", __FUNCTION__, defaultSpeed);
        LOG4CPLUS_INFO(logger, " set default speed: " << defaultSpeed << "mm/s");
      }
      break;
      case ORBIT_END_POS:
      {
        float beginPos = settingService->m_setting.getBeginPos();
        float endPos = nValue;
        float curPos = m_pMotor->getCurPosition();
        if (curPos <= endPos + 10) {          //防止数值小幅跳动
            nRet += m_pMotor->setEndPos(endPos);
            settingService->m_setting.setEndPos(endPos);
            printf("%s(), set end pos: %fmm\n", __FUNCTION__, endPos);
            LOG4CPLUS_INFO(logger, " set end pos: " << endPos << "mm");

        }
        else {
          nRet += -1;
          res.sFeedback = "终点值不在范围";
          printf("%s(), end pos %fmm not in range, curPos: %f, beginPos: %f!\n", __FUNCTION__, endPos, curPos, beginPos);
          LOG4CPLUS_INFO(logger, " endPos " << endPos << "mm not in range, curPos: " 
                                  << curPos << ", beginPos: " << beginPos);
        }
      }
      break;
      case ORBIT_BEGIN_POS:
      {
        float beginPos = nValue;
        float endPos = settingService->m_setting.getEndPos();
        float curPos = m_pMotor->getCurPosition();
        if (curPos >= beginPos - 10) {
          nRet += m_pMotor->setBeginPos(beginPos);
          settingService->m_setting.setBeginPos(beginPos);
          printf("%s(), set begin pos: %fmm\n", __FUNCTION__, beginPos);
          LOG4CPLUS_INFO(logger, " set begin pos: " << beginPos << "mm");
        }
        else {
          nRet += -1;
          res.sFeedback = "起点值不在范围";
          printf("%s(), begin pos %fmm not in range, curPos: %f, endPos: %f!\n", __FUNCTION__, beginPos, curPos, endPos);
          LOG4CPLUS_INFO(logger, " beginPos " << beginPos << "mm not in range, curPos: " 
                                  << curPos << ", endPos: " << endPos);
        }
      }
      break;
      case ORBIT_CUR_POS:
      {
        float beginPos = settingService->m_setting.getBeginPos();
        float endPos = settingService->m_setting.getEndPos();
        if (beginPos <= fValue && endPos >= fValue) {
          nRet += m_pMotor->setCurPos(fValue);
          printf("%s(), set cur pos: %fmm\n", __FUNCTION__, fValue);
                LOG4CPLUS_INFO(logger, " set cur pos: " << fValue << "mm");
        }
        else {
          nRet += -1;
          printf("%s(), cur pos: %fmm is not in range(%f - %f)\n", __FUNCTION__, fValue, beginPos, endPos);
                LOG4CPLUS_INFO(logger, " cur pos: " << fValue << "mm is not in range(" << beginPos << " - "<< endPos <<")");
        }
      }
      break;
      case ORBIT_ACC:
      {
          float acc = fValue;
          if (acc < 0) { 
              nRet += -1;
              printf("%s(), acc: %f is incorrect\n", __FUNCTION__, acc);
          }
          else {
              nRet += m_pMotor->setAcc(acc);
              printf("%s(), set acc: %fmm\n", __FUNCTION__, acc);
              LOG4CPLUS_INFO(logger, " set acc: " << acc << "mm");
          }
      }
      break;
      case ORBIT_DEC:
      {
          float dec = fValue;
          if (dec < 0) { 
              nRet += -1;
              printf("%s(), dec: %f is incorrect\n", __FUNCTION__, dec);
          }
          else {
              nRet += m_pMotor->setDec(dec);
              printf("%s(), set dec: %fmm\n", __FUNCTION__, dec);
              LOG4CPLUS_INFO(logger, " set dec: " << dec << "mm");
          }
      }
      break;
      case ARM_BEGIN_POS:
      {
        float beginPos = fValue;
        nRet += m_pMotor->setArmBeginPos(beginPos);
        settingService->m_setting.setArmBeginPos(beginPos);
        printf("%s(), set arm begin pos: %fmm\n", __FUNCTION__, fValue);
        LOG4CPLUS_INFO(logger, " set arm begin pos: " << fValue << "mm");
      }
      break;
      case ARM_END_POS:
      {
        float endPos = fValue;
        nRet += m_pMotor->setArmEndPos(endPos);
        settingService->m_setting.setArmEndPos(endPos);
        printf("%s(), set arm end pos: %fmm\n", __FUNCTION__, fValue);
        LOG4CPLUS_INFO(logger, " set arm end pos: " << fValue << "mm");
      }
      break;
      case ARM_LIFT_MOVING:
      {
        bool armLiftMoving = (bool)nValue;
        settingService->m_setting.setArmLiftMoving(armLiftMoving);
      }
      break;
      case CHARGE_SWITCH:
      {
        /* ChargePileDriver* pDriver = ChargePileDriver::getInstance();
        pDriver->swCtrl(nValue); */
      }
      break;
      case CLEAR_FRID:
      {
        // m_pMotor->clearFRID();
      }
      break;
      case SET_RUN_MODE:
      {
          nRet += -1;
          Json::Value req;
          req["bindMap"] = mode.bindMap;
          req["value"] = mode.value;
          req["mapName"] = mode.mapName;
          req["isCancel"] = mode.isCancel;
          std::string sendStr = req.toStyledString();

          printf("%s(), set run mode: %d -> %d, sendStr: %s\n", __FUNCTION__, settingService->m_setting.getRunMode(), mode.value, sendStr.c_str());
          LOG4CPLUS_INFO(logger, " set run mode: " << settingService->m_setting.getRunMode() << " -> " << mode.value << ", sendStr: " << sendStr);
          if (settingService->m_setting.getRunMode() == SCAN_MODE && mode.value != SCAN_MODE) {
              //放弃本次扫描结果
              if (mode.isCancel) {
                  if (m_pBusinessInfor->rfidCalibration(MAP_SCAN_CANCEL) != 0) {
                      printf("%s(), MAP_SCAN_CANCEL fail!\n", __FUNCTION__);
                      break;
                  }
              }
              //保存本次扫描结果
              else {
                  /////////////////////////////////////////////////////
                  //新建地图和切换地图分开实现
                  //新建：
                  // 1）从Driver/单片机取参数(getMapInfoFromDriver())
                  // 2）插入数据库          (insertMapInfoIntoDB())
                  // 3）根据bind配置，决定是否bind
                  //切换：
                  // 1）查询数据库获得mapId (getMapInfoFromDB())
                  // 2）直接bind            (updateBindMapInfo())
                  // 3）下发给Driver/单片机 (setMapInfoIntoDriver())
                  //配置参数：
                  // 1）RFID
                  // 2）充电桩
                  // 3）巡检任务
                  // 4）定点动作
                  /////////////////////////////////////////////////////

                  // 新建：地图,RFID地图
                  RobotMap::Header newMap;
                  newMap.name = mode.mapName;
                  newMap.id = 0;
                  std::vector<RfidMapping> rfidMap;

                  printf("+++++++++++++++++++++++++ New Map    +++++++++++++++++++++++\n");
                  if (Robot::getMapInfoFromDriver(newMap, rfidMap)) {
                      printf("%s(), getMapInfoFromDriver() fail!\n", __FUNCTION__);
                      break;
                  }

                  // 插库：地图,RFID地图
                  printf("+++++++++++++++++++++++++ Insert Map +++++++++++++++++++++++\n");
                  if (insertMapInfoIntoDB(newMap, rfidMap)) {
                      printf("%s(), insertMapInfoIntoDB() fail!\n", __FUNCTION__);
                      break;
                  }

                  if (mode.bindMap) {   //绑定本次扫描的地图
                      if (m_pBusinessInfor->rfidCalibration(MAP_SCAN_STOP) != 0) {
                          printf("%s(), MAP_SCAN_STOP fail\n", __FUNCTION__);
                          break;
                      }
                  }
                  else {                //不绑定，但保存地图
                      if (m_pBusinessInfor->rfidCalibration(MAP_SCAN_CANCEL) != 0) {
                          printf("%s(), MAP_SCAN_CANCEL fail\n", __FUNCTION__);
                          break;
                      }
                  }

                  if (mode.bindMap) {   //绑定本次扫描的地图
                      printf("+++++++++++++++++++++++++ Bind Map Y +++++++++++++++++++++++\n");
                      // 绑定地图
                      if (newMap.id) {//插库成功
                          bindSimpleMap(newMap.id);
                          printf("%s()@%d, Bind to simple map, id is (%d)\n", __FUNCTION__, __LINE__, newMap.id);

                          //清除旧有的任务
                          tasksRefresh();

                          //清空旧充电桩
                          ChargePileDriver::deleteChargePile();
                          std::map<int, char*> chargePilesMap;
                          chargePilesMap.clear();
                          m_chargingPilesPos.clear();
                          m_chargingPilesName.clear();
                          m_pChargePileDriver->init(chargePilesMap);    
                      }
                      else {
                          printf("%s()@%d, Cannot bind simple map, id invalid (%d)\n", __FUNCTION__, __LINE__, newMap.id);
                      }
                  }

#if 0
                  //获取地图
                  std::vector<RfidMapping> rfidMap;
                  if (m_pBusinessInfor->rfidMapGet(rfidMap) != 0) {
                      break;
                  }
                  //存入数据库
                  RFIDService::deleteRFID();
                  RFID rfid;
                  for (auto iter = rfidMap.begin(); iter != rfidMap.end(); iter++) {
                      RFID rfid;
                      rfid.setRfidNum(iter->num);
                      rfid.setPosition(iter->pos);
                      RFIDService::insertSelective(rfid);
                  }
                  //清空旧充电桩
                  ChargePileDriver::deleteChargePile();
                  std::map<int, char*> chargePilesMap;
                  chargePilesMap.clear();
                  m_chargingPilesPos.clear();
                  m_chargingPilesName.clear();
                  m_pChargePileDriver->init(chargePilesMap);
#endif
              }
          }
          else if (settingService->m_setting.getRunMode() != SCAN_MODE && mode.value == SCAN_MODE) {
              if (m_pBusinessInfor->rfidCalibration(MAP_SCAN_START) != 0) {
                  printf("%s(), MAP_SCAN_START fail\n", __FUNCTION__);
                  break;
              }
          }
          settingService->m_setting.setRunMode((RunMode)mode.value);
          printf("%s(), set run mode finished success\n", __FUNCTION__);
          LOG4CPLUS_INFO(logger, " set run mode finished");
          nRet += 1;
      }
      break;
      case CHARGEPILE_POSITION:
      {
        if (m_pRobotCurMap != NULL) {
          std::vector<ChargePile> vChargePile;
          chargePileService->queryByMapId(m_pRobotCurMap->mapHeader().id, vChargePile);
          std::map<int, char*> chargePilesMap;
          chargePilesMap.clear();
          m_chargingPilesPos.clear();
          m_chargingPilesName.clear();
          printf("%s(), get chargepile position: ", __FUNCTION__);
          for(int i = 0; i < vChargePile.size(); ++i)
          {
            printf("%d ", vChargePile[i].getPosition());
            m_chargingPilesPos.push_back(vChargePile[i].getPosition());
            std::cout << "Charge pile name: "<< vChargePile[i].getInstanceName() << endl;
            m_chargingPilesName.push_back(vChargePile[i].getInstanceName());
            chargePilesMap.insert(std::pair<int, char*>(vChargePile[i].getPosition(), const_cast<char*>(vChargePile[i].getIp().c_str())));
          }
          m_pChargePileDriver->init(chargePilesMap);
        }
      }
      break;
      case CGARGEPILE_MIN_POWER:
      {
        float quantityMinLimit = fValue;
        printf("%s(), get min quantity limit: %f%%\n", __FUNCTION__, quantityMinLimit);
        LOG4CPLUS_INFO(logger, " get min quantity limit: " << quantityMinLimit << "%%");
        if( quantityMinLimit < 10 ) { quantityMinLimit = 10; }
        else if( quantityMinLimit > 90 ) { quantityMinLimit = 90; }
        settingService->m_setting.setBatteryQuantityMin(quantityMinLimit);
        if (m_pBatteryDriver != NULL) {
          float quantity = m_pBatteryDriver->getQuantity();
          if(quantity > quantityMinLimit) {    //电量正常，清除低电量充电任务
              m_pMoveTaskManage->clearChargeTaskCommands();
          }
        }
      }
      break;
      case BATTERY_MAX_QUANTITY:
      {
        // m_fBatteryMAxQuantity = stof(setting.generalList[i].strValue);
      }
      break;
      case MOTOR_CONTROL_LOCK:
      {
        bool isMotorLocked = (bool)nValue;
        //把标志位加到motordriver里面，在movecontrolcommand里面设置
        if (isMotorLocked) {
            m_pMotor->motionControl(Motion_Lock, isMotorLocked);
        }
        else {
            settingService->m_setting.setMotorLock(isMotorLocked);
        }
      }
      break;
      case ULSOUND_THRESHOLD:
      {
        nRet += pRadar->setWarnThreshold(nValue);
      }
      break;
      case ULSOUND_BACK_THRESHOLD:
      {
        nRet += pRadar->setBackWarnThreshold(nValue);
      }
      break;
      case ULSOUND_DOWN_THRESHOLD:
      {
        nRet += pRadar->setDownWarnThreshold(nValue);
      }
      break;
      case POSPLAN_UPDATE:
      {
        PositionMove* posMove = PositionMove::getInstance();
        if(posMove->m_ready == false){
          nRet += -1;
        }
        else{
          nRet += posMove->updatePosPlan();
        }
      }
      break;
      case ROBOT_REBOOT:
      {
          printf("Robot::%s(), starting reboot", __FUNCTION__);
          LOG4CPLUS_INFO(logger, " starting reboot");
          system("sudo reboot");
      }
      break;
      case CHANGE_MAP:
      {
        int mapId = nValue;
        
        //TODO:下发map，更新rfid等信息
        // nRet = m_pMotor->changeMap(mapId);
        printf("%s(), change map, mapId: %d, nRet: %d\n", __FUNCTION__, mapId, nRet);
        LOG4CPLUS_INFO(logger, " change map, mapId: " << mapId << ", nRet: " << nRet);
        if (nRet != RET_SUCCESS) { break; }

        //切换地图
        // 暂不支持，单片机侧不支持
        //nRet += updateBindMapInfo(mapId);   
        //nRet = switchMap(mapId);
      }
      break;
      case PTZ_SWITCH:
      {
        bool isPtzLocked = (bool)nValue;
        settingService->m_setting.setPtzSwitch(isPtzLocked);
        std::cout << "PTZ Rotate Switch:" << isPtzLocked << std::endl;
        LOG4CPLUS_INFO(logger, " PTZ Rotate Switch: " << isPtzLocked);
      }
      break;
      case ULSOUND_SWITCH:
      {
        bool isUlsound = (bool)nValue;
        if (RadarDriver::getInstance()->radarSwitch(RADAR_FRONT, isUlsound) == 0) {
            settingService->m_setting.setUlsoundSwitch(isUlsound);
        }
        std::cout << "ULSOUND Switch:" << isUlsound << std::endl;
        LOG4CPLUS_INFO(logger, " ULSOUND Switch: " << isUlsound);
      }
      break;
      case SET_MOVE_WARN:
      {
        bool isWarn = (bool)nValue;
        settingService->m_setting.setMoveWarn(isWarn);
        printf("%s(), set move warn: %d, nRet: %d\n", __FUNCTION__, isWarn, nRet);
        LOG4CPLUS_INFO(logger, " set move warn: " << isWarn << ", nRet: " << nRet);
      }
      break;
      case ULSOUND_SWITCH_BACK:
      {
        bool isUlsound = (bool)nValue;
        if (RadarDriver::getInstance()->radarSwitch(RADAR_BACK, isUlsound) == 0) {
            settingService->m_setting.setBackUlsoundSwitch(isUlsound);
        }
        std::cout << "Back ULSOUND Switch:" << isUlsound << std::endl;
        LOG4CPLUS_INFO(logger, " Back ULSOUND Switch: " << isUlsound);
      }
      break;
      case ULSOUND_SWITCH_DOWN:
      {
        bool isUlsound = (bool)nValue;
        if (RadarDriver::getInstance()->radarSwitch(RADAR_DOWN, isUlsound) == 0) {
            settingService->m_setting.setDownUlsoundSwitch(isUlsound);
        }
        std::cout << "Down ULSOUND Switch:" << isUlsound << std::endl;
        LOG4CPLUS_INFO(logger, " Down ULSOUND Switch: " << isUlsound);
      }
      break;
      case PTZ_SLEEP_SWITCH:
      {
        bool isPtz = (bool)nValue;
        settingService->m_setting.setPtzSleepSwitch(isPtz);
        //关闭即给电
        if(!isPtz){
          if(settingService->m_setting.getPtzPowerSwitch())
          {
            m_pPowerDriver->switchPowerPtz(!isPtz);
            ptz_status = !isPtz;
          }
        }
        std::cout << "Ptz Sleep Switch:" << isPtz << std::endl;
      }
      break;
      case PTZ_MANUAL_SWITCH:
      {
        bool isPtz = (bool)nValue;
        m_pPowerDriver->switchPowerPtz(isPtz);
        settingService->m_setting.setPtzPowerSwitch(isPtz);
        ptz_status = isPtz;
        std::cout << "Ptz Switch:" << isPtz << std::endl;
      }
      break;
      case DRIVER_MSG_SWITCH:
      {
          bool isDrvSwitchOn = (bool)nValue;
          settingService->m_setting.setDriverMsgSwitch(isDrvSwitchOn);
          std::cout << "Driver message Switch:" << isDrvSwitchOn << std::endl;
      }
      break;
      case LIFT_MANUAL_SWITCH:
      {
        bool isLiftSwitch = (bool)nValue;
        settingService->m_setting.setLiftRestrictSwitch(isLiftSwitch);
        std::cout << "Lift Restrict Switch:" << isLiftSwitch << std::endl;
      }
      break;
      case HOME_SWITCH:
      {
        bool isHomeSwitch = (bool)nValue;
        settingService->m_setting.setHomeSwitch(isHomeSwitch);
        std::cout << "Home Switch:" << isHomeSwitch << std::endl;
      }
      break;
      default:
          break;
    }
  }

  res.nErrcode = (0 == nRet) ? 0 : nRet;
  res.sTransId = setting.sTransId;

  return nRet;
}

bool Robot::responseControl(COMPkg::Control::Request &control, COMPkg::Control::Response &res)
{
  int nRet = 0;
  int nCmd = 0;
  int nValue = 0;
  float fValue = 0.0;
  res.sFeedback = "";

  nCmd = control.nCommand;
  nValue = control.nValue;
  fValue = control.fValue;
  printf("--- Robot:%s(), nCmd = %d, nValue = %d, fValue = %f\n", __FUNCTION__, nCmd, nValue, fValue);
  switch(nCmd) 
  {
	  case EMERGENCE_CMD:
	  {
      m_pMotor->motionControl(Motion_EmergencyStop, control.fValue);
	    break;
	  }
	    
	  case INSPECTION_PAUSE_CMD:
	  {
	    break;
	  }

	  case MANUAL_MOTOR_CMD:
	  {
      if(m_pMotor->isGoingHomingStatus()) {
          res.nErrcode=-2;
          res.sFeedback="升降臂归零中";
          return nRet;
      }   
      SettingService *settingService = SettingService::getInstance();
      if(settingService->m_setting.getLiftRestrictSwitch() && 
        (nValue == Motion_Forward || nValue == Motion_Backward || nValue == Motion_Goto||nValue==Motion_StepBackward||nValue==Motion_StepForward) &&
        m_pMotor->getArmCurPosition()>(settingService->m_setting.getArmBeginPos()+30)) {
          nRet = -1;
          res.sFeedback = "请收起升降臂";
          break;
      }
      switch (nValue) 
      {
        case Motion_Stop:
        {
          m_pMotor->motionControl(Motion_Stop, control.fValue);
          break;
        }

        case Motion_StepForward:
        {
          m_pMotor->motionControl(Motion_StepForward, control.fValue);
          break;
        }
          
        case Motion_StepBackward:
        {
            m_pMotor->motionControl(Motion_StepBackward, control.fValue);
            break;
        }
        
        case Motion_Forward:
        {
            m_pMotor->motionControl(Motion_Forward, control.fValue);
            break;
        }

        case Motion_Backward:
        {
            m_pMotor->motionControl(Motion_Backward, control.fValue);
            break;
        }

        case Motion_EmergencyStop:
        {
            m_pMotor->motionControl(Motion_EmergencyStop, control.fValue);
            break;
        }

        case Motion_Goto:
        {
            m_pMotor->motionControl(Motion_Goto, control.fValue,control.posName);
            break;
        }

        case Motion_Lock:
        {
            m_pMotor->motionControl(Motion_Lock, control.fValue);
            break;
        }

        case Motion_Unlock:
        {
            m_pMotor->motionControl(Motion_Unlock, control.fValue);
            break;
        }

        case Motion_Arm_Up:
        {
          if (m_pCapabilities->isSupportArmReverse()) {
            m_pMotor->motionControl(Motion_Arm_Down, control.fValue);
          }
          else {
            m_pMotor->motionControl(Motion_Arm_Up, control.fValue);
          }            
          break;
        }

        case Motion_Arm_Down:
        {
          if (m_pCapabilities->isSupportArmReverse()) {
            m_pMotor->motionControl(Motion_Arm_Up, control.fValue);
          }
          else {
            m_pMotor->motionControl(Motion_Arm_Down, control.fValue);
          }
          break;
        }

        case Motion_Arm_Stop:
        {
          m_pMotor->motionControl(Motion_Arm_Stop, control.fValue);
          break;
        }

        case Motion_Arm_MoveTo:
        {
          m_pMotor->motionControl(Motion_Arm_MoveTo, control.fValue);
          break;
        }

        case Motion_PDD_Forward:
        {
          nRet = m_pPTZDriver->ctrlPTZ(DISCHARGE_MOTOR,TURN_UP,0);
          printf("--- Robot:%s(), ctrlPTZ DISCHARGE_MOTOR STOP, nRet: %d\n",__FUNCTION__, nRet);
          break;
        }

        case Motion_PDD_Backward:
        {
          nRet = m_pPTZDriver->ctrlPTZ(DISCHARGE_MOTOR,TURN_DOWN,0);
          printf("--- Robot:%s(), ctrlPTZ DISCHARGE_MOTOR STOP, nRet: %d\n",__FUNCTION__, nRet);
          break;
        }

        case Motion_PDD_Stop:
        {
          nRet = m_pPTZDriver->ctrlPTZ(DISCHARGE_MOTOR,TURN_STOP,0);
          printf("--- Robot:%s(), ctrlPTZ DISCHARGE_MOTOR STOP, nRet: %d\n",__FUNCTION__, nRet);
          break;
        }

        default:
        {
          break;
        }
      }
      break;
	  }

	  case RECHARGE_CMD:
	  {
	    m_pMoveTaskManage->manulCharge();
	    break;
	  }
	    
	  case RUN_MODE:
	  {
		// m_pMotor->setMotionMode((RunMode)0);
	    break;
	  }
	    
	  case ECECUTE_TASK_NOW:
	  {
	    int nMotion = (int)Motion_Stop;
	    m_pMoveTaskManage->manulMoveControl(nMotion, control.fValue);
	    break;  
	  }
	    
	  case CLOSE_FIRE_DOOR:
	  {
	    // m_pFireDoorDriver->switchStatus(nValue, FIRE_DOOR_OPT_CLOSED);
	  }
	    break;
	  case OPEN_FIRE_DOOR:
	  {
	    // m_pFireDoorDriver->switchStatus(nValue, FIRE_DOOR_OPT_OPEN);
	  }
	    break;
	  case POWER_SWITCH:
	  {
	    //m_pPowerDriver->switchPowerBattery(nValue);
	    break;
	  }
	    
	  case MOTOR_SWITCH:
	  {
        LowConManage *pMng = LowConManage::getInstance();
        pMng->moveMotorSwitch(nValue);
        break;
	  }

    case RADAR_SWITCH_CMD:
    {
        LowConManage *pMng = LowConManage::getInstance();
        pMng->radarSwitch(nValue);
        break;
    }

    case CHARGE_FLAG:
    {
        /* ChargePileDriver *pCharge = ChargePileDriver::getInstance();
        pCharge->swCtrl(nValue); */
      break;
    }

    case CHARGE_SWITCH_CMD:
    {
        /* ChargePileDriver *pCharge = ChargePileDriver::getInstance();
        pCharge->swCtrl(0);
        sleep(1);
        pCharge->swCtrl(1); */
      break;
    }

    case INQUERY_POS_CMD:
    {
        res.sFeedback = m_pMotor->getPosName();
        break;
    }

    case READ_MOTOR_DATA_CMD:
    {
        int nMotorId = nValue & 0xff;
        int subindex = (nValue >> 8) & 0xff;
        int index = (nValue >> 16) & 0xffff;
        int data = 0;
        // nRet = m_pMotor->m_pMotorDriver->readSDOData(nMotorId, index, subindex, &data);
        char msg[20] = "";
        snprintf(msg, sizeof(msg), "%d", data);
        res.sFeedback = msg;

        printf("%s(), motorid: %d, index: %x, subindex: %x, data: %d\n", __FUNCTION__, nMotorId, index, subindex, data);
        break;
    }

    case WRITE_MOTOR_DATA_CMD:
    {
        int nMotorId = nValue & 0xff;
        int subindex = (nValue >> 8) & 0xff;
        int index = (nValue >> 16) & 0xffff;
        int data = (int)control.fValue;
        // nRet = m_pMotor->m_pMotorDriver->writeSDOData(nMotorId, index, subindex, data);
        printf("%s(), motorid: %d, index: %x, subindex: %x, data: %d\n", __FUNCTION__, nMotorId, index, subindex, data);
        break;
    }

    case VERSION_QUERY_CMD:
    {
      VersionQuery* pVersionQuery = VersionQuery::getInstance();
      nRet = pVersionQuery->queryVersion((VersionInfo)nValue);
      res.sFeedback = pVersionQuery->getVersion();
      break;
    }

    case HOME_CMD:
	  {
      if(m_pCapabilities->isSupportArmMotor() && m_pCapabilities->isSupportPtzMotor()) {
          m_pMoveTaskManage->createPtzArmHomeTaskItem();
      }
      nRet = 0;
	    break;
	  }

    default:
    {
        break;
    }
  }

  res.nErrcode = (0 == nRet) ? 0 : nRet;
  res.sTransId = control.sTransId;

  return nRet;
}

bool Robot::responseTasksSyncDown(COMPkg::SyncDown::Request &tasksReq, COMPkg::SyncDown::Response &res)
{
    int nRet = 0;
    Robot* pRobot = Robot::getInstance();
    MoveTaskManage* moveTaskManage = pRobot->m_pMoveTaskManage;

    printf("--- Robot:%s(), taskType = %d, taskId = %d\n", __FUNCTION__, tasksReq.nTaskType, tasksReq.nTaskId);
    tasksRefresh();

    //();
    if(tasksReq.nTaskType == TASK_NOW) {        //立即执行任务，手动控制执行一遍
        for(std::list<Task>::iterator iter = pRobot->m_taskList.begin(); iter != pRobot->m_taskList.end(); ++iter) {
            if(iter->getTaskId() == tasksReq.nTaskId) {
                moveTaskManage->clearManualTaskCommands();
                pRobot->mkdirsByDate();
                Task task = *iter;
                RunMode curRunMode = SettingService::getInstance()->m_setting.getRunMode();
                moveTaskManage->createPatroPlanTaskItem(task, MoveTaskManage::TP_MANUL);
                printf("--- Robot:%s(), get task success\n", __FUNCTION__);
            }
        }
    }
    //();

    res.nErrcode = (0 == nRet) ? 0 : nRet;
    res.sTransId = tasksReq.sTransId;

    return nRet;
}

//任务列表更新
void Robot::tasksRefresh()
{
    //查询数据库
    //();
    m_taskList.clear();
    if (m_pRobotCurMap != NULL) {
        m_pTaskService->queryTask(m_nRobotID, m_pRobotCurMap->mapHeader().id, m_taskList);
        printf("%s(), update task, task id: ", __FUNCTION__);
        for(auto iter = m_taskList.begin(); iter != m_taskList.end(); ++iter) {
            printf("%d ", iter->getTaskId());
        }
        printf("\n");
    }
    //();
}

///////////////////////////////////
// 和地图相关的信息
// 1）RFID
// 2）充电桩
// 3）巡检任务
// 4）定点动作
///////////////////////////////////

// 从Driver/单片机获取和地图相关联的信息
int Robot::getMapInfoFromDriver(RobotMap::Header &newMap, std::vector<RfidMapping> &rfidMap)
{
    //新增地图
    newMap.minPosition = 0;
    newMap.maxPosition = m_pMotor->getCurPosition();
    //mapId 需要插库后，由数据库返回，详细见函数insertMapInfoIntoDB()

    //获取RFID
    if (m_pBusinessInfor->rfidMapGet(rfidMap)) {
        printf("Function %s, Get RFID failed!\n", __FUNCTION__);
        return -1;
    }

    for (auto it = rfidMap.begin(); it != rfidMap.end(); ++it) {
        printf("%s()@%d, RFID num:%lld, pos:%d\n", __FUNCTION__, __LINE__, it->num, it->pos);
    }


    //充电桩
    //巡检任务
    //定点动作

    return 0;
}
//把和地图相关的信息插入数据库
int Robot::insertMapInfoIntoDB(RobotMap::Header &newMap, std::vector<RfidMapping> &rfidMap)
{
    //Map
    newMap.id = MapService::insertNewMapHeader(newMap);

    //RFID
    printf("+++++++++ RFID size: %d ++++++++\n", rfidMap.size());
    for (auto iter = rfidMap.begin(); iter != rfidMap.end(); iter++) {
        RFID rfid;
        rfid.setRfidNum(iter->num);
        rfid.setPosition(iter->pos);
        RFIDService::insertSelective(rfid);
        if (true == RFIDService::updateRFIDbyMapId(rfid, newMap.id)) {
            std::cout << __FUNCTION__ << ":"
                      << "@" << __LINE__
                      <<"updateRFIDbyMapId:: [MapId]: " << newMap.id
                      << ", [Rfid_Num]: " << rfid.getRfidNum()
                      << ", [Rfid_Pos]: " << rfid.getPosition()
                      << std:: endl;
        }
        else {
            std::cout << "saveNewMap: Fail to update mapId of rfidNum: " << rfid.getRfidNum() << std::endl;
        }
    }


    //充电桩
    //巡检任务
    //定点动作
    return 0;
}

//从数据库中获取和地图相关的信息
int Robot::getMapInfoFromDB(const uint32_t mapId,                     //map id
                        RobotMap::Ptr &map,                    //map info
                        std::vector<RfidMapping> &rfidMap,     //RFID
                        std::list<float> &vChargePilePos,      //充电桩
                        std::list<Task> &taskList,             //巡检任务
                        std::list<PositionPlan> &posPlan       //定点动作
)
{
    auto logger = initLogger(LOG_TAG + std::string(__FUNCTION__));
    int nRet = 0;
    //Map
    map = MapService::queryMapbyId(mapId);
    if (map == NULL) {
        nRet = -1;
        printf("%s(), map info query failed!\n", __FUNCTION__);
        LOG4CPLUS_ERROR(logger, " map info query failed!");
        return nRet;
    }

    //RFID
    std::list<RFID> rfids;
    rfidMap.clear();
    if (RFIDService::queryRFIDListByMapId(mapId, rfids) > 0) {
        for (auto iter = rfids.begin(); iter != rfids.end(); ++iter) {
            RfidMapping map;
            map.num = iter->getRfidNum();
            map.pos = iter->getPosition();
            rfidMap.push_back(map);
            printf("%s(), get rfid list num: %ll, pos: %d \n", __FUNCTION__, map.num, map.pos);
        }
    }
    else {
        printf("%s(), query rfid list failed\n", __FUNCTION__);
    }

    //充电桩
    ChargePileService* chargePileService = ChargePileService::getInstance();
    std::vector<ChargePile> vChargePile;
    //std::list<float> vChargePilePos;
    vChargePilePos.clear();
    nRet = chargePileService->queryByMapId(mapId, vChargePile);
    if (nRet != 0) {
        return nRet;
    }
    printf("%s(), get chargepile position: ", __FUNCTION__);
    for (int i = 0; i < vChargePile.size(); ++i) {
        printf("%d ", vChargePile[i].getPosition());
        vChargePilePos.push_back(vChargePile[i].getPosition());
    }
    printf("\n");

    //巡检任务
    //std::list<Task> taskList;
    taskList.clear();
    nRet = m_pTaskService->queryTask(m_nRobotID, mapId, taskList);
    if (nRet != 0) {
        return nRet;
    }
    printf("%s(), get task, task id: ", __FUNCTION__);
    for (auto iter = taskList.begin(); iter != taskList.end(); ++iter) {
        printf("%d ", iter->getTaskId());
    }
    printf("\n");

    //定点动作
    //std::list<PositionPlan> posPlan;
    posPlan.clear();
    nRet = PositionPlanService::queryPPListByRobotMap(m_nRobotID, mapId, posPlan);
    if (nRet != 0) {
        return nRet;
    }
    printf("%s(), get position plan, plan id: ", __FUNCTION__);
    for (auto iter = posPlan.begin(); iter != posPlan.end(); ++iter) {
        printf("%d ", iter->getId());
    }
    printf("\n");
    return 0;
}

//把RFID地图相关的信息下发给Driver/单片机
int Robot::setMapInfoIntoDriver(std::vector<RfidMapping>& rfid)
{
    //RFID
    if (m_pBusinessInfor->rfidMapImport(rfid))
    {
        printf("%s(), set RFID map into deriver failed!\n", __FUNCTION__);
        return -1;
    }
    return 0;
}

//切换地图
int Robot::switchMap(int mapId)
{
    auto logger = initLogger(LOG_TAG + std::string(__FUNCTION__));
    //
    // 从数据库获取以下信息
    // map info
    // RFID map
    // 充电桩
    // 巡检任务
    // 定点动作
    //
    RobotMap::Ptr map;
    std::vector<RfidMapping> rfidMap;
    std::list<float> vChargePilePos;
    std::list<Task> taskList;
    std::list<PositionPlan> posPlan;
    getMapInfoFromDB(mapId, map, rfidMap, vChargePilePos, taskList, posPlan);

    // bind
    // map
    m_pRobotCurMap = map;
    // FIXME: how to bind RFID??
    setMapInfoIntoDriver(rfidMap);
    // 充电桩
    m_chargingPilesPos = vChargePilePos;
    // 巡检任务
    //();
    m_taskList = taskList;
    //();

    PositionMove* posMove = PositionMove::getInstance();
    posMove->m_nMapId = mapId;
    // 定点动作
    posMove->m_PosPlan = posPlan;

    return 0;
}

//only map ID
int Robot::bindSimpleMap(int mapId)
{
    auto logger = initLogger(LOG_TAG + std::string(__FUNCTION__));
    PositionMove* posMove = PositionMove::getInstance();
    posMove->m_nMapId = mapId;
    auto map = MapService::queryMapbyId(mapId);
    if (map == NULL) {
        printf("%s(), map info query failed!\n", __FUNCTION__);
        LOG4CPLUS_ERROR(logger, " map info query failed!");
        return -1;
    }
    m_pRobotCurMap = map;

    //更新数据库robot表，更新map ID by Robot ID
    if (MapService::updateRobotbyMapId(m_nRobotID, mapId))
    {
        printf("%s(), updateRobotbyMapId(Robot ID: %d, Map ID: %d) SUCCESS!\n", __FUNCTION__, m_nRobotID, mapId);
    }
    else {
        printf("%s(), updateRobotbyMapId(Robot ID: %d, Map ID: %d) FAILED!\n", __FUNCTION__, m_nRobotID, mapId);
        return -1;
    }
    return 0;
}

int Robot::updateBindMapInfo(int mapId)
{
    auto logger = initLogger(LOG_TAG + std::string(__FUNCTION__));
    int nRet = 0;
    if (mapId <= 0) {
        nRet = -1;
        printf("%s(), mapId(%d) is invalid!\n", __FUNCTION__, mapId);
        LOG4CPLUS_ERROR(logger, " mapId(" << mapId << ") is invalid!");
        return nRet;
    }
    //更新map信息
    auto map = MapService::queryMapbyId(mapId);
    if (map == NULL ) {
        nRet = -1;
        printf("%s(), map info query failed!\n", __FUNCTION__);
        LOG4CPLUS_ERROR(logger, " map info query failed!");
        return nRet;
    }

    //更新充电桩信息
    ChargePileService* chargePileService = ChargePileService::getInstance(); 
    std::vector<ChargePile> vChargePile;
    std::list<float> vChargePilePos;
    nRet = chargePileService->queryByMapId(mapId, vChargePile);
    if (nRet != 0) { 
        return nRet;
    }
    printf("%s(), update chargepile position: ", __FUNCTION__);
    for(int i = 0; i < vChargePile.size(); ++i) {
        printf("%d ", vChargePile[i].getPosition());
        vChargePilePos.push_back(vChargePile[i].getPosition());
    }
    printf("\n");
    
    //更新巡检任务信息
    std::list<Task> taskList;
    nRet = m_pTaskService->queryTask(m_nRobotID, mapId, taskList);
    if (nRet != 0) { 
        return nRet;
    }
    printf("%s(), update task, task id: ", __FUNCTION__);
    for(auto iter = taskList.begin(); iter != taskList.end(); ++iter) {
        printf("%d ", iter->getTaskId());
    }
    printf("\n");
    
    //更新定点动作信息
    std::list<PositionPlan> posPlan;
    nRet = PositionPlanService::queryPPListByRobotMap(m_nRobotID, mapId, posPlan);
    if (nRet != 0) { 
        return nRet;
    }
    printf("%s(), update position plan, plan id: ", __FUNCTION__);
    for(auto iter = posPlan.begin(); iter != posPlan.end(); ++iter) {
        printf("%d ", iter->getId());
    }
    printf("\n");

    m_pRobotCurMap = map;
    m_chargingPilesPos = vChargePilePos;

    //();
    m_taskList = taskList;
    //();

    PositionMove* posMove = PositionMove::getInstance();
    posMove->m_nMapId = mapId;
    posMove->m_PosPlan = posPlan;

    return nRet;
}

//按照离当前位置的远近排序充电桩
void Robot::getSortedChargingPilePos(float fCurPosition, std::list<float> &sortedPos, std::list<float> pilePos)
{
    float distance = 999999999;
    int totalPileNum = pilePos.size();
    if(totalPileNum <= 0) {
        printf("Robot::%s, sorted pile pos:", __FUNCTION__);    //打印排序后的位置
        for(std::list<float>::iterator iter = sortedPos.begin(); iter != sortedPos.end(); iter++) {
            printf(" %f", *iter);
        }
        printf("\n");
        return;           //排序完成，退出
    }

    std::list<float>::iterator nearestPosIter, iter = pilePos.begin();
    for(; iter != pilePos.end(); iter++) {
        if(fabs(fCurPosition - *iter) < distance) {
            distance = fabs(fCurPosition - *iter);
            nearestPosIter = iter;
        }
    }
    pilePos.erase(nearestPosIter);
    sortedPos.push_back(*nearestPosIter);

    getSortedChargingPilePos(*nearestPosIter, sortedPos, pilePos);
}

float Robot::getNearestChargingPilePos(float fCurPosition)
{
  float fRet = -1;
  float fDelta = 999999999, fVal = 0, fTmp = 0;

  g_fNearestChargingPilePos = -1.0;
  std::list<float>::iterator iter = m_chargingPilesPos.begin();
  
  for(; iter != m_chargingPilesPos.end(); ++iter) {

    fVal = *iter;
    fTmp = fabs(fCurPosition - fVal);

    if(fTmp < fDelta) {
      fDelta = fTmp;
      g_fNearestChargingPilePos = fVal;
    }

    printf("+++ getNearestChargingPilePos_i %f \n", g_fNearestChargingPilePos);
  }

  printf("+++ getNearestChargingPilePos ret=%f \n", g_fNearestChargingPilePos);
  return g_fNearestChargingPilePos;
}

int Robot::mkdirsByDate()
{
  time_t timep;
  struct tm *p_tm;

  timep = time(NULL);
  p_tm = localtime(&timep);

  char sShellCmd[64] = {0};
  sprintf(sShellCmd, "%s/%d-%d-%d", m_sPhotoRootPath.c_str(), (p_tm->tm_year+1900), (p_tm->tm_mon+1), p_tm->tm_mday);

  if ( 0 == mkdir(sShellCmd, S_IRWXU|S_IRWXG|S_IRWXO ) ) 
  {
    m_nPhotoIndex = 0;
    //m_bNeedReOpen = true;
  } 
  m_sPhotoRootPathDate = sShellCmd;

  return 0;
}

std::string Robot::getTime()
{
  time_t timep;
  time(&timep);

  char tmp[128];
  strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));

  return tmp;
}

std::string Robot::getDate()
{
  time_t timep;
  time(&timep);

  char tmp[128];
  strftime(tmp, sizeof(tmp), "%Y-%m-%d", localtime(&timep));

  return tmp;
}

int Robot::cacuCurDaySecond()
{
  time_t currentTime;
  time(&currentTime);
  struct tm *p;
  p = localtime(&currentTime);
  int nSecond = 3600 * p->tm_hour + 60 * p->tm_min + p->tm_sec;
  return nSecond;
}

time_t Robot::cacuCurSecond()
{
  time_t currentTime;
  time(&currentTime);
  return currentTime;
}

void Robot::cacuHour(int time, int &hour, int &curHourSecond)
{
  hour = time/3600;
  curHourSecond = time%3600;
}

void Robot::cacuDay(int time, int &day, int &curDaySecond)
{
  day = time/3600/24;
  curDaySecond = time%(3600*24);
}

//定时任务轮询，检查是否到时
void Robot::taskRotation(Robot* pRobot)
{
  static time_t lastTime = 0;
  static bool addTaskFlag = false;
  printf("--- Robot::%s() start\n", __FUNCTION__);
  while(true) 
  {
    time_t curTime = pRobot->cacuCurSecond();   //当前时间
    if(lastTime != curTime) {                   //每秒只获取一次
        lastTime = curTime;
    }
    else { 
        usleep(500000);
        continue; 
    }

    for(std::list<Task>::iterator iter = pRobot->m_taskList.begin(); iter != pRobot->m_taskList.end(); ++iter) 
    {
        if(iter->getViewPoints().size() <= 0) { continue; }

        int taskType = iter->getTaskType();         //任务类型,0:周期,1:定期,2:立即
        int repeatUnit = iter->getRepeatUnit();     //重复单位,0:天,1:小时
        int repeatPeriod = iter->getRepeatPeriod(); //重复周期
        time_t startTime = iter->getStartTime();    //任务开始时间
        time_t endTime = iter->getEndTime();        //任务结束时间
        time_t cri_startTime = startTime;

        switch(taskType) {
            case TASK_PERIOD:       //周期执行
            {
                if(curTime < (cri_startTime - 300) || curTime > endTime) { addTaskFlag = false; pRobot->clearPowerSavingStatus(distance(pRobot->m_taskList.begin(),iter)); break; }     //提前300s云台预启动在设置期间内执行

                if(repeatUnit == PERIOD_DAY) {              //每n天触发一次
                    int curDay,startDay;
                    int curDaySecond,startDaySecond;

                    pRobot->cacuDay(curTime, curDay, curDaySecond);
                    pRobot->cacuDay(startTime, startDay, startDaySecond);

                    int nextTask = startDaySecond - curDaySecond;

                    if((curDay - startDay) % repeatPeriod == 0 && 
                        curDaySecond == startDaySecond) {
                        addTaskFlag = true;
                    }

                    if((curDay - startDay) % repeatPeriod == 0 && nextTask > 0 && nextTask < 300)
                    {
                        pRobot->setPowerSavingStatus(distance(pRobot->m_taskList.begin(),iter));
                    }
                    else
                    {
                        pRobot->clearPowerSavingStatus(distance(pRobot->m_taskList.begin(),iter));
                    }
                }
                else if(repeatUnit == PERIOD_HOUR) {        //每n小时触发一次
                    int curHour,startHour;
                    int curHourSecond,startHourSecond;

                    pRobot->cacuHour(curTime, curHour, curHourSecond);
                    pRobot->cacuHour(startTime, startHour, startHourSecond);

                    int nextTask = startHourSecond - curHourSecond;

                    if((curHour - startHour) % repeatPeriod == 0 &&
                        curHourSecond == startHourSecond) {
                        addTaskFlag = true;
                    }

                    if((curHour - startHour) % repeatPeriod == 0 && nextTask > 0 && nextTask < 300)
                    {
                       pRobot->setPowerSavingStatus(distance(pRobot->m_taskList.begin(),iter));
                    }
                    else
                    {
                        pRobot->clearPowerSavingStatus(distance(pRobot->m_taskList.begin(),iter));
                    }
                }
                break;
            }
            case TASK_ONETIME:      //定时执行一次
            {
                time_t startTime = iter->getStartTime();
                time_t curTime = pRobot->cacuCurSecond();
                if(curTime == startTime) {
                    addTaskFlag = true;
                }

                int nextTask = startTime - curTime;
                if( nextTask > 0 && nextTask < 300)
                {
                    pRobot->setPowerSavingStatus(distance(pRobot->m_taskList.begin(),iter));
                }
                else
                {
                    pRobot->clearPowerSavingStatus(distance(pRobot->m_taskList.begin(),iter));
                }
                break;
            }
            case TASK_NOW:          //立即执行
            {
                time_t startTime = iter->getStartTime();
                time_t curTime = pRobot->cacuCurSecond();
                int nextTask = startTime - curTime;
                if( nextTask < 0)
                {
                    pRobot->clearPowerSavingStatus(distance(pRobot->m_taskList.begin(),iter));
                }
                else
                {
                    pRobot->setPowerSavingStatus(distance(pRobot->m_taskList.begin(),iter));
                }
            }
            default:
            {
                pRobot->clearPowerSavingStatus(distance(pRobot->m_taskList.begin(),iter));
                addTaskFlag = false;
                break;
            }
        }

        if(addTaskFlag == true) {
            addTaskFlag = false;
            pRobot->mkdirsByDate();
            Task task = *iter;
            pRobot->m_pMoveTaskManage->createPatroPlanTaskItem(task, MoveTaskManage::TP_REGULAR);
        }
    }
  }
  printf("--- Robot::%s() end\n", __FUNCTION__);
}




void Robot::upLoadPhotos(Robot *pRobot)
{
  system("/home/tetraelc/workspace/T3B_Machine/share/slave_pkg/scp.sh");
}



//定时发布机器运行数据
void Robot::selfInspection(Robot *pRobot)
{
  int nTryReconnect = 0;
  static float fLastCurrentValue = -1.0;
  printf("--- Robot::%s() start\n", __FUNCTION__);
  std::string logname = LOG_TAG + std::string(__FUNCTION__);
  auto logger = initLogger(logname);
  LOG4CPLUS_INFO(logger, " start ");
  while(true) 
  {
    int nRet = 0;
	//检查相机连接情况
    if(pRobot->m_bReconnectCamera)
    {
      	pRobot->m_pIndustrialCamera->init();
        //nRet = pRobot->m_pIndustrialCamera->connect();
        if( 0 == nRet)
        {
            pRobot->m_bReconnectCamera = false;
        }
    }
	//检查热像仪连接情况
    if(pRobot->m_bReconnectThermel) 
    {   
    	pRobot->m_pThermalImager->init();
        //nRet = pRobot->m_pThermalImager->connect();
        if(0 == nRet) 
        {
            pRobot->m_bReconnectThermel = false;
        }
    }

    //检查ros节点运行状况
/*     if( (pRobot->m_bROSAlive == true) && pRobot->m_pDialogue->isRosAlive() ) 
    {

    } 
    else 
    {
        const RosInfo *pRosInfo = pRobot->m_pConfigServer->getRosInfo();
        nRet += pRobot->m_pDialogue->initRos(pRosInfo->sMasterUrl,
                                                pRosInfo->sHostUrl,
                                                pRobot->m_pConfigServer->getRobotId());
        MotorDriver* pMotorDriver = MotorDriver::getInstance();
        nRet += pMotorDriver->registerRosClient(pRobot->m_pConfigServer->getRobotId());
        if(nRet != 0) 
        {
            pRobot->m_bROSAlive = false;
            nTryReconnect++;
            printf("--- initRos Error: %d, %d\n", nRet, nTryReconnect);
            LOG4CPLUS_ERROR(logger, " initRos Error: "<< nRet << nTryReconnect);
        }
        else 
        {
            pRobot->m_bROSAlive = true;
            printf("--- initRos Success\n");
            LOG4CPLUS_INFO(logger, " initRos Success ");

            pRobot->m_pSettingService->querySettingByRobotId(pRobot->m_nRobotID);

            uint32_t mapId = MapService::queryMapIdbyRobotId(pRobot->m_nRobotID);
            if (mapId > 0 && mapId != pRobot->m_pRobotCurMap->mapHeader().id) {
                nRet = pRobot->updateBindMapInfo(mapId);
            }
        }
    }
 */
    sleep(2);
  } 
  printf("--- Robot::%s() end\n", __FUNCTION__);
  LOG4CPLUS_INFO(logger, " end ");
}

//定时发布运行数据
void Robot::timingPublishRunData()
{
    int nTaskId = -1;
    int nExecId = -1;
    int nViewPointId = -1;
    ItemType nTaskType = NO_ITEM;
    printf("--- Robot::%s() start\n", __FUNCTION__);

    int cnt = 0;
    COMParse* pParse = COMParse::getInstance();
    while (1)
    {
        COMPkg::CurStatus msg;
        COMPkg::TaskStatus task_msg;
        COMPkg::AirStatus air_msg;

        std::string sMsg;
        std::string sTaskMsg;
        std::string sAirMsg;

        msg.nRobotId = m_pConfigServer->getRobotId();
        task_msg.nRobotId = m_pConfigServer->getRobotId();
        air_msg.nRobotId = m_pConfigServer->getRobotId();

        //机器版本
        msg.sSoftVersion = m_pConfigServer->getSoftVersion();
        
        //机器人运行模式
        RunMode curRunMode = SettingService::getInstance()->m_setting.getRunMode();
        msg.nMode = curRunMode;

        //热像仪温度
        msg.fMaxTemp = m_pThermalImager->getMaxTemp();

        //局放信息
        if (m_pCapabilities->isSupportPDDetect()) {
            std::vector<int> status;
            int nRet = m_pPTZDriver->getPTZCoordinate(status);
            if(nRet != 0) {
                printf("--- Robot:%s(), read ptz coordinate failed\n",__FUNCTION__);
            }
            if (status.size() >= 4) {
                msg.motor.nPddDis = status[3];
                std::cout << "局放距离: " << status[3] << std::endl;
                msg.motor.nPddStatus = 0;
            }
            msg.pdd.tev = 0;
            msg.pdd.uw = 0;
        }

        //电池信息
        if (m_pCapabilities->isSupportBatteryCharge()) {
            msg.battery.fCurrent = m_pBatteryDriver->getCurrent();
            msg.battery.fQuantity = m_pBatteryDriver->getQuantity();
            msg.battery.fTemperature = m_pBatteryDriver->getTemperature();
            msg.battery.fVoltage = m_pBatteryDriver->getVoltage();
            if (!m_pCapabilities->isSupportSeerBattery()) {
                if (msg.battery.fCurrent > 0.5) { 
                    msg.battery.nChargeStatus = 1; 
                }
                else { 
                    msg.battery.nChargeStatus = 0; 
                }
            }
            else {
                msg.battery.nChargeStatus = m_pBatteryDriver->isCharge();
            }
        }

        //超声波信息
        if(m_pCapabilities->isSupportRadarManage()) {
            msg.radar.fForwardDistance = m_pRadarManager->getDistance(0);
            msg.radar.fBackDistance = m_pRadarManager->getDistance(1);
            if (m_pCapabilities->isSupportRadarBelow()) {
                msg.radar.fDownDistance = m_pRadarManager->getDistance(2);
            }
        }

        //电机信息
        msg.motor.fCurrentPosition = m_pMotor->getCurPosition();
        msg.motor.fYCurrentPosition = m_pMotor->getCurPosition(1);
        msg.motor.fAngleCurrentPosition = m_pMotor->getCurPosition(2);
        msg.motor.fSpeed = m_pMotor->getSpeed();
        msg.motor.fYSpeed = m_pMotor->getSpeed(1);
        msg.motor.fAngleSpeed = m_pMotor->getSpeed(2);
        msg.motor.fArmPosition = m_pMotor->getArmCurPosition();
        msg.motor.nStatus = m_pMotor->getCurStatus();
        msg.motor.fMoveTemp = m_pMotor->getMoveTemp();
        msg.motor.fArmTemp = m_pMotor->getArmTemp();
        msg.motor.fMoveCurrent = m_pMotor->getCurrent();
        msg.motor.fLiftCurrent = m_pMotor->getArmCurrent();
        msg.motor.fArmSpeed = m_pMotor->getArmSpeed();
        /* long long int rfidFlag = m_pMotor->getCheckRFIDNUM();
        msg.motor.checkRFIDNUM = std::to_string(rfidFlag); */

        /* std::cout << "vx: " << msg.motor.fSpeed
                  << "vy: " << msg.motor.fYSpeed
                  << "w: " << msg.motor.fAngleSpeed
                  << "x: " << msg.motor.fCurrentPosition
                  << "y: " << msg.motor.fYCurrentPosition
                  << "a: " << msg.motor.fAngleCurrentPosition
                  << "mc: " << msg.motor.fMoveCurrent
                  << "lc: " << msg.motor.fLiftCurrent
                  << "pdd:" << msg.motor.nPddStatus
                  << "tev:" << msg.pdd.tev
                  << "uw:"  << msg.pdd.uw
                  << std::endl; */

        if(m_pSettingService->m_setting.getMotorLock() == true) {
            msg.motor.nStatus = LOCK_STATE;
        }
        
        pParse->spliceCurStatus(msg, sMsg);
        m_pZmqDriver->publishMsg("/OrbitalRobot/CurStatus/" + sMsg);

        //巡检信息
        // if(m_pMoveTaskManage->getCurTaskItem() != NULL ) {
            int ret = m_pMoveTaskManage->queryCurPlanTaskAndViewPoint(nTaskId, nViewPointId, nExecId, nTaskType);
            if(ret == 0) {
                task_msg.nTaskId = nTaskId;
                task_msg.nExecId = nExecId;
                task_msg.nTaskType = nTaskType;
                task_msg.nViewPointId = nViewPointId;
            }
            else {
                task_msg.nTaskId = 0;
                task_msg.nExecId = 0;
                task_msg.nTaskType = NO_ITEM;
                task_msg.nViewPointId = -1;
            }
            pParse->spliceTaskStatus(task_msg, sTaskMsg);
            m_pZmqDriver->publishMsg("/OrbitalRobot/InspectCurStatus/" + sTaskMsg);
        // }

        //气体信息
        if(cnt > 5 ) {
            cnt = 0;
            air_msg.gas.fCH4 = m_pSensorBoardEx->getGasInfo().fCH4;
            air_msg.gas.fCO = m_pSensorBoardEx->getGasInfo().fCH4;
            air_msg.gas.fO2 = m_pSensorBoardEx->getGasInfo().fO2;
            air_msg.gas.fSO2 = m_pSensorBoardEx->getGasInfo().fSO2;
            air_msg.gas.fSmog = m_pSensorBoardEx->getGasInfo().fSmog;
            air_msg.gas.fCO2 = m_pSensorBoardEx->getGasInfo().fCO2;
            air_msg.env.fIlluminance = m_pSensorBoardEx->getEnvInfo().fIlluminance;
            air_msg.env.fTemperature = m_pSensorBoardEx->getEnvInfo().fTemperature;
            air_msg.env.fHumidity = m_pSensorBoardEx->getEnvInfo().fHumidity;	
            air_msg.pllt.fPm1_0 = m_pSensorBoardEx->getPlltInfo().fPm10;
            air_msg.pllt.fPm2_5 = m_pSensorBoardEx->getPlltInfo().fPm2_5;
            air_msg.pllt.fPm10 = m_pSensorBoardEx->getPlltInfo().fPm10;
            air_msg.pllt.fSF6 = m_pSensorBoardEx->getPlltInfo().fSF6;

            pParse->spliceAirStatus(air_msg, sAirMsg);
            m_pZmqDriver->publishMsg("/OrbitalRobot/GasCurStatus/" + sAirMsg);
        }
                
        /* //PLC告警
        common_pkg::CurStatus curStatusPLC;
        ROSPkg::callPLC msgplc;
        CallPlcFlag FlagNN=m_pWarningManager->getCallPlcFlag();
        EventPlcFlag FlagEvent=m_pWarningManager->getEventPlcFlag();
        msgplc.breakdownFlag=FlagNN.breakdownFlag;
        msgplc.divulgeFlag=FlagNN.divulgeFlag;
        msgplc.gasFlag=FlagNN.gasFlag;
        msgplc.stoneFlag=FlagNN.stoneFlag;
        msgplc.tempFlag=FlagNN.tempFlag;
        int machineidPlc=m_pConfigServer->getRobotId();
        //告警事件触发
        int TypeBreakdownid;
        if(FlagEvent.eventBreakdownFlag)
        {
           TypeBreakdownid=2;
           pParse->splicePlcStatus(TypeBreakdownid,FlagNN.breakdownFlag,machineidPlc,curStatusPLC.sMsg); 
          m_pDialogue->publishCurStatusNP(curStatusPLC);
        }
        if(FlagEvent.eventDivulgeFlag)
        {
           TypeBreakdownid=3;
           pParse->splicePlcStatus(TypeBreakdownid,FlagNN.divulgeFlag,machineidPlc,curStatusPLC.sMsg); 
           m_pDialogue->publishCurStatusNP(curStatusPLC);
        }
        if(FlagEvent.eventGasFlag)
        {
           TypeBreakdownid=1;
           pParse->splicePlcStatus(TypeBreakdownid,FlagNN.gasFlag,machineidPlc,curStatusPLC.sMsg); 
           m_pDialogue->publishCurStatusNP(curStatusPLC);
        }
        if(FlagEvent.eventStoneFlag)
        {
           TypeBreakdownid=4;
           pParse->splicePlcStatus(TypeBreakdownid,FlagNN.stoneFlag,machineidPlc,curStatusPLC.sMsg); 
           m_pDialogue->publishCurStatusNP(curStatusPLC);
        }
        if(FlagEvent.eventTempFlag)
        {
           TypeBreakdownid=0;
           pParse->splicePlcStatus(TypeBreakdownid,FlagNN.tempFlag,machineidPlc,curStatusPLC.sMsg); 
           m_pDialogue->publishCurStatusNP(curStatusPLC);
        } */
        cnt++;

        //Driver interaction : Tcp command and msg
        TcpCltSession* cltSession = TcpCltSession::getInstance();
        //Robot *pRobot = Robot::getInstance();
        //std::string strResult = m_pMySQLServer->getSettingParam(driver_msg_switch, pRobot->m_nRobotID);
        //bool switch_on = atoi(strResult.c_str());
        bool switch_on = SettingService::getInstance()->m_setting.getDriverMsgSwitch();
        //printf("++++ Get switch: %d ++++\n", switch_on);
        if(switch_on) {
            for(int tcpNo=(int)TCP_MAIN_BOARD; tcpNo<=(int)TCP_ENV_BOARD_UPGRADE; tcpNo ++) {
                TcpClient * pClient = cltSession->getSession((TcpNum)tcpNo);
                while(pClient && !pClient->m_queue.empty()) {
                    std::string strMsg = "/OrbitalRobot/DriverInteraction/" + pClient->m_queue.front();
                    m_pZmqDriver->publishMsg(strMsg);
                    pClient->m_queue.pop();
                    //printf("++++[%d] MSG: %s ++++\n", tcpNo, strMsg.c_str());
                }
            }
        }
        sleep(1);
    }
    printf("--- Robot::%s() end\n", __FUNCTION__);
}


