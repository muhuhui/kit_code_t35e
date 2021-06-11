#ifndef UartPortSelect_H
#define UartPortSelect_H

#include "base/ModbusBase.h"
#include "base/SerialPortBase.h"
#include "base/SingleTon.h"
#include <string.h>
#include <unistd.h>
#include <memory>
#include <mutex>
#include <map>

typedef struct {
    std::mutex lck;
    std::shared_ptr<ModbusBase> pModbus;
    std::shared_ptr<SerialPortBase> pUart;
} UartPort;

template <typename T> using SPtr = std::shared_ptr<T>;

class UartPortSelect
{
public:
    UartPortSelect();
    ~UartPortSelect();
    static UartPortSelect *getInstance();

    int init(const char *sDevice, int nBaud, int nDataBit, int nParity, int nStopBit);
    int queryModbusFrame(std::string sDevice, int nSlaveID, int nAddr, int nNb, uint16_t *pDest);
    int queryModbusFrame_IR(std::string sDevice, int nSlaveID, int nAddr, int nNb, uint16_t *pDest);
    int writeModbusFrame(std::string sDevice, int nSlaveID, int nAddr, int nNb, int value);
    int writeModbusFrame_Rs(std::string sDevice, int nSlaveID, int nAddr, int nNb, uint16_t *pDest);
    int queryUartFrame(std::string sDevice, char* cmd, int cmdSize, char* rcvData, int rcvDataSize, int& rcvBytes, int timeout=200);
    int sendUartCmd(std::string sDevice, char* cmd, int cmdSize, int timeout=200);
    SPtr<ModbusBase> getModbusBase(std::string sDevice);
    SPtr<SerialPortBase> getUartBase(std::string sDevice);

private:
    std::map<std::string, SPtr<UartPort>> m_uart;
};

#endif
