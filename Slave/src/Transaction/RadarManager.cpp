#include "Transaction/RadarManager.h"
#include "Transaction/SettingService.h"
#include "Transaction/Motor.h"
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include "Driver/MyLog4CXX.h"

const std::string LOG_TAG("root.RadarManager::");

RadarManager::~RadarManager()
{
    m_bThreadRun = false;
}

RadarManager *RadarManager::getInstance()
{
    return SingleTon<RadarManager>::GetInstance();
}

int RadarManager::addRadar(RadarNum num)
{
    SettingService* pSettingService = SettingService::getInstance();
    std::pair<RadarNum, int> radar;
    radar.first = num;
    switch (num) {
        case RADAR_FRONT:
            radar.second = pSettingService->m_setting.getULSoundThreshold();
            m_radardisMap.insert(radar);
            break;
        case RADAR_BACK:
            radar.second = pSettingService->m_setting.getbackULSoundThreshold();
            m_radardisMap.insert(radar);
            break;
        case RADAR_DOWN:
            radar.second = pSettingService->m_setting.getdownULSoundThreshold();
            m_radardisMap.insert(radar);
            break;
        default:
            break;
    }

    return 0;
}

void RadarManager::init()
{
    m_pRadarDriver = RadarDriver::getInstance();
    m_pRadarDriver->init();
    m_bThreadRun = true;
    m_eWarnType = 0;
    
    setThreshold();

    std::thread runThread;
    runThread = std::thread(RadarManager::run, this);
    runThread.detach();
}

int RadarManager::setWarnThreshold(int nThreshold)
{
    if ((nThreshold <= 100) || (nThreshold >= 1000)) {
        printf("----inavailible setting-----\n");
        return -1;
    }
    else {
        m_radardisMap[RADAR_FRONT] = nThreshold;
        printf("----前超声波距离 to %d mm \n", m_radardisMap[RADAR_FRONT]);
        int nRet = setThreshold();
        return nRet;
    }
}

int RadarManager::setBackWarnThreshold(int nThreshold)
{
    if ((nThreshold <= 100) || (nThreshold >= 1000)) {
        printf("----inavailible setting-----\n");
        return -1;
    }
    else {
        m_radardisMap[RADAR_BACK] = nThreshold;
        printf("----后超声波距离 to %d mm \n", m_radardisMap[RADAR_BACK]);
        int nRet = setThreshold();
        return nRet;
    }
}

int RadarManager::setDownWarnThreshold(int nThreshold)
{
    if ((nThreshold <= 100) || (nThreshold >= 1000)) {
        printf("----inavailible setting-----\n");
        return -1;
    }
    else {
        m_radardisMap[RADAR_DOWN] = nThreshold;
        printf("----下超声波距离 to %d mm \n", m_radardisMap[RADAR_DOWN]);
        int nRet = setThreshold();
        return nRet;
    }
}

void RadarManager::attach(Observers *pObserver)
{
    m_ObserverList.push_back(pObserver);
}

void RadarManager::detach(Observers *pObserver)
{
    m_ObserverList.remove(pObserver);
}

void RadarManager::notify()
{
    std::list<Observers *>::iterator it = m_ObserverList.begin();
    while (it != m_ObserverList.end())
    {
        (*it)->update(SUB_ULTRASOUND);
        ++it;
    }
}

void RadarManager::judgeWarn()
{
    auto logger = initLogger(LOG_TAG + std::string(__FUNCTION__));

    int lastWarnStatus = getWarnStatus();
    SettingService* pSettingService = SettingService::getInstance();
    bool softSwitch = pSettingService->m_setting.getUlsoundSwitch();
    bool softbackSwitch = pSettingService->m_setting.getBackUlsoundSwitch();
    bool softdownSwitch = pSettingService->m_setting.getDownUlsoundSwitch();

    RadarStatus radarStatus;
    m_pRadarDriver->getRadarStatus(radarStatus);
    
    for (auto iter = m_radardisMap.begin(); iter != m_radardisMap.end(); iter++) {
        RadarNum num = iter->first;
        if ((softSwitch && (num == RADAR_FRONT) && radarStatus.radarFront.isBlock) ||
            (softbackSwitch && (num == RADAR_BACK) && radarStatus.radarBack.isBlock) ||
            (softdownSwitch && (num == RADAR_DOWN) && radarStatus.radarBottom.isBlock)) {
            setWarnStatus((RadarNum)num);
        }
        else {
            clearWarnStatus((RadarNum)num);
        }
/* 
        int status = 0, arm_status = 0;
        Motor* pMotor = Motor::getInstance();
        status = pMotor->getCurStatus();
        if ((getWarnStatus() != lastWarnStatus) &&
            (((status > 0) && getWarnStatus(RADAR_FRONT)) ||
            ((status < 0) && getWarnStatus(RADAR_BACK)))) {
            pMotor->emergencyStop();
            printf("+++++ radar emergeStop ------\n");
            LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT(" radar emergeStop. ") );
        }

        arm_status = pMotor->getArmCurStatus();
        if ((getWarnStatus() != lastWarnStatus) &&
            ((arm_status > 0) && getWarnStatus(RADAR_DOWN))) {
            pMotor->armEmergencyStop();
            printf("+++++ radar arm emergeStop ------\n");
            LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT(" radar arm emergeStop. ") );
        }
 */
    }
    if (getWarnStatus() != lastWarnStatus) 
	{
        notify(); 
    }
}

void RadarManager::run(RadarManager *pRadarManager)
{
    printf("--- RadarManager run Start...\n");

    const int nBaseFrequence = 1000*1000;   //1s

    while (pRadarManager->m_bThreadRun) {
        pRadarManager->judgeWarn();
        usleep(nBaseFrequence);
    }
    printf("--- RadarManager run Over...\n");
}

float RadarManager::getDistance(int num)
{
    RadarStatus radarStatus;
    m_pRadarDriver->getDisAndWarn(radarStatus);
    
    if(num == RADAR_FRONT)
    {
        return radarStatus.radarFront.distance/1000.0;
    }
    else if(num == RADAR_BACK)
    {
        return radarStatus.radarBack.distance/1000.0;
    }
    else if(num == RADAR_DOWN)
    {
        return radarStatus.radarBottom.distance/1000.0;
    }
    else{
        return 0.0;
    }
}

int RadarManager::setThreshold() 
{
    int nRet = -1;
    m_pRadarDriver = RadarDriver::getInstance();

    std::vector<RadarConfig> vConfig;
    for (auto iter = m_radardisMap.begin(); iter != m_radardisMap.end(); iter++) {
        RadarConfig radarConfig;
        radarConfig.id = iter->first;
        radarConfig.warnDis = iter->second;
        radarConfig.blindZoneMin = 0;
        radarConfig.blindZoneMax = 1000;
        SettingService* pSettingService = SettingService::getInstance();
        switch(radarConfig.id) {
            case RADAR_FRONT:
                radarConfig.enable = pSettingService->m_setting.getUlsoundSwitch();
                break;
            case RADAR_BACK:
                radarConfig.enable = pSettingService->m_setting.getBackUlsoundSwitch();
                break;
            case RADAR_DOWN:
                radarConfig.enable = pSettingService->m_setting.getDownUlsoundSwitch();
                break;
            default:
                break;
        }
        vConfig.push_back(radarConfig);
    }

    nRet = m_pRadarDriver->setRadarConfig(vConfig);
    return nRet;
}


