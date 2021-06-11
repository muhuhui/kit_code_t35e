#include "Driver/PartialDischargeDetectorDriver.h"
#include "ThirdParty/jsoncpp/json.h"
#include <algorithm>
#include <unistd.h>
#include <thread>
#include <string.h>



PartialDischargeDetectorDriver::~PartialDischargeDetectorDriver()
{
    m_bThreadRun = false;
}

PartialDischargeDetectorDriver *PartialDischargeDetectorDriver::getInstance()
{
    return SingleTon<PartialDischargeDetectorDriver>::GetInstance();
}

int PartialDischargeDetectorDriver::init()
{
    memset(&m_data, 0, sizeof(m_data));
    m_bThreadRun = true;

    TcpCltSession *session = TcpCltSession::getInstance();
    m_tcpClient = session->getSession(TCP_MAIN_BOARD);

    std::thread runThread;
    runThread = std::thread(PartialDischargeDetectorDriver::getInfoThread, this);
    runThread.detach();

    return 0;
}

std::string PartialDischargeDetectorDriver::spliceReq(const char* reqName, int transId)
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

int PartialDischargeDetectorDriver::parseRes(std::string res, TcpResp& result, PDDStat& stat)
{
    Json::Reader reader;
    Json::Value val;

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

        if ((!val["result"].isNull()) && (result.errcode == 0)) {
            Json::Value array = val["result"];
            stat.tev = array["tev"].asInt();
            stat.ultra = array["ultra"].asInt();
        }
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }
    JSON_CATCH
    return result.errcode;
}

int PartialDischargeDetectorDriver::getPDDStatus()
{
    std::string req = spliceReq("pdInfo", (int)time(0));
    std::string res;
    int nRet = m_tcpClient->request(req, res);
    if (nRet != 0) {
        return -1;
    }

    TcpResp result;
    nRet = parseRes(res, result, m_data);
    if (nRet != 0) {
        return -1;
    }

    return 0;
}

float PartialDischargeDetectorDriver::getTEVData()
{
    return m_data.tev;
}

float PartialDischargeDetectorDriver::getUWData()
{
    return m_data.ultra;
}

void PartialDischargeDetectorDriver::getInfoThread(PartialDischargeDetectorDriver* pddDriver)
{
    printf("PartialDischargeDetectorDriver::%s, start", __FUNCTION__);
    while (pddDriver->m_bThreadRun)
    {
        pddDriver->getPDDStatus();
        sleep(5);
    }
}

