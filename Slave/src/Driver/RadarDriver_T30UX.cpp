#include "Driver/RadarDriver_T30UX.h"
#include "Driver/MotorDriver.h"
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <algorithm>


RadarDriver_T30UX::~RadarDriver_T30UX()
{
  delete m_pI2CBase;
}

int RadarDriver_T30UX::init(char* i2cDevName, char i2cAddr)
{
  int nRet = 0;

  m_pI2CBase = new I2CBase(i2cDevName, i2cAddr);

  nRet = m_pI2CBase->i2c_init();
  if(nRet == -1){
    printf("fail to initiate i2c device\n");
    return -1;
  }
  return nRet;
}

int RadarDriver_T30UX::us_readVal(char addr, unsigned short &val)
{
  unsigned char tbuf1[3];
  unsigned char tbuf2[1];

  tbuf1[0] = 0x01;      //config register
  tbuf1[1] = ((addr & 0x07) << 4) | 0x82;      //continuous mode
  tbuf1[2] = 0x83;      //128sps
  tbuf2[0] = 0x00;      //conversion register

  int volatile ret;
  unsigned char rbuf[256] = {0};

  ret = m_pI2CBase->i2c_write(tbuf1, sizeof(tbuf1));
  if (ret != 0)
  {
    printf("i2c write failed----1\n");
    return -1;
  }

  ret = m_pI2CBase->i2c_write(tbuf2, sizeof(tbuf2));
  if (ret != 0)
  {
    printf("i2c write failed----2\n");
    return -1;
  }
  usleep(20000);
  //从指定寄存器读取数据
  ret = m_pI2CBase->i2c_read(rbuf, sizeof(rbuf)); //从REG中读取数据，存放在rbuf中
  if (ret != 0)
  {
    printf("read failed\n");
    return -1;
  }
  if(rbuf[2] == 255) val = rbuf[0]<<8 | rbuf[1];
  else { printf("rbuf read failed\n"); };

  val = 1000 - (4.096*val/32767)*(900/2.5); //Negative mode,换算成长度，单位mm
  return 0;
}


