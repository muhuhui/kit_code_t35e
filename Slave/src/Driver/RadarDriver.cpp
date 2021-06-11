#include "RadarDriver.h"
#include "jsoncpp/json.h"
#include <thread>
#include <algorithm>
#include <sys/time.h>
#include <iostream>

RadarDriver::~RadarDriver()
{
}

RadarDriver *RadarDriver::getInstance()
{
    return SingleTon<RadarDriver>::GetInstance();
}

void RadarDriver::init()
{
    TcpCltSession *session = TcpCltSession::getInstance();
    m_tcpClient = session->getSession(TCP_MAIN_BOARD);
}

//雷达数据状态获取和获得雷达配置参数请求
std::string RadarDriver::spliceReq(const char* reqName, int transId)
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

//解析雷达数据:雷达的距离和告警状态
int RadarDriver::parseRes(std::string res, TcpResp& result, RadarStatus& m_Radar)
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
        if((!val["result"].isNull()) && (result.errcode == 0)){
            //雷达检测距离
            m_Radar.radarFront.distance=val["result"][0]["distance"].asFloat(); 
            m_Radar.radarBack.distance=val["result"][1]["distance"].asFloat();
            m_Radar.radarBottom.distance=val["result"][2]["distance"].asFloat();
            //雷达告警状态
            m_Radar.radarFront.isBlock=val["result"][0]["warn"].asFloat(); 
            m_Radar.radarBack.isBlock=val["result"][1]["warn"].asBool();
            m_Radar.radarBottom.isBlock=val["result"][2]["warn"].asBool();
        }
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }
    JSON_CATCH
    return result.errcode;
}
//获取雷达数据：
int RadarDriver::getRadarStatus(RadarStatus& m_Radar)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    std::string req = spliceReq("radarStatus", tv.tv_usec);
    std::string res;
    int nRet = m_tcpClient->request(req, res);
    if (nRet != 0) {
        return -1;
    }

    TcpResp result;
    result.transId = std::to_string(tv.tv_usec);
    nRet = parseRes(res, result, m_Radar);
    if (nRet != 0) {
        return -1;
    }
    m_RadarStatus = m_Radar;

    return 0;
}
//-------------------------------------------------------------------------//
//雷达设置写入请求
std::string RadarDriver::spliceReq(const char* reqName,std::vector<RadarConfig> radarConfig,int transId)
{
    std::string req;
    Json::Value val;
    val["transId"] = std::to_string(transId);
    val["reqName"] = reqName;
    req = val.toStyledString();
    for (int i = 0; i < (int)radarConfig.size(); i++) {
        val["para"][i]["id"]=radarConfig[i].id;
        if (radarConfig[i].blindZoneMin >= 0) {
            val["para"][i]["blindZoneMin"]=radarConfig[i].blindZoneMin;
        }
        if (radarConfig[i].blindZoneMax >= 0) {
            val["para"][i]["blindZoneMax"]=radarConfig[i].blindZoneMax;
        }
        if (radarConfig[i].warnDis >= 0) {
            val["para"][i]["warnDis"]=radarConfig[i].warnDis;
        }
        if (radarConfig[i].enable >= 0) {
            val["para"][i]["enable"]=radarConfig[i].enable;
        }
    }

    req = val.toStyledString();
    req.erase(std::remove_if(req.begin(), req.end(), 
                [](char c) -> bool { return (c == '\n' || c == '\t' || c == '\r' || c == ' '); }), req.end());
    return req;
}

//解析雷达设置写入的返回值与读取雷达配置返回值
int RadarDriver::parseRes(std::string res, TcpResp& result)
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
//读取雷达配置请求
int RadarDriver::parseRes(std::string res, TcpResp& result, RadarConfigData m_radarConfig)
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
        if((!val["result"].isNull()) && (result.errcode == 0))
        {
            //雷达前、后、下部
            //雷达前
            m_radarConfig.m_radarConfigFront.blindZoneMax=val["result"][0]["blindZoneMax"].asFloat();
            m_radarConfig.m_radarConfigFront.blindZoneMin=val["result"][0]["blindZoneMin"].asFloat();
            m_radarConfig.m_radarConfigFront.warnDis=val["result"][0]["warnDis"].asFloat();
            m_radarConfig.m_radarConfigFront.enable=val["result"][0]["enable"].asInt();
            //雷达后
            m_radarConfig.m_radarConfigBack.blindZoneMax=val["result"][1]["blindZoneMax"].asFloat();
            m_radarConfig.m_radarConfigBack.blindZoneMin=val["result"][1]["blindZoneMin"].asFloat();
            m_radarConfig.m_radarConfigBack.warnDis=val["result"][1]["warnDis"].asFloat();
            m_radarConfig.m_radarConfigBack.enable=val["result"][1]["enable"].asInt();
            //雷达下部
            m_radarConfig.m_radarConfigBottom.blindZoneMax=val["result"][2]["blindZoneMax"].asFloat();
            m_radarConfig.m_radarConfigBottom.blindZoneMin=val["result"][2]["blindZoneMin"].asFloat();
            m_radarConfig.m_radarConfigBottom.warnDis=val["result"][2]["warnDis"].asFloat();    
            m_radarConfig.m_radarConfigBottom.enable=val["result"][2]["enable"].asInt();
        }
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }
    JSON_CATCH
    return result.errcode;
}

int RadarDriver::setRadarConfig(std::vector<RadarConfig> radarConfig)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    std::string req = spliceReq("radarConfig", radarConfig, tv.tv_usec);
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

int RadarDriver::radarSwitch(RadarNum num, int enable)
{
    std::vector<RadarConfig> vradar;
    RadarConfig radar = {num, -1, -1, -1, enable};
    vradar.push_back(radar);
    
    int nRet = setRadarConfig(vradar);

    return nRet;
}

int RadarDriver::readRadarConfig(RadarConfigData &radarConfig)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    std::string req = spliceReq("radarRead", tv.tv_usec);
    std::string res;
    int nRet = m_tcpClient->request(req, res);
    if (nRet != 0) {
        return -1;
    }

    TcpResp result;
    result.transId = std::to_string(tv.tv_usec);
    nRet = parseRes(res, result, radarConfig);
    if (nRet != 0) {
        return -1;
    }

    return 0;
}

void RadarDriver::updateInfo()
{
    const int nBaseFrequence = 1000*1000;
    printf("RadarDriver::%s(), start\n", __FUNCTION__);
    while(1) {
        getRadarStatus(m_RadarStatus);
        usleep(nBaseFrequence);
    }
}

int RadarDriver::getDisAndWarn(RadarStatus& radar) 
{
    radar = m_RadarStatus;
    return 0;
}
