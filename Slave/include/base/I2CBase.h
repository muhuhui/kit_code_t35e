#ifndef I2CBASE_H
#define I2CBASE_H

#include "base/SingleTon.h"
// #include <map>

class I2CBase
{
public:
  ~I2CBase();
  // static I2CBase *getInstance();
  I2CBase(const char* deviceName, int slaveAddr);
  int i2c_read(unsigned char *rbuf, int len);
  int i2c_write(unsigned char *tbuf, int len);
  int i2c_smbus_read_word(unsigned char cmd, int &nVal);
  int i2c_smbus_write_word(unsigned char cmd, int nVal);
  int i2c_init();

  // int initMonitor(int *Addr, int nCnt);
  // int getValue(int nAddr);
  // void setValue(int nAddr, int nValue);

  // static void run(I2CBase *pI2CBase);
  // int us_readVal(int nAddr, int &nVal);

private:
  int openDevice();
  void closeDevice();
  
  //int readVal(int nAddr, int &nVal);
  

private:
  // bool                    m_bInit = false;
  // bool                    m_bThreadRun;
  // bool                    m_bSet;

  // int                     m_nAddr;
  // int                     m_nValue;
  int                     m_nFd;

  // std::map<int, int>      m_AddrMap;   

  char*               m_nDeviceName;
  int                     m_nSlaveAddr;
};

#endif // I2CBASE_H
