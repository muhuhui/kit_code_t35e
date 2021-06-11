#include "Transaction/ImageProcessManager.h"
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include "Driver/MyLog4CXX.h"
#include "Common/HttpPost/CHttpPost.hpp"
#include "Poco/Net/HTTPResponse.h"
#include <vector>
#include <set>

const std::string LOG_TAG("root.ImageProcessManager::");

static void clear(queue<ViewPointInfo>& q) {
	queue<ViewPointInfo> empty;
	swap(empty, q);
}

ImageProcessManager::~ImageProcessManager()
{
}

ImageProcessManager *ImageProcessManager::getInstance()
{
    return SingleTon<ImageProcessManager>::GetInstance();
}

void ImageProcessManager::init()
{
    while(!m_ViewPointInfo.empty()) {
        m_ViewPointInfo.pop();
    }

    std::thread runThread;
    runThread = std::thread(ImageProcessManager::run, this);
    runThread.detach();

}

void ImageProcessManager::run(ImageProcessManager *pImageProcessManager)
{
    printf("--- ImageProcessManager run Start...\n");

    const int nBaseFrequence = 1000*1000;   //1s

    while (1) {

        usleep(nBaseFrequence);
        pImageProcessManager->m_lck.lock();
        if(pImageProcessManager->m_ViewPointInfo.empty()) {
            pImageProcessManager->m_lck.unlock();
            continue;
        }

        //缓冲区最大存放100个点
        if(pImageProcessManager->m_ViewPointInfo.size() > 100) {
            clear(pImageProcessManager->m_ViewPointInfo);
            pImageProcessManager->m_lck.unlock();
            continue;
        }

        ViewPointInfo viewPoint = pImageProcessManager->m_ViewPointInfo.front();
        pImageProcessManager->m_ViewPointInfo.pop();
        pImageProcessManager->m_lck.unlock();

        pImageProcessManager->getProcessResult(viewPoint);
    }
    printf("--- ImageProcessManager run Over...\n");
}

int ImageProcessManager::getProcessResult(ViewPointInfo info)
{
    static int transId = 0;
    EqusInfo equsInfo;
    equsInfo.transId = transId;

    //查询当前巡检点所有设备
    MySQLServer::getInstance()->queryPointEqusInfo(info, equsInfo);

    if(equsInfo.meterInfo.size() == 0) {
        return 0;
    }

    //转换JSON
    Poco::JSON::Object equsObj;
    MySQLServer::getInstance()->equsInfo2Json(equsInfo, equsObj);

    std::vector<CHttpPost::IdentifyResult> resultList;

    //请求图像识别
    if (CHttpPost::GetImageProRes(equsObj, resultList) == 0) {
        for(auto result: resultList) {
            //告警判断
            if (equsInfo.meterInfo.size() <= 0) {
                printf("ImageProcessManager::%s(), no remain equipment!\n", __FUNCTION__);
                break;
            }

            auto equIter = equsInfo.meterInfo.begin();
            double alarmValue, alarmMin, alarmMax;
            if (equIter->nAlarmDType <= 3) {
                alarmValue = atof(result.value.c_str());
                alarmMin = atof(equIter->sAlarmMin.c_str());
                alarmMax = atof(equIter->sAlarmMax.c_str());
            }

            printf("ImageProcessManager::%s(), alarmValue: %s, min: %f, max: %f, alarmType: %d, innerId: %d, equId: %d\n", __FUNCTION__, result.value.c_str(), alarmMin, alarmMax, equIter->nAlarmDType, equIter->nInnerId, equIter->nEquId);
            result.isAlarm = false;
            switch (equIter->nAlarmRule) {
                case EQU_ALARM_BT:
                    if (alarmValue > alarmMax) {
                        result.isAlarm = true;
                    }
                    break;
                case EQU_ALARM_LT:
                    if (alarmValue < alarmMax) {
                        result.isAlarm = true;
                    }
                    break;
                case EQU_ALARM_EQ:
                    if (alarmValue == alarmMax) {
                        result.isAlarm = true;
                    }
                    break;
                case EQU_ALARM_BE:
                    if (alarmValue >= alarmMax) {
                        result.isAlarm = true;
                    }
                    break;
                case EQU_ALARM_LE:
                    if (alarmValue <= alarmMax) {
                        result.isAlarm = true;
                    }
                    break;
                case EQU_ALARM_RANGE:
                    if ((alarmValue >= alarmMin) && 
                        (alarmValue <= alarmMax)) {
                        result.isAlarm = true;
                    }
                    break;
                case EQU_ALARM_IN:
                    if(equIter->objectSet.find(atoi(result.value.c_str())) != equIter->objectSet.end()) {
                        result.isAlarm = true;
                    }
                    break;
                case EQU_ALARM_NOTIN:
                    if(equIter->objectSet.find(atoi(result.value.c_str())) == equIter->objectSet.end()) {
                        result.isAlarm = true;
                    }
                    break;
                default:
                    break;
            }

            if (!equIter->valueMap.isNull(result.value)) {
                Poco::Dynamic::Var value = equIter->valueMap.get(result.value);
                result.value = value.toString();
            }

            //记录识别结果
            MySQLServer::getInstance()->recordImageIdentifyResult(info, *equIter, result);
            equsInfo.meterInfo.pop_front();
        }
    }
    transId++;
    return 0;
}

void ImageProcessManager::appendViewPointInfo(ViewPointInfo info)
{
    m_lck.lock();
    m_ViewPointInfo.push(info);
    m_lck.unlock();
}

