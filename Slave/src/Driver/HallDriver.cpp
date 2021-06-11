#include "Driver/HallDriver.h"
#include "ThirdParty/jsoncpp/json.h"
#include <algorithm>
#include <thread>
#include <stdio.h>
#include <unistd.h>


HallDriver::~HallDriver()
{
  m_bThreadRun = false;
}

HallDriver *HallDriver::getInstance()
{
  return SingleTon<HallDriver>::GetInstance();
}

int HallDriver::init()
{
    m_bThreadRun = true;
    m_bCaught = false;

    TcpCltSession *session = TcpCltSession::getInstance();
    m_tcpClient = session->getSession(TCP_MAIN_BOARD);
    
    /* std::thread runThread;
    runThread = std::thread(HallDriver::run, this);
    runThread.detach(); */
    return 0;
}

std::string HallDriver::spliceReq(const char* reqName, int transId)
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

int HallDriver::parseRes(std::string res, TcpResp& result, int& trigger)
{
    Json::Reader reader;
    Json::Value val;
    Json::Value array;

    JSON_TRY
    result.errcode = -1;
    if (reader.parse(res, val, false)) {
        if (!val["transId"].isNull()) {
            result.transId = val["transId"].asString();
        }
        if (!val["errcode"].isNull()) {
            result.errcode = val["errcode"].asInt();
        }
        if (!val["errmsg"].isNull()) {
            result.errmsg = val["errmsg"].asString();
        }

        if ((!val["trigger"].isNull()) && (result.errcode == 0)) {
            trigger = val["trigger"].asInt();
        }
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }
    JSON_CATCH
    return result.errcode;
}

bool HallDriver::getHall()
{
    return m_bCaught;
}

int HallDriver::catchHall(int& trigger)
{
    std::string req = spliceReq("chargeTriggerStatus", (int)time(0));
    std::string res;
    int nRet = m_tcpClient->request(req, res);
    if (nRet != 0) {
        return -1;
    }

    TcpResp result;
    nRet = parseRes(res, result, trigger);
    if (nRet != 0) {
        return -1;
    }
    m_bCaught = trigger;
    return 0;
}

/* void HallDriver::run(HallDriver *pHallDriver)
{
    printf("--- HallDriver run Start...\n");

    int hallStat;
    const int nBaseFrequence = 1000000;

    while (pHallDriver->m_bThreadRun)
    {
        pHallDriver->catchHall(hallStat);
        usleep(nBaseFrequence);
    }
    printf("--- HallDriver run Over...\n");
} */
