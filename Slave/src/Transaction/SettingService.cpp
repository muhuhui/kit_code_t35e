#include "Transaction/SettingService.h"
#include <string>
#include <stdlib.h>
#include "Driver/MyLog4CXX.h"

typedef Poco::Tuple<std::string, std::string> TupleChargepara;
const std::string LOG_TAG("root.SettingService::");

SettingService::~SettingService()
{

}

//构造函数
SettingService::SettingService()
{
	//init();
}

SettingService *SettingService::getInstance()
{
  return SingleTon<SettingService>::GetInstance();
}

int SettingService::init()
{
  m_pPocoBase = MySQLDriver::getInstance();
  return 0;
}

std::string SettingService::getSettingParam(std::string strName, int nRobotID)
{
  std::string strRet = "0";
  std::string logname = LOG_TAG + std::string(__FUNCTION__);
  
  auto logger = initLogger(logname);
  try {

    std::vector<std::string> vstrValues;
    char pSQL[256];
    snprintf(pSQL, sizeof(pSQL), "SELECT parameter_value FROM parameters as a left JOIN parameter_config as b \
             on b.config_id = a.config_id where robot_id = %d and parameter_name ='%s'", \
        nRobotID, strName.c_str());

    Session ses = m_pPocoBase->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement<< pSQL, into(vstrValues), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "SettingService::getSettingParam " << exc.displayText() << std::endl;
      }
    }
    ses.close();

    if (vstrValues.size() > 0)
      strRet = vstrValues[0];
    else
      strRet = "0";

  } catch(Poco::Exception& exc) {

    std::cerr << "SettingService::getSettingParam " << exc.displayText() << std::endl;
    LOG4CPLUS_ERROR(logger, " Exception occur ");
  }

  return strRet;
}

int SettingService::querySettingByRobotId(int robotId)
{
    auto logger = initLogger(LOG_TAG + std::string(__FUNCTION__));
    printf("%s(): enter.\n", __FUNCTION__);
    LOG4CPLUS_INFO(logger, " enter. " );

    std::string strResult = "";

    //升降臂回正开关
    strResult = getSettingParam(arm_lift_switch, robotId);
    bool armliftSwitch = atoi(strResult.c_str());
    m_setting.setArmLiftMoving(armliftSwitch);
    printf("%s(), get arm lift switch: %d\n", __FUNCTION__, m_setting.getArmLiftMoving());
    LOG4CPLUS_INFO(logger, " get arm lift switch " << m_setting.getArmLiftMoving()); 

    //云台休眠开关
    strResult = getSettingParam(ptz_sleep_switch, robotId);
    bool ptzSleepSwitch = atoi(strResult.c_str());
    m_setting.setPtzSleepSwitch(ptzSleepSwitch);
    printf("%s(), get ptz sleep switch: %d\n", __FUNCTION__, m_setting.getPtzSleepSwitch());
    LOG4CPLUS_INFO(logger, " get ptz switch " << m_setting.getPtzSleepSwitch());  

    //前超声波软开关
    strResult = getSettingParam(ulsound_switch, robotId);
    bool ulsoundSwitch = atoi(strResult.c_str());
    m_setting.setUlsoundSwitch(ulsoundSwitch);
    printf("%s(), get ulsoundswitch switch: %d\n", __FUNCTION__, m_setting.getUlsoundSwitch());
    LOG4CPLUS_INFO(logger, " get ulsoundswitch switch " << m_setting.getUlsoundSwitch());

    //后超声波软开关
    strResult = getSettingParam(ulsound_switch_back, robotId);
    ulsoundSwitch = atoi(strResult.c_str());
    m_setting.setBackUlsoundSwitch(ulsoundSwitch);
    printf("%s(), get back ulsoundswitch switch: %d\n", __FUNCTION__, m_setting.getBackUlsoundSwitch());
    LOG4CPLUS_INFO(logger, " get back ulsoundswitch switch " << m_setting.getBackUlsoundSwitch());

    //下超声波软开关
    strResult = getSettingParam(ulsound_switch_down, robotId);
    ulsoundSwitch = atoi(strResult.c_str());
    m_setting.setDownUlsoundSwitch(ulsoundSwitch);
    printf("%s(), get down ulsoundswitch switch: %d\n", __FUNCTION__, m_setting.getDownUlsoundSwitch());
    LOG4CPLUS_INFO(logger, " get down ulsoundswitch switch " << m_setting.getDownUlsoundSwitch());

    //移动时云台转向开关
    strResult = getSettingParam(ptz_switch, robotId);
    bool ptzSwitch = atoi(strResult.c_str());
    m_setting.setPtzSwitch(ptzSwitch);
    printf("%s(), get ptz switch: %d\n", __FUNCTION__, m_setting.getPtzSwitch());
    LOG4CPLUS_INFO(logger, " get ptz switch: " << m_setting.getPtzSwitch());

    //超声波电源开关
    strResult = getSettingParam(radar_switch, robotId);
    bool radarSwitch = atoi(strResult.c_str());
    m_setting.setRadarSwitch(radarSwitch);
    printf("%s(), get radar switch: %d\n", __FUNCTION__, m_setting.getRadarSwitch());
    LOG4CPLUS_INFO(logger, " get radar switch: " << m_setting.getRadarSwitch());

    //前超声波侦测距离
    strResult = getSettingParam(ul_sound_threshold, robotId);
    int ULSound = atoi(strResult.c_str());
    m_setting.setULSoundThreshold(ULSound);
    printf("%s(), get ulsound threshold: %d\n", __FUNCTION__, m_setting.getULSoundThreshold());
    LOG4CPLUS_INFO(logger, " get ulsound threshold: " << m_setting.getULSoundThreshold());

    //后超声波侦测距离
    strResult = getSettingParam(back_ul_sound_threshold, robotId);
    int backULSound = atoi(strResult.c_str());
    m_setting.setbackULSoundThreshold(backULSound);
    printf("%s(), get back ulsound threshold: %d\n", __FUNCTION__, m_setting.getbackULSoundThreshold());
    LOG4CPLUS_INFO(logger, " get back ulsound threshold: " << m_setting.getbackULSoundThreshold());

    //下超声波侦测距离
    strResult = getSettingParam(down_ul_sound_threshold, robotId);
    int downULSound = atoi(strResult.c_str());
    m_setting.setdownULSoundThreshold(downULSound);
    printf("%s(), get down ulsound threshold: %d\n", __FUNCTION__, m_setting.getdownULSoundThreshold());
    LOG4CPLUS_INFO(logger, " get down ulsound threshold: " << m_setting.getdownULSoundThreshold());

    //起点
    strResult = getSettingParam(begin_pos, robotId);
    float beginPos = atof(strResult.c_str());
    m_setting.setBeginPos(beginPos);
    printf("%s(), get begin pos: %f\n", __FUNCTION__, m_setting.getBeginPos());
    LOG4CPLUS_INFO(logger, " get begin pos: " << m_setting.getBeginPos());
    
    //终点
    strResult = getSettingParam(end_pos, robotId);
    float endPos = atof(strResult.c_str());
    m_setting.setEndPos(endPos);
    printf("%s(), get end pos: %f\n", __FUNCTION__, m_setting.getEndPos());
    LOG4CPLUS_INFO(logger, " get end pos: " << m_setting.getEndPos());

    //Arm起点
    strResult = getSettingParam(arm_begin_pos, robotId);
    float armbeginPos = atof(strResult.c_str());
    m_setting.setArmBeginPos(armbeginPos);
    printf("%s(), get Arm begin pos: %f\n", __FUNCTION__, m_setting.getArmBeginPos());
    LOG4CPLUS_INFO(logger, " get Arm begin pos: " << m_setting.getArmBeginPos());
    
    //Arm终点
    strResult = getSettingParam(arm_end_pos, robotId);
    float armendPos = atof(strResult.c_str());
    m_setting.setArmEndPos(armendPos);
    printf("%s(), get Arm end pos: %f\n", __FUNCTION__, m_setting.getArmEndPos());
    LOG4CPLUS_INFO(logger, " get Arm end pos: " << m_setting.getArmEndPos());

    //默认速度
    strResult = getSettingParam(default_speed, robotId);
    float defaultSpeed = atof(strResult.c_str());
    printf("%s(), get default speed: %f\n", __FUNCTION__, defaultSpeed);
    LOG4CPLUS_INFO(logger, " get default speed: " << defaultSpeed);

    if(defaultSpeed <= 10) { defaultSpeed = 10; }
    else if(defaultSpeed >= 2000) { defaultSpeed = 2000; }
    m_setting.setDefaultSpeed(defaultSpeed);
    printf("%s(), set default speed: %f\n", __FUNCTION__, defaultSpeed);
    LOG4CPLUS_INFO(logger, " set default speed: " << defaultSpeed);

    //加速度
    strResult = getSettingParam(acceleration, robotId);
    float acc = atof(strResult.c_str());
    if (acc > 0) {
        m_setting.setAcc(acc);
        printf("%s(), set acceleration: %f\n", __FUNCTION__, acc);
        LOG4CPLUS_INFO(logger, " set acceleration: " << acc);
    }
    else {
        printf("%s(), get acceleration failed!\n", __FUNCTION__);
        LOG4CPLUS_INFO(logger, " get acceleration failed! ");

    }

    //减速度
    strResult = getSettingParam(deceleration, robotId);
    float dec = atof(strResult.c_str());
    if (dec > 0) {
        m_setting.setDec(dec);
        printf("%s(), set deceleration: %f\n", __FUNCTION__, dec);
        LOG4CPLUS_INFO(logger, " set deceleration: " << dec);
    }
    else {
        printf("%s(), get deceleration failed!\n", __FUNCTION__);
        LOG4CPLUS_INFO(logger, " get deceleration failed! ");
    }

    //运行模式
    strResult = getSettingParam(run_mode, robotId);
    int runMode = atoi(strResult.c_str());
    printf("%s(), get run mode: %d\n", __FUNCTION__, runMode);
    LOG4CPLUS_INFO(logger, " get run mode " << runMode);

    m_setting.setRunMode((RunMode)runMode);

    //锁定
    strResult = getSettingParam(motor_lock, robotId);
    bool isMotorLocked = atoi(strResult.c_str());
    m_setting.setMotorLock(isMotorLocked);
    printf("%s(), get motor lock status: %d\n", __FUNCTION__, isMotorLocked);
    LOG4CPLUS_INFO(logger, " get motor lock status: " << isMotorLocked);

    //回充电量
    strResult = getSettingParam(battery_alarm_quantity, robotId);
    float quantityMinLimit = atof(strResult.c_str());
    printf("%s(), get min quantity limit: %f%%\n", __FUNCTION__, quantityMinLimit);
    LOG4CPLUS_INFO(logger, " get min quantity limit: " << quantityMinLimit << "%%");

    if( quantityMinLimit < 10 ) { quantityMinLimit = 10; }
    else if( quantityMinLimit > 90 ) { quantityMinLimit = 90; }
    m_setting.setBatteryQuantityMin(quantityMinLimit);
    printf("%s(), set min quantity limit: %f%%\n", __FUNCTION__, quantityMinLimit);
    LOG4CPLUS_INFO(logger, " set min quantity limit: " << quantityMinLimit << "%%");

    //可见光和热像仪
    strResult = getSettingParam(camera_ip, robotId);
    m_setting.setIndustrialCameraIp(strResult);
    printf("%s(), get industial camera ip: %s\n", __FUNCTION__, strResult.c_str());
    LOG4CPLUS_INFO(logger, " get industial camera ip: " << strResult.c_str());

    strResult = getSettingParam(camera_tcp_port, robotId);
    int cameraPort = atoi(strResult.c_str());
    m_setting.setIndustrialCameraPort(cameraPort);
    printf("%s(), get industial camera port: %d\n", __FUNCTION__, cameraPort);
    LOG4CPLUS_INFO(logger, " get industial camera port: " << cameraPort);

    strResult = getSettingParam(thermal_imager_ip, robotId);
    m_setting.setThermalImagerIp(strResult);
    printf("%s(), get industial thermal ip: %s\n", __FUNCTION__, strResult.c_str());
    LOG4CPLUS_INFO(logger, " get industial thermal ip: " << strResult.c_str());

    strResult = getSettingParam(thermal_imager_tcp_port, robotId);
    int thermalPort = atoi(strResult.c_str());
    m_setting.setThermalImagerPort(thermalPort);	
    printf("%s(), get industial thermal port: %d\n", __FUNCTION__, thermalPort);
    LOG4CPLUS_INFO(logger, " get industial thermal port: " << thermalPort);

    strResult = getSettingParam(camera_user_name, robotId);
    m_setting.setCameraUserName(strResult);	
    printf("%s(), get camera user name: %s\n", __FUNCTION__, strResult.c_str());
    LOG4CPLUS_INFO(logger, " get camera user name: " << strResult.c_str());

    strResult = getSettingParam(camera_password, robotId);
    m_setting.setCameraPassword(strResult);	
    printf("%s(), get camera password: %s\n", __FUNCTION__, strResult.c_str());
    LOG4CPLUS_INFO(logger, " get camera password: " << strResult.c_str());

    //服务器图片和录像保存路径
    strResult = getSettingParam(picture_path, robotId);
    m_setting.setPicturePath(strResult);	
    printf("%s(), get picture path: %s\n", __FUNCTION__, strResult.c_str());
    LOG4CPLUS_INFO(logger, " get picture path: " << strResult.c_str());


    strResult = getSettingParam(video_path, robotId);
    m_setting.setVideoPath(strResult);	
    printf("%s(), get video path: %s\n", __FUNCTION__, strResult.c_str());
    LOG4CPLUS_INFO(logger, " get video path: " << strResult.c_str());

    //行走声光告警
    strResult = getSettingParam(move_warn, robotId);
    int moveWarn = atoi(strResult.c_str());
    m_setting.setMoveWarn(moveWarn);
    printf("%s(), get move warn: %s\n", __FUNCTION__, strResult.c_str());
    LOG4CPLUS_INFO(logger, " get move warn: " << strResult.c_str());

    //升降臂收起限制开关
    strResult = getSettingParam(arm_restrict_switch, robotId);
    bool liftSwitch = atoi(strResult.c_str());
    m_setting.setLiftRestrictSwitch(liftSwitch);
    printf("%s(), get lift restrict switch: %d\n", __FUNCTION__, liftSwitch);
    LOG4CPLUS_INFO(logger, " get lift restrict switch: " << liftSwitch);

    //巡检前归零
    strResult = getSettingParam(home_switch, robotId);
    bool homeSwitch = atoi(strResult.c_str());
    m_setting.setHomeSwitch(homeSwitch);
    printf("%s(), get home restrict switch: %d\n", __FUNCTION__, homeSwitch);
    LOG4CPLUS_INFO(logger, " get home restrict switch: " << homeSwitch);
    return 0;
}

