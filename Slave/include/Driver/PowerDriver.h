#ifndef POWERDRIVER_H
#define POWERDRIVER_H 
#include "base/SingleTon.h"
#include "base/GPIOBase.h"

#define CHARGEPILE_SWITCH		248         //旧板子是250
#define CHARGE_WARN_INPUT       109
#define ACTUATOR_SWITCH         16          //无
#define BATTERY_SWITCH			181
#define BATTERY_INPUT           229
#define RADAR_SWITCH            238
#define SENSOR_SWITCH           233
#define CAMERA_SWITCH           234
#define RFID_SWITCH             245
#define LED_FRONT_SWITCH        250
#define LED_BACK_SWITCH         237
#define BEEP_SWITCH             172         //0:关，1:开
#define BRAKE_SWITCH            173         //0:刹车，1:停止刹车（上电）

class PowerDriver
{
public:
  ~PowerDriver();
  static PowerDriver *getInstance();

  int init();
  void switchPower(int nSwitch, bool bOn);
  void switchPowerRadar(bool bOn);
  void switchPowerBattery(bool bOn);
  void switchPowerMotor(bool bOn);
  void switchPowerCharge(bool bOn);
  void switchPowerBeep(bool bOn);
  void switchBrake(bool bOn);
  void switchPowerPtz(bool bOn);
  static void powerOffDetectThread(PowerDriver* pPowerDriver);
  static void ledWarnThread(PowerDriver* pPowerDriver);
  void setLedWarn(bool isWarn) { m_isLedWarn = isWarn; }
  bool isLedWarn() { return m_isLedWarn; }
private:
  GPIOBase*               m_pGPIOBase;
  bool                    m_isLedWarn;
};

#endif // POWERDRIVER_H
