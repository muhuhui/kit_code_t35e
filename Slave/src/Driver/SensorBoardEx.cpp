#include "SensorBoardEx.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>

#include "Driver/MyLog4CXX.h"

const std::string LOG_TAG("root.SensorBoardEx::");

SensorBoardEx::SensorBoardEx(/* args */) 
: m_pUartSel(nullptr)
, isExtension(false)
, m_bThreadCanRun(false)
// , m_pDest(nullptr)
, m_Gas({0.})
, m_Env({0.})
, m_Pllt({0.})
{
}

SensorBoardEx::SensorBoardEx(bool hasEx) 
: m_pUartSel(nullptr)
, isExtension(false)
, m_bThreadCanRun(false)
// , m_pDest(nullptr)
, m_Gas({0.})
, m_Env({0.})
, m_Pllt({0.})
{
    activateExtension(hasEx);
}

SensorBoardEx::~SensorBoardEx()
{
    // if (nullptr != m_pDest)
    // {
    //     delete [] m_pDest;
    // }
    
}
SensorBoardEx::Ptr 
SensorBoardEx::getInstance()
{
    //TODO: Singleton
    // static SensorBoardEx instance;
    static SensorBoardEx::Ptr p(new SensorBoardEx);
    return p;
}

bool 
SensorBoardEx::activateExtension(bool setEx)
{
    std::unique_lock<std::mutex> lck(m_mutex);
    isExtension = setEx;
    return true;
}

int 
SensorBoardEx::setDataInterface(UartPortSelect* pUartSel)
{
    std::unique_lock<std::mutex> lck(m_mutex);
    m_pUartSel = pUartSel;
    if (nullptr != m_pUartSel)
    {
        m_bThreadCanRun = true;
        return 0;
    } else {
        return -1;
    }
}

const SensorBoardEx::GasInfo& 
SensorBoardEx::getGasInfo(void)
{
    std::unique_lock<std::mutex> lck(m_mutex);
    return m_Gas;
}

const SensorBoardEx::EnvInfo& 
SensorBoardEx::getEnvInfo(void)
{
    std::unique_lock<std::mutex> lck(m_mutex);
    return m_Env;
}

const SensorBoardEx::PlltInfo& 
SensorBoardEx::getPlltInfo(void)
{
    std::unique_lock<std::mutex> lck(m_mutex);
    return m_Pllt;
}

void 
SensorBoardEx::setGasInfo(GasInfo gas)
{
    std::unique_lock<std::mutex> lck(m_mutex);
    m_Gas = gas;
}

void 
SensorBoardEx::setEnvInfo(EnvInfo env)
{
    std::unique_lock<std::mutex> lck(m_mutex);
    m_Env = env;
}

void 
SensorBoardEx::setPlltInfo(PlltInfo pllt)
{
    std::unique_lock<std::mutex> lck(m_mutex);
    m_Pllt = pllt;
}

int 
SensorBoardEx::readSensorInfo(void)
{
    auto logger = initLogger(LOG_TAG + std::string(__FUNCTION__));
    int nNb = ( isExtension ) ? 0x001C/2 : 0x0012/2;
    uint16_t pDest[256]={0}; //Modbus max data len = 250
    int nRet = m_pUartSel->queryModbusFrame(m_strDevice, 1, 0x0380, nNb, pDest);

    /* Debug: check modbus recieved data */
    // auto printLog = [logger](uint16_t _dst){
    //     std::cout << " " << std::hex << _dst;
    //     LOG4CPLUS_DEBUG(logger, " " << _dst);
    //     };
    // std::cout << "SensorBoardEx::readSensorInfo output: "<< std::endl;
    // std::for_each(pDest, pDest + nNb, printLog);
    // std::cout << std::endl;

    if (0 == nRet) {
        GasInfo _gas;
        EnvInfo _env;
        _gas.fCH4 = pDest[0] / 10.0;
        _gas.fO2 = pDest[1] / 10.0;
        _gas.fCO = pDest[2] / 1.0;
        _gas.fSO2 = pDest[3] / 1.0;
        _gas.fSmog = pDest[4] / 1.0;
        setGasInfo(_gas);
        _env.fTemperature = pDest[5] / 10.0;
        _env.fHumidity = pDest[6] / 10.0;
        _env.fIlluminance = (float)(pDest[7] << 16 | pDest[8]) / 1.0;
        setEnvInfo(_env);
        PlltInfo _pllt{0.0,0.0,0.0,0.0};
        if ( isExtension ) {
            _pllt.fPm1_0 = pDest[9] /1.0;
            _pllt.fPm2_5 = pDest[10] /1.0;
            _pllt.fPm10 = pDest[11] /1.0;
            _pllt.fSF6 = pDest[12] /1.0;
            _gas.fCO2 = pDest[13] /1.0;
        }
        setPlltInfo(_pllt);
        return 0;
    } else {
        return nRet;
    }
}


void 
SensorBoardEx::run(SensorBoardEx::Ptr pSensorBoardEx)
{
    std::cout << "--- SensorBoardEx run Start... " << std::endl;
    while (pSensorBoardEx->m_bThreadCanRun) {
        if ( 0 != pSensorBoardEx->readSensorInfo() ) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        std::this_thread::sleep_for(std::chrono::seconds(20));
    }
    std::cout << "--- SensorBoardEx run Over... " << std::endl;
}

int 
SensorBoardEx::runThread(void)
{
    if ( true == m_bThreadCanRun ) {
        std::thread runThread;
        runThread = std::thread(SensorBoardEx::run, Ptr(this) );
        runThread.detach();
        return 0;
    } else {
        return -1;
    }
}


// int 
// SensorBoardEx::init(const IUartlike::Ptr pIUart, int SensorBoardVer)
// {
//     if ( 0 == setDataInterface(pIUart) && activateExtension( (bool)(SensorBoardVer & 0x1) ) ) {
//         return init();
//     } else {
//         return -2;
//     }
// }

void 
SensorBoardEx::setDeviceName(std::string strDevice)
{
    m_strDevice = strDevice;
}


int 
SensorBoardEx::init(const char *sDevice, int nBaud, int nDataBit, int nParity, int nStopBit, int SensorBoardVer)
{
    setDeviceName(sDevice);
    setDataInterface( UartPortSelect::getInstance() );
    activateExtension( (bool)(SensorBoardVer & 0x1) );
    int nRet = m_pUartSel->init(sDevice, nBaud, nDataBit, nParity, nStopBit);
    if (nRet != 0 ) {
        std::cout << " fail to initiate " << sDevice << ", nRet: "<< nRet << std::endl;
        return nRet;
    }
    runThread();
    return 0;
}
