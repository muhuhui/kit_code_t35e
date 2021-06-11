#ifndef BatteryDeviceDRIVER_H
#define BatteryDeviceDRIVER_H

#include "base/SingleTon.h"
#include "base/SerialPortBase.h"


struct Battery
{
   float    fQuantity;
   float    fVoltage;
   float    fCurrent;
   float    fTemperature;
};

/*RS485*/
class BatteryDeviceDriver : public SerialPortBase
{
public:
  ~BatteryDeviceDriver();
  static BatteryDeviceDriver *getInstance();

  int init(const char *sDevice, int nBaud = 9600, int nDataBit = 8, int nParity = 0, int nStopBit = 1);
  const Battery* getBatteryInfo();
  int setLEDContent(char cDataLen, char cAddr, char cCmd, char cIndex, char *data);
  int setBatteryQuantity(int nContextIndex, int nQuantityPercent);
  int changeChargeStatus(bool bCharged);

  static void run(BatteryDeviceDriver *pBatteryDeviceDriver);

private:
  int writeData(char *sData, int nCount);
  int readBattery();

  unsigned short crc16(unsigned char *ptr, unsigned char len);

private:
  bool          m_bThreadRun;
  Battery       m_Battery;
};

#endif // BatteryDeviceDRIVER_H
