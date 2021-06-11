#include "BatteryDriver.h"
#include "jsoncpp/json.h"
#include <thread>
#include <algorithm>
#include <sys/time.h>

BatteryDriver::~BatteryDriver()
{
}

BatteryDriver *BatteryDriver::getInstance()
{
    return SingleTon<BatteryDriver>::GetInstance();
}

std::string BatteryDriver::spliceReq(const char* reqName, int transId)
{
    std::string req;
    Json::Value val;
    Json::Value array;
    val["transId"] = std::to_string(transId);
    val["reqName"] = reqName;
    req = val.toStyledString();
    req.erase(std::remove_if(req.begin(), req.end(), 
                [](char c) -> bool { return (c == '\n' || c == '\t' || c == '\r' || c == ' '); }), req.end());
    return req;
}

int BatteryDriver::parseRes(std::string res, TcpResp& result, BatteryInfo& batteryInfo)
{
    Json::Reader reader;
    Json::Value val;
    Json::Value array;

    JSON_TRY
    result.errcode = -1;
    if (reader.parse(res, val, false)) {
        if (!val["transId"].isNull()) {
            if (result.transId != val["transId"].asString()) {
                return -1;
            }
            result.transId = val["transId"].asString();
        }
        if (!val["errcode"].isNull()) {
            result.errcode = val["errcode"].asInt();
        }
        if (!val["errmsg"].isNull()) {
            result.errmsg = val["errmsg"].asString();
        }

        if ((!val["result"].isNull()) && (result.errcode == 0)) {
            batteryInfo.quantity=val["result"][0]["quantity"].asFloat();
            batteryInfo.voltage=val["result"][0]["voltage"].asFloat() / 1000;
            batteryInfo.current=val["result"][0]["current"].asFloat() / 1000;
            batteryInfo.temp=val["result"][0]["temp"].asFloat();
            batteryInfo.cycle=val["result"][0]["cycle"].asFloat();
            batteryInfo.capacity=val["result"][0]["capacity"].asFloat();
        }
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }
    JSON_CATCH
    return result.errcode;
}

int BatteryDriver::getBatteryInfo(BatteryInfo& batteryInfo)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    std::string req = spliceReq("batteryInfo", tv.tv_usec);
    std::string res;
    int nRet = m_tcpClient->request(req, res);
    if (nRet != 0) {
        return -1;
    }
    TcpResp result;
    result.transId = std::to_string(tv.tv_usec);
    nRet = parseRes(res, result, batteryInfo);
    if (nRet != 0) {
        return -1;
    }

    return 0;
}

void BatteryDriver::updateInfo(BatteryDriver* pBatteryDriver)
{
    printf("BatteryDriver::%s(), start\n", __FUNCTION__);
    while (1)
    {
        pBatteryDriver->getBatteryInfo(pBatteryDriver->m_BatteryInfo);
        sleep(5);
    }
}

int BatteryDriver::init() 
{
    TcpCltSession *session = TcpCltSession::getInstance();
    m_tcpClient = session->getSession(TCP_MAIN_BOARD);

    std::thread updateInfoThread;
    updateInfoThread = std::thread(BatteryDriver::updateInfo, this);
    updateInfoThread.detach();  

    return 0;
}

