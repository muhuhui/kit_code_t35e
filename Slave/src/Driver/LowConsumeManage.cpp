#include "Driver/LowConsumeManage.h"
#include "ThirdParty/jsoncpp/json.h"
#include <thread>
#include <algorithm>

LowConManage::~LowConManage()
{
}

LowConManage *LowConManage::getInstance()
{
    return SingleTon<LowConManage>::GetInstance();
}

void LowConManage::init()
{
    TcpCltSession *session = TcpCltSession::getInstance();
    m_tcpClient = session->getSession(TCP_MAIN_BOARD);
}

std::string LowConManage::spliceReq(const char* reqName, int transId)
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

std::string LowConManage::spliceReq(const char* reqName, int transId, std::map<LowConEnum, int>& stat)
{
    std::string req;
    Json::Value val;
    Json::Value root;

    if (stat.find(LOW_CON_RADAR) != stat.end()) {
        val["radar"] = stat[LOW_CON_RADAR];
    }
    if (stat.find(LOW_CON_RFID) != stat.end()) {
        val["rfid"] = stat[LOW_CON_RFID];
    }
    if (stat.find(LOW_CON_MOVE_MOTOR) != stat.end()) {
        val["moveMotor"] = stat[LOW_CON_MOVE_MOTOR];
    }
    if (stat.find(LOW_CON_LIFT_MOTOR) != stat.end()) {
        val["liftMotor"] = stat[LOW_CON_LIFT_MOTOR];
    }

    root["switch"] = Json::Value(val);
    root["transId"] = std::to_string(transId);
    root["reqName"] = reqName;

    req = root.toStyledString();
    req.erase(std::remove_if(req.begin(), req.end(), 
                [](char c) -> bool { return (c == '\n' || c == '\t' || c == '\r' || c == ' '); }), req.end());
    return req;
}

int LowConManage::parseRes(std::string res, TcpResp& result)
{
    Json::Reader reader;
    Json::Value val;

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

int LowConManage::parseRes(std::string res, TcpResp& result, LowConStatus& stat)
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
            array = val["switch"];
            stat.radar = array["radar"].asInt();
            stat.rfid = array["rfid"].asInt();
            stat.moveMotor = array["moveMotor"].asInt();
            stat.liftMotor = array["liftMotor"].asInt();
        }
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }
    return result.errcode;
}

int LowConManage::switchGet(LowConStatus& stat)
{
    std::string req = spliceReq("lowConsumSwStatus", (int)time(0));
    std::string res;
    int nRet = m_tcpClient->request(req, res);
    if (nRet != 0) {
        return -1;
    }

    TcpResp result;
    nRet = parseRes(res, result, stat);
    if (nRet != 0) {
        return -1;
    }

    return 0;
}

int LowConManage::switchSet(LowConStatus& stat)
{
    std::map<LowConEnum, int> statmap;
    statmap[LOW_CON_RFID] = stat.rfid;
    statmap[LOW_CON_RADAR] = stat.radar;
    statmap[LOW_CON_MOVE_MOTOR] = stat.moveMotor;
    statmap[LOW_CON_LIFT_MOTOR] = stat.liftMotor;
    std::string req = spliceReq("lowConsumSwCtrl", (int)time(0), statmap);
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

int LowConManage::rfidSwitch(bool sw)
{
    std::map<LowConEnum, int> statmap;
    statmap[LOW_CON_RFID] = sw;
    std::string req = spliceReq("lowConsumSwCtrl", (int)time(0), statmap);
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

int LowConManage::radarSwitch(bool sw)
{
    std::map<LowConEnum, int> statmap;
    statmap[LOW_CON_RADAR] = sw;
    std::string req = spliceReq("lowConsumSwCtrl", (int)time(0), statmap);
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

int LowConManage::moveMotorSwitch(bool sw)
{
    std::map<LowConEnum, int> statmap;
    statmap[LOW_CON_MOVE_MOTOR] = sw;
    std::string req = spliceReq("lowConsumSwCtrl", (int)time(0), statmap);
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

int LowConManage::liftMotorSwitch(bool sw)
{
    std::map<LowConEnum, int> statmap;
    statmap[LOW_CON_LIFT_MOTOR] = sw;
    std::string req = spliceReq("lowConsumSwCtrl", (int)time(0), statmap);
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

