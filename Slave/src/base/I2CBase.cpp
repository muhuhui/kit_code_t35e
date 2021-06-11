#include "base/I2CBase.h"
#include <thread>
#include <stdio.h>
#include <unistd.h>

#define MAX_SIZE  128

extern "C"
{
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <linux/i2c.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>

int I2CBase::i2c_read(unsigned char *rbuf, int len)
{
  //i2c读取数据需要发送2个i2c_msg，一个用于写入要读取的寄存器地址，另一个用于接收读取的数据
  struct i2c_rdwr_ioctl_data i2c_data;
  struct i2c_msg msgs;
  int ret = 0;

  i2c_data.nmsgs = 1;
  i2c_data.msgs = &msgs;

  msgs.flags = 1;	//读标志，实际值为0x1
  msgs.addr  = m_nSlaveAddr;	//i2c地址
  msgs.len   = len;//len;	//读入的数据长度
  msgs.buf   = rbuf;	//读入数据的存放地址
  ret =ioctl(m_nFd, I2C_RDWR, (unsigned long) &i2c_data);	//驱动接口，际发送给驱动的数据结构是 i2c_data的指针
  if (ret < 0){
    perror("ioctl");
    return ret;
  }

  return 0;
}

int I2CBase::i2c_write(unsigned char *tbuf, int len)
{
  struct i2c_rdwr_ioctl_data i2c_data;
  struct i2c_msg msgs;
  int ret = 0;

  i2c_data.nmsgs = 1;
  i2c_data.msgs = &msgs;

  msgs.flags = 0;	//写标志
  msgs.addr  = m_nSlaveAddr;	//设备i2c地址
  msgs.len   = len;		//i2c发送数据包的字节数，也就是msgs.buf的大小
  msgs.buf   = tbuf;		//i2c要发送的数据

  ret =ioctl(m_nFd, I2C_RDWR, (unsigned long) &i2c_data);	//驱动接口，实际发送给驱动的数据结构是 i2c_data的指针
  // printf("send %d bytes to %d----\n", len, m_nSlaveAddr);
  if (ret < 0){
    perror("ioctl");
    return ret;
  }

  return 0;
}
}

I2CBase::~I2CBase()
{
  // m_bThreadRun = false;
  if(m_nFd != -1) closeDevice();
  delete[] m_nDeviceName;
}

// I2CBase *I2CBase::getInstance()
// {
//   return SingleTon<I2CBase>::GetInstance();
// }

I2CBase::I2CBase(const char* deviceName, int slaveAddr)
{
  m_nDeviceName = new char[sizeof(deviceName)];
  strcpy(m_nDeviceName, deviceName);
  m_nSlaveAddr = slaveAddr;
  m_nFd = -1;
}

int I2CBase::i2c_init()
{
  int nRet = 0;
  if(m_nFd == -1) nRet = openDevice();
  return nRet;
}

int I2CBase::openDevice()
{
  printf("open I2C device %s\n", m_nDeviceName);
  int ret;
  if ((m_nFd = open(m_nDeviceName,O_RDWR))< 0) {
    printf("fail to open i2c device\n");
    return -1;
  }
  ret = ioctl(m_nFd, I2C_SLAVE_FORCE, (unsigned long)m_nSlaveAddr);
  if(ret < 0)
  {
    printf("set slaveAddr err ,ret:%d\n",ret);
    return -1;
  }
  ret = ioctl(m_nFd, I2C_TENBIT, 0);
  if(ret < 0)
  {
    printf("set time out err ,ret:%d\n",ret);
    return -1;
  }
  ret = ioctl(m_nFd, I2C_RETRIES, 2);
  if(ret < 0)
  {
    printf("set sretries err ,ret:%d\n",ret);
    return -1;
  }

  ret = ioctl(m_nFd, I2C_TIMEOUT, 10);
  if(ret < 0)
  {
    printf("set time out err ,ret:%d\n",ret);
    return -1;
  }

  printf("i2c device handle: %d\n", m_nFd);
  return 0;
}

void I2CBase::closeDevice()
{
  close(m_nFd);
}

int I2CBase::i2c_smbus_read_word(unsigned char cmd, int &nVal)
{
  struct i2c_smbus_ioctl_data i2c_msbus_data;
  union i2c_smbus_data data;
  int ret = 0;

  i2c_msbus_data.read_write = I2C_SMBUS_READ; // 读标志，实际值为0x1
  i2c_msbus_data.command = cmd; // 命令值
  i2c_msbus_data.size = I2C_SMBUS_WORD_DATA; //数据长度，实际值为2
  i2c_msbus_data.data = &data;

  ret =ioctl(m_nFd, I2C_SMBUS, &i2c_msbus_data);
  if (ret < 0){
    // perror("ioctl");
    return ret;
  }

  // nVal = data.block[2]<<8 | data.block[1];
  nVal = data.word;

  // printf("----msbus read data %02X \n", (data.block[1]<<8 | data.block[0]));
  // printf("----msbus read data %02X using cmd %02X \n", nVal, cmd);
  return 0;
}

int I2CBase::i2c_smbus_write_word(unsigned char cmd, int nVal)
{
  struct i2c_smbus_ioctl_data i2c_msbus_data;
  union i2c_smbus_data data;
  int ret = 0;

  data.word = nVal;

  i2c_msbus_data.read_write = I2C_SMBUS_WRITE; // 写标志，实际值为0x0
  i2c_msbus_data.command = cmd; // 命令值
  i2c_msbus_data.size = I2C_SMBUS_WORD_DATA; //数据长度，实际值为3
  i2c_msbus_data.data = &data;

  ret =ioctl(m_nFd, I2C_SMBUS, &i2c_msbus_data);
  // printf("------- msbus write cmd: %02X ---------\n", data.word);
  
  if (ret < 0){
    // perror("ioctl");
    return ret;
  }

  return 0;
}
