#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H

#include "Common/TcpClient/TcpCltSession.h"
#include "base/SingleTon.h"
#include "base/Subjects.h"
#include "base/Observers.h"
#include <string>
#include "Transaction/Setting.h"
#include <map>

typedef enum { 
    RET_SUCCESS = 0,
    RET_COMM_FAIL = -1, 
    RET_ACT_FAIL = -2, 
    RET_ULTRA_DECT = -3, 
} RetRes;

typedef enum {
    WALK_MOTOR = 0,
    LIFT_MOTOR,
    TOTAL_MOTOR
} MotorIndex;

typedef enum 
{
    GO_AHEAD_CMD = 0,
    GO_BACK_CMD,
    JOG_AHEAD_CMD,
    JOG_BACK_CMD,
    STOP_CMD,
    EMERGENCY_STOP_CMD,
    GOTO_CMD,
    ENABLE_CMD,
    DISABLE_CMD,
    HOMING_CMD
} MotorCmd;

typedef enum {
    OPERA_CALI_MODE = 0,
    OPERA_RUN_MODE = 1,
} OperationMode;

typedef enum {
    LOCK_STATE = -3,
    BACK_STATE = -2,
    JOG_BACK_STATE = -1,
    STOP_STATE = 0,
    JOG_AHEAD_STATE = 1,
    AHEAD_STATE = 2,
    HOMING_STATE = 3,
    OPEN_DOOR,
    CLOSE_DOOR,
} MoveState;

typedef struct {
    int nSoftBegin;
    int nSoftEnd;
    int nSpeed;
    int nNormCurrent;
    int nMaxCurrent;
    int nAccel;
    int nDecel;
} MotorCfgPara;

typedef struct {
    float fCurrent;
    float fMotorTemp;
    float fSpeed;
    float fCurPos;
    int nErrNum;
    MoveState state;
    OperationMode mode;
} MotorInfo;

class MotorDriver: public Subjects
{
public:
    MotorDriver();
    ~MotorDriver();
    static MotorDriver *getInstance();

    int init(int nMotorId[], int motorNum);

    std::string spliceReq(const char* reqName, int transId);
    std::string spliceReq(const char* reqName, int transId, MotorCmd cmd, int pos);
    std::string spliceReq(const char* reqName, int transId, std::map<std::string, float> para);
    int parseRes(std::string res, TcpResp& result);
    int parseRes(std::string res, TcpResp& result, MotorInfo& info);
    int parseRes(std::string res, TcpResp& result, MotorCfgPara& para);

    int getMotorStatus(MotorIndex idx, MotorInfo& info);
    int motorCtrl(MotorIndex idx, MotorCmd cmd, int pos = 0);
    int getMotorConfig(MotorIndex idx, MotorCfgPara& para);
    int setMotorConfig(MotorIndex idx, std::map<std::string, float> para);

    // ctrl
    int forward(MotorIndex idx);
    int stepForward(MotorIndex idx);
    int backward(MotorIndex idx);
    int stepBackward(MotorIndex idx);
    int stop(MotorIndex idx);
    int emergeStop(MotorIndex idx);
    int goTo(MotorIndex idx, float pos);
    int enable(MotorIndex idx);
    int disable(MotorIndex idx);
    int homing(MotorIndex idx);

    // info
    MotorInfo getMotorInfo(MotorIndex idx) { return m_motorInfo[idx]; }
    float getCurPos(MotorIndex idx) { return m_motorInfo[idx].fCurPos; }
    float getCurrent(MotorIndex idx) { return m_motorInfo[idx].fCurrent; }
    float getMotorTemp(MotorIndex idx) { return m_motorInfo[idx].fMotorTemp; }
    float getCurSpeed(MotorIndex idx) { return m_motorInfo[idx].fSpeed; }
    MoveState getCurStatus(MotorIndex idx) { return m_motorInfo[idx].state; }
    OperationMode getOperaMode(MotorIndex idx) { return m_motorInfo[idx].mode; }
    int getCurErrorNum(MotorIndex idx) { return m_motorInfo[idx].nErrNum; }

    // setconfig
    int setSpeed(MotorIndex idx, float speed);
    int setBeginPos(MotorIndex idx, float pos);
    int setEndPos(MotorIndex idx, float pos);
    int setAcceleration(MotorIndex idx, float acc);
    int setDeceleration(MotorIndex idx, float dec);
    int setNominalCurrent(MotorIndex idx, float current);
    int setMaxCurrent(MotorIndex idx, float current);

    void attach(Observers *pObserver);
    void detach(Observers *pObserver);
    void notify();

    static void updateInfo(MotorDriver* pMotorDriver);
public:
    SubType                   m_eSubType;
    TcpClient*                m_tcpClient;
private:
    std::list<Observers* >    m_ObserverList;
    MotorCfgPara              m_motorCfg[TOTAL_MOTOR];
    MotorInfo                 m_motorInfo[TOTAL_MOTOR];
};

#endif // MOTORDRIVER_H

