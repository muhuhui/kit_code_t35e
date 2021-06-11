#ifndef RADARDRIVER_H
#define RADARDRIVER_H

#include "Common/TcpClient/TcpCltSession.h"
#include "SingleTon.h"
#include <stdio.h> 
#include <string>
#include <unistd.h>

enum RadarNum {
    RADAR_FRONT = 0,
    RADAR_BACK,
    RADAR_DOWN,
    RADAR_TOTAL
};

//获取雷达的检测距离和状态:左、右、下雷达
struct RadarStruct{
    float distance;
    bool isBlock;
};
struct RadarStatus{
    RadarStruct radarFront;
    RadarStruct radarBack;
    RadarStruct radarBottom;
};
//雷达的配置参数
struct RadarConfig{
    int id;
    float blindZoneMin;
    float blindZoneMax;
    float warnDis;
    int enable;
};
struct RadarConfigData{
    RadarConfig m_radarConfigFront;
    RadarConfig m_radarConfigBack;
    RadarConfig m_radarConfigBottom;
};

class RadarDriver
{
public:
    ~RadarDriver();
    static RadarDriver* getInstance();

    void init();
    //获得Radar状态和获得雷达配置信息请求
    std::string spliceReq(const char* reqName, int transId);
    //获取雷达配置信息请求
    std::string spliceReq(const char* reqName,std::vector<RadarConfig> radarConfig,int transId);
    //获得雷达状态解析
    int parseRes(std::string res, TcpResp& result, RadarStatus& radar);
    //获取雷达配置
    int parseRes(std::string res, TcpResp& result);
    //获取雷达配置解析
    int parseRes(std::string res, TcpResp& result, RadarConfigData radarConfig);
    //提供雷达的距离
    int getRadarStatus(RadarStatus& radar);
    //雷达设置写入
    int setRadarConfig(std::vector<RadarConfig> radarConfig);
    int radarSwitch(RadarNum num, int enable);
    //读取雷达的配置
    int readRadarConfig(RadarConfigData &radarConfig);
    //获取距离与告警
    int getDisAndWarn(RadarStatus& radar);
    //雷达状态轮询
    void updateInfo();
private:
    RadarStatus m_RadarStatus;
    TcpClient* m_tcpClient;
};

#endif // RADARDRIVER_H
