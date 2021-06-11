#include "Driver/IlluminateDriver.h"
#include "ThirdParty/jsoncpp/json.h"
#include <algorithm>
#include <stdio.h>
#include <unistd.h>


IlluminateDriver::~IlluminateDriver()
{
    m_bThreadRun = false;
}

IlluminateDriver *IlluminateDriver::getInstance()
{
    return SingleTon<IlluminateDriver>::GetInstance();
}

int IlluminateDriver::init()
{
    TcpCltSession *session = TcpCltSession::getInstance();
    m_tcpClient = session->getSession(TCP_MAIN_BOARD);

    return 0;
}

std::string IlluminateDriver::spliceReq(const char* reqName, int transId)
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

std::string IlluminateDriver::spliceReq(const char* reqName, int transId, int sw)
{
    std::string req;
    Json::Value val;
    Json::Value array;
    val["transId"] = std::to_string(transId);
    val["reqName"] = reqName;
    val["switch"] = sw;
    req = val.toStyledString();
    req.erase(std::remove_if(req.begin(), req.end(), 
                [](char c) -> bool { return (c == '\n' || c == '\t' || c == '\r' || c == ' '); }), req.end());
    return req;
}

int IlluminateDriver::parseRes(std::string res, TcpResp& result)
{
    Json::Reader reader;
    Json::Value val;
    Json::Value array;

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
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }
    return result.errcode;
}

int IlluminateDriver::parseRes(std::string res, TcpResp& result, int& sw)
{
    Json::Reader reader;
    Json::Value val;
    Json::Value array;

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

        if ((!val["switch"].isNull()) && (result.errcode == 0)) {
            sw = val["switch"].asInt();
        }
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }
    return result.errcode;
}

int IlluminateDriver::swCtrl(int sw)
{
    std::string req = spliceReq("illumSwCtrl", (int)time(0), sw);
    std::string res;
    int nRet = m_tcpClient->request(req, res);
    if (nRet != 0) {
        return -1;
    }

    TcpResp result;
    nRet = parseRes(res, result);
    if (nRet != 0) {
        return -1;
    }
    return 0;
}

int IlluminateDriver::getSw(int& sw)
{
    std::string req = spliceReq("illumSwStatus", (int)time(0));
    std::string res;
    int nRet = m_tcpClient->request(req, res);
    if (nRet != 0) {
        return -1;
    }

    TcpResp result;
    nRet = parseRes(res, result, sw);
    if (nRet != 0) {
        return -1;
    }
    return 0;
}


