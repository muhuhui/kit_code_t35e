#include "Driver/VersionQuery.h"
#include "ThirdParty/jsoncpp/json.h"
#include <algorithm>
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

VersionQuery *VersionQuery::getInstance()
{
  return SingleTon<VersionQuery>::GetInstance();
}

int VersionQuery::queryVersion(VersionInfo info)
{
    TcpCltSession* cltSession = TcpCltSession::getInstance();
    int nRet = 0;

    struct timeval tv;
    gettimeofday(&tv, NULL);

    switch(info)
    {
    case reqMotionVersion:
    {
        m_tcpClient = cltSession->getSession(TCP_MAIN_BOARD);

        std::string req = spliceReq("reqMotionVersion", tv.tv_usec);
        std::string res;
        nRet = m_tcpClient->request(req, res);
        if (nRet != 0) {
            break;
        }

        DeviceInfo result;
        result.transId = std::to_string(tv.tv_usec);
        nRet = parseRes(res, result);
        if (nRet != 0) {
            break;
        }

        m_deviceInfo.application = result.application;

        break;
    }
    case reqPtzVersion: 
    {
        m_tcpClient = cltSession->getSession(TCP_PTZ_BOARD);

        std::string req = spliceReq("reqPTZVersion", tv.tv_usec);
        std::string res;
        nRet = m_tcpClient->request(req, res);
        if (nRet != 0) {
            break;
        }

        DeviceInfo result;
        result.transId = std::to_string(tv.tv_usec);
        nRet = parseRes(res, result);
        if (nRet != 0) {
            break;
        }

        m_deviceInfo.application = result.application;

        break;
    }
    case reqSensorVersion: 
    {
        m_tcpClient = cltSession->getSession(TCP_PTZ_BOARD);

        std::string req = spliceReq("reqSensorVersion", tv.tv_usec);
        std::string res;
        nRet = m_tcpClient->request(req, res);
        if (nRet != 0) {
            break;
        }

        DeviceInfo result;
        result.transId = std::to_string(tv.tv_usec);
        nRet = parseRes(res, result);
        if (nRet != 0) {
            break;
        }

        m_deviceInfo.application = result.application;

        break;
    }
/*     case reqPMUVersion: 
    {
        m_tcpClient = cltSession->m_cltSession[TCP_MAIN_BOARD];
        break;
    } */
    default:
        break;
    }

    return nRet;
}

std::string VersionQuery::spliceReq(const char* reqName, int transId)
{
    std::string req;
    Json::Value val;
    val["transId"] = std::to_string(transId);
    val["reqName"] = reqName;
    req = val.toStyledString();
    req.erase(std::remove_if(req.begin(), req.end(), 
                [](char c) -> bool { return (c == '\n' || c == '\t' || c == '\r' || c == ' '); }), req.end());
    return req;
}

int VersionQuery::parseRes(std::string res, DeviceInfo& result)
{
    Json::Reader reader;
    Json::Value val;
    Json::Value array;

    JSON_TRY
    if (reader.parse(res, val, false)) {
        if (!val["transId"].isNull()) {
            if (result.transId != val["transId"].asString()) {
                return -1;
            }
            result.transId = val["transId"].asString();
        }
        else{
            return -1;
        }
        if (!val["bootloader"].isNull()) {
            result.bootloader = val["bootloader"].asString();
        }
        else{
            return -1;
        }
        if (!val["application"].isNull()) {
            result.application = val["application"].asString();
        }
        else{
            return -1;
        }
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
        return -1;
    }
    JSON_CATCH
    return 0;
}

std::string VersionQuery::getVersion()
{
    return m_deviceInfo.application;
}