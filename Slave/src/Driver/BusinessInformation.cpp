#include "Driver/BusinessInformation.h"
#include "ThirdParty/jsoncpp/json.h"
#include <thread>
#include <algorithm>
#include <sys/time.h>

BusinessInfor::~BusinessInfor()
{
}

BusinessInfor *BusinessInfor::getInstance()
{
    return SingleTon<BusinessInfor>::GetInstance();
}

std::string BusinessInfor::spliceReq(const char* reqName, int transId)
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

std::string BusinessInfor::spliceReq(const char* reqName, int transId, int cmd)
{
    std::string req;
    Json::Value root;

    root["cmd"] = cmd;
    root["transId"] = std::to_string(transId);
    root["reqName"] = reqName;

    req = root.toStyledString();
    req.erase(std::remove_if(req.begin(), req.end(), 
                [](char c) -> bool { return (c == '\n' || c == '\t' || c == '\r' || c == ' '); }), req.end());
    return req;
}

std::string BusinessInfor::spliceReq(const char* reqName, int transId, std::vector<RfidMapping>& rfid)
{
    std::string req;
    Json::Value val;
    Json::Value array;
    Json::Value array_item;
    val["transId"] = std::to_string(transId);
    val["reqName"] = reqName;

    if(!rfid.empty()) {
        for (auto iter = rfid.begin(); iter != rfid.end(); iter++) {
            array_item["id"] = (*iter).num;
            array_item["pos"] = (*iter).pos;
            array.append(array_item);
        }
    }
    val["rfidMap"] = Json::Value(array);

    req = val.toStyledString();
    req.erase(std::remove_if(req.begin(), req.end(), 
                [](char c) -> bool { return (c == '\n' || c == '\t' || c == '\r' || c == ' '); }), req.end());
    return req;
}

int BusinessInfor::parseRes(std::string res, TcpResp& result)
{
    Json::Reader reader;
    Json::Value val;

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
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }
    JSON_CATCH
    return result.errcode;
}

int BusinessInfor::parseRes(std::string res, TcpResp& result, std::vector<RfidMapping>& rfid)
{
    Json::Reader reader;
    Json::Value val;
    Json::Value array;
    RfidMapping rfmap;

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

        if ((!val["rfidMap"].isNull()) && (result.errcode == 0)) {
            rfid.clear();
            array = val["rfidMap"];
            for(int i=0; i<array.size(); i++) {
                rfmap.num = array[i]["id"].asInt64();
                rfmap.pos = array[i]["pos"].asInt();
                rfid.push_back(rfmap);
            }
        }
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }
    JSON_CATCH
    return result.errcode;
}

int BusinessInfor::parseRes(std::string res, TcpResp& result, InspectStatus& stat)
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
            array = val["result"];
            stat.rfid = array["rfid"].asInt();
            stat.battery = array["battery"].asInt();
            stat.radar = array["radar"].asInt();
            stat.moveMotor = array["moveMotor"].asInt();
            stat.liftMotor = array["liftMotor"].asInt();
        }
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }
    JSON_CATCH
    return result.errcode;
}

int BusinessInfor::getInspectStatus(InspectStatus& stat)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    std::string req = spliceReq("selfInspectStatus", tv.tv_usec);
    std::string res;
    int nRet = m_tcpClient->request(req, res);
    if (nRet != 0) {
        return -1;
    }

    TcpResp result;
    result.transId = std::to_string(tv.tv_usec);
    nRet = parseRes(res, result, stat);
    if (nRet != 0) {
        return -1;
    }

    return 0;
}

int BusinessInfor::init()
{
    TcpCltSession *session = TcpCltSession::getInstance();
    m_tcpClient = session->getSession(TCP_MAIN_BOARD);
    return 0;
}

int BusinessInfor::rfidCalibration(int cmd)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    std::string req = spliceReq("rfidCaliCtrl", tv.tv_usec, cmd);
    std::string res;

    int nRet = m_tcpClient->request(req, res);

    TcpResp result;
    result.transId = std::to_string(tv.tv_usec);
    nRet = parseRes(res, result);
    if (nRet != 0) {
        return -1;
    }
    return 0;
}

int BusinessInfor::rfidMapGet(std::vector<RfidMapping>& rfid)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    std::string req = spliceReq("rfidMapGet", tv.tv_usec);
    std::string res;
    int nRet = m_tcpClient->request(req, res);
    if (nRet != 0) {
        return -1;
    }

    TcpResp result;
    result.transId = std::to_string(tv.tv_usec);
    nRet = parseRes(res, result, rfid);
    if (nRet != 0) {
        return -1;
    }

    return 0;
}

int BusinessInfor::rfidMapImport(std::vector<RfidMapping>& rfid)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    std::string req = spliceReq("rfidMapImport", tv.tv_usec, rfid);
    std::string res;
    int nRet = m_tcpClient->request(req, res);
    if (nRet != 0) {
        return -1;
    }

    TcpResp result;
    result.transId = std::to_string(tv.tv_usec);
    nRet = parseRes(res, result);
    if (nRet != 0) {
        return -1;
    }

    return 0;
}

