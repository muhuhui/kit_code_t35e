#include "Driver/PDDMotorDriver.h"
#include "ThirdParty/jsoncpp/json.h"
#include <thread>
#include <algorithm>

PDDMotorDriver::~PDDMotorDriver()
{
}

PDDMotorDriver *PDDMotorDriver::getInstance()
{
    return SingleTon<PDDMotorDriver>::GetInstance();
}

void PDDMotorDriver::init()
{
    TcpCltSession *session = TcpCltSession::getInstance();
    m_tcpClient = session->getSession(TCP_MAIN_BOARD);
}

std::string PDDMotorDriver::spliceReq(const char* reqName, int transId)
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

std::string PDDMotorDriver::spliceReq(const char* reqName, int transId, PDDMotorCmd cmd, int pos)
{
    std::string req;
    Json::Value val;
    val["transId"] = std::to_string(transId);
    val["reqName"] = reqName;
    val["cmd"] = cmd;
    if (cmd == PDD_GO_TO) {
        val["pos"] = pos;
    }
    req = val.toStyledString();
    req.erase(std::remove_if(req.begin(), req.end(), 
                [](char c) -> bool { return (c == '\n' || c == '\t' || c == '\r' || c == ' '); }), req.end());
    return req;
}

int PDDMotorDriver::parseRes(std::string res, TcpResp& result)
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
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }
    JSON_CATCH
    return result.errcode;
}

int PDDMotorDriver::parseRes(std::string res, TcpResp& result, PDDMotorInfo& stat)
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

        if ((!val["result"].isNull()) && (result.errcode == 0)) {
            Json::Value array = val["result"];
            stat.moveStat = (PDDMoveStat)array["moveStat"].asInt();
            stat.pos = array["pos"].asInt();
            stat.velocity = array["velocity"].asInt();
            stat.warn = array["warn"].asInt();
        }
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }
    JSON_CATCH
    return result.errcode;
}

int PDDMotorDriver::getPDDStatus(PDDMotorInfo& stat)
{
    std::string req = spliceReq("pdMotorStatus", (int)time(0));
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

int PDDMotorDriver::ctrlPDD(PDDMotorCmd cmd, int pos)
{
    std::string req = spliceReq("pdMotorCtrl", (int)time(0), cmd, pos);
    std::string res;

    int nRet = m_tcpClient->request(req, res);

    TcpResp result;
    nRet = parseRes(res, result);
    if (nRet != 0) {
        return -1;
    }
    return 0;
}

int PDDMotorDriver::manualForward()
{
	return ctrlPDD(PDD_GO_FORWARD);
}

int PDDMotorDriver::manualBackward()
{
	return ctrlPDD(PDD_GO_BACK);
}

int PDDMotorDriver::manualStop()
{
	return ctrlPDD(PDD_STOP);
}

int PDDMotorDriver::gotoPosition(int pos)
{
	return ctrlPDD(PDD_GO_TO, pos);
}

int PDDMotorDriver::calibration()
{
	return ctrlPDD(PDD_CALI);
}

