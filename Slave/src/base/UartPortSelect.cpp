#include "base/UartPortSelect.h"

UartPortSelect::UartPortSelect()
{
    m_uart.clear();
}

UartPortSelect::~UartPortSelect()
{

}

UartPortSelect *UartPortSelect::getInstance()
{
    return SingleTon<UartPortSelect>::GetInstance();
}

int UartPortSelect::init(const char *sDevice, int nBaud, int nDataBit, int nParity, int nStopBit)
{
    std::string devName(sDevice);
    int nRet = 0;
    //一个串口只初始化一次
    if (m_uart.find(devName) != m_uart.end()) {
        printf("UartPortSelect::%s(), uart already initialized, exit.\n", __FUNCTION__);
        return 0;
    }

    SPtr<UartPort> pUartPort = std::make_shared<UartPort>();
    pUartPort->pUart = std::make_shared<SerialPortBase>();
    pUartPort->pModbus = std::make_shared<ModbusBase>();

    // printf("uart base=================%s\n", sDevice);

    nRet = pUartPort->pUart->open(sDevice, nBaud, nDataBit, nParity, nStopBit);
    if (nRet != SerialPortBase::OK) {
        printf("UartPortSelect::%s(), fail to initiate uart, device %s, nRet: %d\n", __FUNCTION__, sDevice, nRet);
        return -1;
    }
    
    char cParity = (!nParity) ? 'N' : (nParity == 1) ? 'O' : (nParity == 2) ? 'E' : 'N';
    nRet = pUartPort->pModbus->connect(MODBUS_RTU_RS485, sDevice, nBaud, cParity, nDataBit, nStopBit);
    if (nRet != 0) {
        printf("UartPortSelect::%s(), fail to initiate modbus, device %s, nRet: %d\n", __FUNCTION__, sDevice, nRet);
        return -1;
    }

    m_uart.insert(std::pair<std::string, SPtr<UartPort>>(devName, pUartPort));
    return 0;
}

int UartPortSelect::queryModbusFrame(std::string sDevice, int nSlaveID, int nAddr, int nNb, uint16_t *pDest)
{
    if (m_uart.find(sDevice) == m_uart.end()) {
        printf("UartPortSelect::%s(), %s does not exist!\n", __FUNCTION__, sDevice.c_str());
        return -1;
    }
    SPtr<UartPort> pUartPort = m_uart.at(sDevice);
    
    if (nullptr != pUartPort->pModbus)
    {
        std::lock_guard<std::mutex> lckGuard(pUartPort->lck);
        return pUartPort->pModbus->readRegisters(MODBUS_RTU, nSlaveID, nAddr, nNb, pDest);
    } 
    else {
        return -1;
    }
    return 0;
}

int UartPortSelect::writeModbusFrame_Rs(std::string sDevice, int nSlaveID, int nAddr, int nNb, uint16_t *pDest)
{
    if (m_uart.find(sDevice) == m_uart.end()) {
        printf("UartPortSelect::%s(), %s does not exist!\n", __FUNCTION__, sDevice.c_str());
        return -1;
    }
    SPtr<UartPort> pUartPort = m_uart.at(sDevice);
    
    if (nullptr != pUartPort->pModbus)
    {
        std::lock_guard<std::mutex> lckGuard(pUartPort->lck);
        return pUartPort->pModbus->writeRegisters(MODBUS_RTU,nSlaveID, nAddr, nNb, pDest);
        // return pUartPort->pModbus->readRegisters(MODBUS_RTU, nSlaveID, nAddr, nNb, pDest);
    } 
    else {
        return -1;
    }
    return 0;
}

int UartPortSelect::writeModbusFrame(std::string sDevice, int nSlaveID, int nAddr, int nNb, int value)
{
    if (m_uart.find(sDevice) == m_uart.end()) {
        printf("UartPortSelect::%s(), %s does not exist!\n", __FUNCTION__, sDevice.c_str());
        return -1;
    }
    SPtr<UartPort> pUartPort = m_uart.at(sDevice);
    
    if (nullptr != pUartPort->pModbus)
    {
        std::lock_guard<std::mutex> lckGuard(pUartPort->lck);
        return pUartPort->pModbus->writeRegister(MODBUS_RTU, nSlaveID, nAddr, value);
        // return pUartPort->pModbus->readRegisters(MODBUS_RTU, nSlaveID, nAddr, nNb, pDest);
    } 
    else {
        return -1;
    }
    return 0;
}

int UartPortSelect::queryModbusFrame_IR(std::string sDevice, int nSlaveID, int nAddr, int nNb, uint16_t *pDest)
{
    if (m_uart.find(sDevice) == m_uart.end()) {
        printf("UartPortSelect::%s(), %s does not exist!\n", __FUNCTION__, sDevice.c_str());
        return -1;
    }
    SPtr<UartPort> pUartPort = m_uart.at(sDevice);
    
    if (nullptr != pUartPort->pModbus)
    {
        std::lock_guard<std::mutex> lckGuard(pUartPort->lck);
        return pUartPort->pModbus->readInputRegisters(MODBUS_RTU, nSlaveID, nAddr, nNb, pDest);
    } 
    else {
        return -1;
    }
    return 0;
}

int UartPortSelect::queryUartFrame(std::string sDevice, char* cmd, int cmdSize, char* rcvData, int rcvDataSize, int& rcvBytes, int timeout)
{
    if (m_uart.find(sDevice) == m_uart.end()) {
        printf("UartPortSelect::%s(), %s does not exist!\n", __FUNCTION__, sDevice.c_str());
        return -1;
    }
    SPtr<UartPort> pUartPort = m_uart.at(sDevice);
    SPtr<SerialPortBase> pUart = pUartPort->pUart;
    
    if (nullptr == pUartPort->pUart) {
        printf("UartPortSelect::%s(), uart %s is null!\n", __FUNCTION__, sDevice.c_str());
        return -1;
    }

    std::lock_guard<std::mutex> lckGuard(pUartPort->lck);
    int nRet = pUart->write(cmd, cmdSize);
    if (nRet != cmdSize) {
        printf("UartPortSelect::%s(), uart %s write failed\n", __FUNCTION__, sDevice.c_str());
        return -1;
    }
    pUart->enableRead();

    usleep(40000);

    int nBytes = pUart->read(rcvData, rcvDataSize, 10);
    if (nBytes < rcvBytes) {
        printf("UartPortSelect::%s(), uart %s read failed\n", __FUNCTION__, sDevice.c_str());
        return -1;
    }
    pUart->disableRead();
    rcvBytes = nBytes;
    
    return 0;
}

SPtr<ModbusBase> UartPortSelect::getModbusBase(std::string sDevice)
{
    if (m_uart.find(sDevice) == m_uart.end()) {
        printf("UartPortSelect::%s(), no match modbus %s!", __FUNCTION__, sDevice.c_str());
        return nullptr;
    }
    return m_uart.at(sDevice)->pModbus;
}

SPtr<SerialPortBase> UartPortSelect::getUartBase(std::string sDevice)
{
    if (m_uart.find(sDevice) == m_uart.end()) {
        printf("UartPortSelect::%s(), no match modbus %s!", __FUNCTION__, sDevice.c_str());
        return nullptr;
    }
    return m_uart.at(sDevice)->pUart;
}

int UartPortSelect::sendUartCmd(std::string sDevice, char* cmd, int cmdSize, int timeout){
    if (m_uart.find(sDevice) == m_uart.end()) {
        printf("UartPortSelect::%s(), %s does not exist!\n", __FUNCTION__, sDevice.c_str());
        return -1;
    }
    SPtr<UartPort> pUartPort = m_uart.at(sDevice);
    SPtr<SerialPortBase> pUart = pUartPort->pUart;
    int nRet = 0;
    
    if (nullptr == pUartPort->pUart) {
        printf("UartPortSelect::%s(), uart %s is null!\n", __FUNCTION__, sDevice.c_str());
        return -1;
    }

    std::lock_guard<std::mutex> lckGuard(pUartPort->lck);
    nRet = pUart->write(cmd, cmdSize);
    if (nRet != cmdSize) {
        printf("UartPortSelect::%s(), uart %s write failed\n", __FUNCTION__, sDevice.c_str());
        return -1;
    }
    return 0;
}