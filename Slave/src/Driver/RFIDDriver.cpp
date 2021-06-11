#include "RFIDDriver.h"
#include "jsoncpp/json.h"
#include <thread>
#include <algorithm>
#include <sys/time.h>

RFIDDriver::~RFIDDriver()
{
}

//请求接口
RFIDDriver *RFIDDriver::getInstance()
{
    return SingleTon<RFIDDriver>::GetInstance();
}

void RFIDDriver::init()
{
    TcpCltSession *session = TcpCltSession::getInstance();
    m_tcpClient = session->getSession(TCP_MAIN_BOARD);
}

std::string RFIDDriver::spliceReq(const char* reqName, int transId)
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

int RFIDDriver::parseRes(std::string res, TcpResp& result,std::string &rfidNUM)
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
        //返回值中的rfidNum
        if ((!val["rfidNum"].isNull()) && (result.errcode == 0)) {
            //返回RFID结果：RFID的编号
            rfidNUM=val["rfidNum"].asString();
        }     
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }
    JSON_CATCH
    return result.errcode;
}

int RFIDDriver::getRFIDNum(std::string& rfidNUM)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    std::string req = spliceReq("rfidRead", tv.tv_usec);
    std::string res;
    int nRet = m_tcpClient->request(req, res);
    if (nRet != 0) {
        return -1;
    }
    TcpResp result;
    result.transId = std::to_string(tv.tv_usec);
    nRet = parseRes(res, result,rfidNUM);
    if (nRet != 0) {
        return -1;
    }
    return 0;
}

