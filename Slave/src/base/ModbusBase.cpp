#include "base/ModbusBase.h"


ModbusBase::ModbusBase():
    m_pModbusRtu(NULL),
    m_pModbusTcp(NULL)
{

}

ModbusBase::~ModbusBase()
{
  disconnectAll();
}

int ModbusBase::connect(int nRtuMode, const char * sDevice, int nBaud, char cParity, int nDataBit, int nStopBit)
{
  if(m_pModbusRtu) {
    modbus_close(m_pModbusRtu);
    modbus_free(m_pModbusRtu);
    m_pModbusRtu = NULL;
  }

  m_pModbusRtu = modbus_new_rtu(sDevice, nBaud, cParity, nDataBit, nStopBit);
  printf("ModbusBase::%s(), sDevice: %s, nBaud: %d, cParity: %c, nDataBit: %d, nStopBit: %d\n", __FUNCTION__, sDevice, nBaud, cParity, nDataBit, nStopBit);

  if(NULL == m_pModbusRtu) {
    printf("---ModBusDrive::init RTU Err \n");
    return -1;
  }

  //调试功能，会显示收发数据, 0关闭1打开
  int nRet = 0;
  nRet = modbus_set_debug(m_pModbusRtu, 0);
  if(-1 == nRet) {
    return -2;
  }
  // 启动错误恢复机制，接收数据长度无效、TID错误或者收到的功能码与预期不符时
  // 将执行a sleep and flush sequence
  nRet = modbus_set_error_recovery(m_pModbusRtu,
//                          MODBUS_ERROR_RECOVERY_LINK |
                          MODBUS_ERROR_RECOVERY_PROTOCOL);
  if (-1 == nRet) {
    /* code */
    printf("--- ModBusDriver::modbus_set_error_recovery Err \n");
    printf("--- Recieved data may wrong and can't recover. \n");
  }
  
  //设置超时1s
  struct timeval tv;
  tv.tv_sec = 0; 
  tv.tv_usec = 500000;
  nRet = modbus_set_response_timeout(m_pModbusRtu, tv.tv_sec, tv.tv_usec);
  if( -1 == nRet) {
    printf("--- ModBusDriver::timeout RTU Err \n");
    return -3;
  }

  nRet = modbus_connect(m_pModbusRtu);
  if( -1 == nRet) {
    printf("--- ModBusDriver::connect RTU Err \n");
    return -4;
  }

  return 0;
}

int ModbusBase::connect(const char *sIp, int nPort)
{
  if(m_pModbusTcp) {

    modbus_close(m_pModbusTcp);
    modbus_free(m_pModbusTcp);
    m_pModbusTcp = NULL;
  }

  m_pModbusTcp = modbus_new_tcp(sIp, nPort);
  if(NULL == m_pModbusTcp) {
    printf("ModbusBase::init TCP Err \n");
    return -1;
  }

  struct timeval tv;
  tv.tv_sec = 1; 
  tv.tv_usec = 0;
  modbus_set_response_timeout(m_pModbusTcp, tv.tv_sec, tv.tv_usec);

  int nRet = modbus_connect(m_pModbusTcp);
  if( -1 == nRet ) {
    printf("ModbusBase::connect TCP Err \n");
  }

  return 0;
}

int ModbusBase::disconnectAll()
{
  if(m_pModbusRtu) {

    modbus_close(m_pModbusRtu);
    modbus_free(m_pModbusRtu);
    m_pModbusRtu = NULL;
  }

  if(m_pModbusTcp) {

    modbus_close(m_pModbusTcp);
    modbus_free(m_pModbusTcp);
    m_pModbusTcp = NULL;
  }
  return 0;
}

int ModbusBase::readRegisters(ModbusType eRtuOrTcp, int nSlaveID, int nAddr, int nNb, uint16_t *pDest)
{
  int nRet = 0;
  modbus_t *pCtx = NULL;

  if(MODBUS_RTU == eRtuOrTcp) {
    pCtx = m_pModbusRtu;
  } else if(MODBUS_TCP == eRtuOrTcp) {
    pCtx = m_pModbusTcp;
  }

  if(pCtx) {

    nRet = modbus_set_slave(pCtx, nSlaveID);
    if(-1 == nRet) {
      printf("--- ModBusDriver::readRegister slaveID:%d Error \n", nSlaveID);
      return -1;
    }

    nRet = modbus_read_registers(pCtx, nAddr, nNb, pDest);  //仅在此处有通信
    if(-1 == nRet) {
      printf("--- ModBusDriver::readRegister slaveID:%d, nAddr:%d Error\n", nSlaveID, nAddr);
      return -2;
    }
  }

  return 0;
}

int ModbusBase::readBits(ModbusType eRtuOrTcp, int nSlaveID, int nAddr, int nNb, uint8_t *pDest)
{
  int nRet = 0;
  modbus_t *pCtx = NULL;

  if(MODBUS_RTU == eRtuOrTcp) {
    pCtx = m_pModbusRtu;
  } else if(MODBUS_TCP == eRtuOrTcp) {
    pCtx = m_pModbusTcp;
  }

  if(pCtx) {

    nRet = modbus_set_slave(pCtx, nSlaveID);
    if(-1 == nRet) {
      printf("--- ModBusDriver::readBits nSlaveID:%d Error \n", nSlaveID);
      return -1;
    }

    nRet = modbus_read_input_bits(pCtx, nAddr, nNb, pDest);
    if(-1 == nRet) {
      printf("--- ModBusDriver::readBits nSlaveID:%d nAddr:%d Error \n", nSlaveID, nAddr);
      return -2;
    }
  }

  return 0;
}

int ModbusBase::writeRegister(ModbusType eRtuOrTcp, int nSlaveID, int nAddr, int nValue)
{
  int nRet = 0;
  modbus_t *pCtx = NULL;

  if(MODBUS_RTU == eRtuOrTcp) {
    pCtx = m_pModbusRtu;
  } else if(MODBUS_TCP == eRtuOrTcp) {
    pCtx = m_pModbusTcp;
  }

  if(pCtx) {

    nRet = modbus_set_slave(pCtx, nSlaveID);
    if(-1 == nRet) {
      printf("--- ModBusDriver::writeRegister nSlaveID:%d Error \n", nSlaveID);
      return -1;
    }

    nRet = modbus_write_register(pCtx, nAddr, nValue);
    if(-1 == nRet) {
      printf("--- ModBusDriver::writeRegister nSlaveID:%d nAddr:%d Error \n", nSlaveID, nAddr);
      return -2;
    }
  }
  return 0;
}

int ModbusBase::writeRegisters(ModbusType eRtuOrTcp, int nSlaveID, int nAddr, int nNb, const uint16_t *pSrc)
{
  int nRet = 0;
  modbus_t *pCtx = NULL;

  if(MODBUS_RTU == eRtuOrTcp) {
    pCtx = m_pModbusRtu;
  } else if(MODBUS_TCP == eRtuOrTcp) {
    pCtx = m_pModbusTcp;
  }

  if(pCtx) {

    nRet = modbus_set_slave(pCtx, nSlaveID);
    if(-1 == nRet) {
      printf("--- ModBusDriver::writeRegister nSlaveID:%d Error \n", nSlaveID);
      return -1;
    }

    nRet = modbus_write_registers(pCtx, nAddr, nNb, pSrc);
    if(-1 == nRet) {
      printf("--- ModBusDriver::writeRegister nSlaveID:%d nAddr:%d Error \n", nSlaveID, nAddr);
      return -2;
    }
  }

  return 0;
}

int ModbusBase::readInputRegisters(ModbusType eRtuOrTcp, int nSlaveID, int nAddr, int nNb, uint16_t *pDest){
  int nRet = 0;
  modbus_t *pCtx = NULL;

  if(MODBUS_RTU == eRtuOrTcp) {
    pCtx = m_pModbusRtu;
  } else if(MODBUS_TCP == eRtuOrTcp) {
    pCtx = m_pModbusTcp;
  }

  if(pCtx) {

    nRet = modbus_set_slave(pCtx, nSlaveID);
    if(-1 == nRet) {
      printf("--- ModBusDriver::readInputRegisters nSlaveID:%d Error \n", nSlaveID);
      return -1;
    }

    nRet = modbus_read_input_registers(pCtx, nAddr, nNb, pDest);
    if(-1 == nRet) {
      printf("--- ModBusDriver::readInputRegisters nSlaveID:%d nAddr:%d Error \n", nSlaveID, nAddr);
      return -2;
    }
  }
  return 0;
}
