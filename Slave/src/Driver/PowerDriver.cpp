#include "Driver/PowerDriver.h"
#include "Transaction/SettingService.h"
#include "ThirdParty/jsoncpp/json.h"
#include <unistd.h>
#include <thread>
#include "Transaction/Motor.h"
#include "Driver/MyLog4CXX.h"

const std::string LOG_TAG("root.PowerDriver::");

PowerDriver::~PowerDriver()
{

}

PowerDriver *PowerDriver::getInstance()
{
  return SingleTon<PowerDriver>::GetInstance();
}

int PowerDriver::init()
{
  int addrs[1] = {BATTERY_INPUT};
  SettingService *pSettingService = SettingService::getInstance();
  m_pGPIOBase = GPIOBase::getInstance();
  m_pGPIOBase->initMonitor(addrs, 1);
  bool radarSwitch = pSettingService->m_setting.getRadarSwitch();
  switchPower(BATTERY_SWITCH, 1);
  switchPower(LED_BACK_SWITCH, 1);
  switchPower(LED_FRONT_SWITCH, 1);
  switchPower(RADAR_SWITCH, 1);
  switchPower(RFID_SWITCH, 1);
  switchPower(SENSOR_SWITCH, 1);
  switchPower(CAMERA_SWITCH, 1);
  switchPower(CHARGEPILE_SWITCH, 0);
  switchPower(BEEP_SWITCH, 0);         //关音频告警
  switchPower(BRAKE_SWITCH, 0);         //默认刹车（不上电）

  //断电开关检测线程
  /*
  std::thread runThread;
  runThread = std::thread(PowerDriver::powerOffDetectThread, this);
  runThread.detach();
  */

  //灯带告警开关
  std::thread runThread;
  runThread = std::thread(PowerDriver::ledWarnThread, this);
  runThread.detach();
}

void PowerDriver::switchPower(int nSwitch, bool bOn)
{
    m_pGPIOBase->setValue(nSwitch, bOn);
}

void PowerDriver::switchPowerRadar(bool bOn)
{
	m_pGPIOBase->setValue(RADAR_SWITCH, bOn);
}

void PowerDriver::switchPowerPtz(bool bOn)
{
	m_pGPIOBase->setValue(CAMERA_SWITCH, bOn);
}

//电池电源开关控制
void PowerDriver::switchPowerBattery(bool bOn)
{
	m_pGPIOBase->setValue(BATTERY_SWITCH, bOn);
}

//电机电源开关控制
void PowerDriver::switchPowerMotor(bool bOn)
{
	m_pGPIOBase->setValue(ACTUATOR_SWITCH, bOn);
}

//充电开关控制
void PowerDriver::switchPowerCharge(bool bOn)
{
	m_pGPIOBase->setValue(CHARGEPILE_SWITCH, bOn);
}

void PowerDriver::switchPowerBeep(bool bOn)
{
	m_pGPIOBase->setValue(BEEP_SWITCH, bOn);
}

//刹车控制
void PowerDriver::switchBrake(bool bOn)
{
	m_pGPIOBase->setValue(BRAKE_SWITCH, bOn);
}

//断电检测线程，上升沿触发断电
void PowerDriver::powerOffDetectThread(PowerDriver* pPowerDriver)
{
    std::string logname = LOG_TAG + std::string(__FUNCTION__);
    auto logger = initLogger(logname);
    static int lastOnOff = 1;       //上电默认打开
    int onOff = 1;
    printf("--- PowerDriver::%s() start\n", __FUNCTION__);
    while (1) {
       sleep(1);
       onOff = pPowerDriver->m_pGPIOBase->getValue(BATTERY_INPUT);
       if (lastOnOff != onOff && onOff) {
           printf("--- PowerDriver::%s(), start power off...\n", __FUNCTION__);
           LOG4CPLUS_INFO(logger, " start power off...");
           pPowerDriver->m_pGPIOBase->setValue(BATTERY_SWITCH, 0);
       }
       lastOnOff = onOff;
    }
    printf("--- PowerDriver::%s() end\n", __FUNCTION__);
}

void PowerDriver::ledWarnThread(PowerDriver* pPowerDriver)
{
    printf("--- PowerDriver::%s() start\n", __FUNCTION__);
    Motor* pMotor = Motor::getInstance();
    int warnDelay = 0;
    SettingService* pService = SettingService::getInstance();
    while (1) {
        if (pPowerDriver->isLedWarn()) {
            warnDelay = 500000;
        }
        else if (pMotor->getCurStatus() != STOP_STATE &&
            pService->m_setting.getMoveWarn()) {
            usleep(200000);
            if (pMotor->getCurStatus() != STOP_STATE) {
                warnDelay = 1000000;
            }
        }
        else {
            warnDelay = 0;
            sleep(1);
            continue;
        }
        pPowerDriver->switchPower(LED_BACK_SWITCH, 0);
        pPowerDriver->switchPower(LED_FRONT_SWITCH, 0);
        usleep(warnDelay);
        pPowerDriver->switchPower(LED_BACK_SWITCH, 1);
        pPowerDriver->switchPower(LED_FRONT_SWITCH, 1);
        usleep(warnDelay);
    }
    printf("--- PowerDriver::%s() end\n", __FUNCTION__);
}


