#ifndef MODBUSBASE_H
#define MODBUSBASE_H

#include <stdio.h>
#include "ThirdParty/modbus/modbus.h"


enum ModbusType {
  MODBUS_RTU = 0,
  MODBUS_TCP
};

class ModbusBase
{
public:
  ModbusBase();
  ~ModbusBase();

  /* nRtuMode: MODBUS_RTU_RS485, or MODBUS_RTU_RS232 */
  int connect(int nRtuMode, const char * sDevice, int nBaud, char cParity, int nDataBit, int nStopBit);
  int connect(const char *sIp, int nPort);
  int disconnectAll();

  /* FUN 0x03 (read holding registers) */
  int readRegisters(ModbusType eRtuOrTcp, int nSlaveID, int nAddr, int nNb, uint16_t *pDest);
  /* FUN 0x02 (read input status) */
  int readBits(ModbusType eRtuOrTcp, int nSlaveID, int nAddr, int nNb, uint8_t *pDest);
  /* FUN 0x04 (read input registers) */
  int readInputRegisters(ModbusType eRtuOrTcp, int nSlaveID, int nAddr, int nNb, uint16_t *pDest);

  /* FUN 0x06 (preset single register) */
  int writeRegister(ModbusType eRtuOrTcp, int nSlaveID, int nAddr, int nValue);
  /* FUN 0x10 (perset multiple registers) */
  int writeRegisters(ModbusType eRtuOrTcp, int nSlaveID, int nAddr, int nNb, const uint16_t *pSrc);

private:
  modbus_t*         m_pModbusRtu;
  modbus_t*         m_pModbusTcp;
};

#endif // MODBUSBASE_H
