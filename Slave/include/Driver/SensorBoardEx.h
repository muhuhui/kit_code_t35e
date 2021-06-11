#ifndef SENSORBOARDEX_H
#define SENSORBOARDEX_H

#include "base/UartPortSelect.h"

class SensorBoardEx
{
public:
    SensorBoardEx(/* args */);
    SensorBoardEx(bool hasEx);
    SensorBoardEx(const SensorBoardEx&) = delete;
    ~SensorBoardEx();
    using Ptr = std::shared_ptr< SensorBoardEx >;

    struct GasInfo
    {
        float   fCH4;
        float   fO2;
        float   fCO;
        float   fSO2;
        float   fSmog;
        float   fCO2;   //二氧化碳
    };

    struct EnvInfo
    {
        float   fTemperature;
        float   fHumidity;
        float   fIlluminance;
    };

    struct PlltInfo
    {
        float fPm1_0;
        float fPm2_5;
        float fPm10;
        float fSF6;
    };
    
    static SensorBoardEx::Ptr getInstance();

    bool activateExtension(bool setEx);
    int setDataInterface( UartPortSelect* pUartSel);

    const SensorBoardEx::GasInfo& getGasInfo(void);
    const SensorBoardEx::EnvInfo& getEnvInfo(void);
    const SensorBoardEx::PlltInfo& getPlltInfo(void);
    int runThread(void);
    int init(const char *sDevice, int nBaud, int nDataBit, int nParity, int nStopBit, int SensorBoardVer);
    void setDeviceName(std::string strDevice);

private:
    UartPortSelect* m_pUartSel;
    bool isExtension;
    std::string m_strDevice;

    std::mutex m_mutex;
    bool m_bThreadCanRun;
    GasInfo       m_Gas;
    EnvInfo       m_Env;
    PlltInfo      m_Pllt;

    int readSensorInfo(void);
    void setGasInfo(GasInfo gas);
    void setEnvInfo(EnvInfo Env);
    void setPlltInfo(PlltInfo Pllt);
    static void run(SensorBoardEx::Ptr pSensorBoardEx);
};




#endif