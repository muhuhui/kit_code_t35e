#include "Driver/ChargePileDriver.h"
#include "ThirdParty/jsoncpp/json.h"
#include <algorithm>
#include <sys/time.h>
#include "Driver/MySQLDriver.h"


ChargePileDriver::~ChargePileDriver()
{
}

ChargePileDriver *ChargePileDriver::getInstance()
{
    return SingleTon<ChargePileDriver>::GetInstance();
}

int ChargePileDriver::init(std::map<int, char* > chargePilesMap)
{
    resetMap(chargePilesMap);

    TcpCltSession *session = TcpCltSession::getInstance();
    m_tcpClient = session->getSession(TCP_MAIN_BOARD);

    return 0;
}

void ChargePileDriver::resetMap(std::map<int, char* > chargePilesMap)
{
    m_statusMap.clear();
    m_chargepilesMap.clear();
    std::map<int, char* >::iterator iter = chargePilesMap.begin();
    while (iter != chargePilesMap.end() ) {
        m_chargepilesMap.insert(*iter);
        m_statusMap.insert(std::make_pair(iter->first, CP_OK));
        ++iter;
    }
}

std::string ChargePileDriver::spliceReq(const char* reqName, int transId)
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

std::string ChargePileDriver::spliceReq(const char* reqName, int transId, int pos)
{
    std::string req;
    Json::Value val;
    Json::Value array;
    val["transId"] = std::to_string(transId);
    val["reqName"] = reqName;
    val["pos"] = pos;
    req = val.toStyledString();
    req.erase(std::remove_if(req.begin(), req.end(), 
                [](char c) -> bool { return (c == '\n' || c == '\t' || c == '\r' || c == ' '); }), req.end());
    return req;
}

int ChargePileDriver::parseRes(std::string res, TcpResp& result)
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
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }
    JSON_CATCH
    return result.errcode;
}

int ChargePileDriver::parseRes(std::string res, TcpResp& result, ChargeInfo& info)
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
            Json::Value resVal = val["result"];
            info.chargeStat = (ChargeStatus)resVal["chargeStat"].asInt();
            info.isWarning = resVal["isWarning"].asInt();
        }
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }
    JSON_CATCH
    return result.errcode;
}

int ChargePileDriver::gotoCharge(int pos)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    std::string req = spliceReq("chargeCtrl", tv.tv_usec, pos);
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

int ChargePileDriver::getChargeInfo(ChargeInfo& info)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    std::string req = spliceReq("chargeStatus", tv.tv_usec);
    std::string res;
    int nRet = m_tcpClient->request(req, res);
    if (nRet != 0) {
        return -1;
    }

    TcpResp result;
    result.transId = std::to_string(tv.tv_usec);
    nRet = parseRes(res, result, info);
    if (nRet != 0) {
        return -1;
    }
    return 0;
}

void ChargePileDriver::deleteChargePile()
{
  MySQLDriver* mySQLDriver = MySQLDriver::getInstance();
  std::string sqlStr = "DELETE FROM charge";
  Session ses = mySQLDriver->getSession();
  if(ses.isConnected())
  {
    try {
        Statement statement(ses);
        statement << sqlStr, now;
    } catch(Poco::Exception& exc) {
      std::cerr << "ChargePileDriver::deleteChargePile " << exc.displayText() << std::endl;
    }
  }
  ses.close();
}

