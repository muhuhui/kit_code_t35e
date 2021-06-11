#ifndef PDDMOTORDRIVER_H
#define PDDMOTORDRIVER_H

#include "Common/TcpClient/TcpCltSession.h"
#include "base/SingleTon.h"
#include <stdio.h> 
#include <string>
#include <unistd.h>

typedef enum {
    PDD_GO_FORWARD = 0,
    PDD_GO_BACK,
    PDD_STOP,
    PDD_GO_TO,
    PDD_CALI
} PDDMotorCmd;

typedef enum {
    PDD_STOPPED = 0,
    PDD_MOVING
} PDDMoveStat;

typedef struct {
    PDDMoveStat moveStat;
    int pos;
    int velocity;
    int warn;
} PDDMotorInfo;

class PDDMotorDriver
{
public:
    ~PDDMotorDriver();
    static PDDMotorDriver* getInstance();
    void init();

    std::string spliceReq(const char* reqName, int transId);
    std::string spliceReq(const char* reqName, int transId, PDDMotorCmd cmd, int pos = 0);
    int parseRes(std::string res, TcpResp& result);
    int parseRes(std::string res, TcpResp& result, PDDMotorInfo& stat);

    int getPDDStatus(PDDMotorInfo& stat);
    int ctrlPDD(PDDMotorCmd cmd, int pos = 0);

    // 手动前进
    int manualForward();
    // 手动后退
    int manualBackward();
    // 移动到给定位置
    int gotoPosition(int pos);
    // 手动停止
    int manualStop();
    // 获取当前绝对位置
    int getCurPos() { return m_motorInfo.pos; }
    // 校准
    int calibration();

    PDDMoveStat getStat() { return m_moveStat; }

private:
    TcpClient* m_tcpClient;
    PDDMoveStat m_moveStat;
    PDDMotorInfo m_motorInfo;
};

#endif // PDDMOTORDRIVER_H
