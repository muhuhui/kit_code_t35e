#ifndef PTZDRIVER_H
#define PTZDRIVER_H

#include "Common/TcpClient/TcpCltSession.h"
#include <mutex>
#include <memory>
#include <vector>

typedef enum {
    CAM_MOTOR = 0,
    INFRA_MOTOR,
    HORIZ_MOTOR,
    DISCHARGE_MOTOR,
    PTZ_TOTAL_MOTOR
} PTZMotorNum;

typedef enum {
    TURN_UP = 0,
    TURN_DOWN,
    TURN_TO,
    TURN_HOME,
    TURN_STOP
} PTZMotorCmd;

typedef enum {
    STOPPED = 0,
    MOVING,
    HOMING
} PTZMoveStat;

typedef struct {
    PTZMotorNum motorId;
    PTZMotorCmd cmd;
    int angle;
} PTZMotorCtrl;

typedef struct {
    PTZMotorNum motorId;
    PTZMoveStat moveStat;
    int angle;
} PTZMotorStat;

class PTZDriver
{
public:
    using Ptr = std::shared_ptr<PTZDriver>;

    PTZDriver();
    PTZDriver(const PTZDriver&) = delete;
    ~PTZDriver();

    int init();
    static PTZDriver::Ptr getInstance();

    std::string spliceReq(const char* reqName, int transId);
    std::string spliceReq(const char* reqName, int transId, std::vector<PTZMotorCtrl> ctrl);
    int parseRes(std::string res, TcpResp& result);
    int parseRes(std::string res, TcpResp& result, std::vector<PTZMotorStat>& stat);

    int getPTZCoordinate(std::vector<int>& coord);
    int getPTZStatus(std::vector<PTZMotorStat>& stat);
    int ctrlPTZ(PTZMotorNum num, PTZMotorCmd cmd, int angle);
    int gotoPreset(int preset[], int size);
    static void updateInfo(PTZDriver* pPTZDriver);

private:
    TcpClient* m_tcpClient;
    static void run(PTZDriver::Ptr pPTZDriver);
    std::vector<PTZMotorStat> m_stat;
    int m_nError;
    std::mutex m_mutex;
};




#endif
