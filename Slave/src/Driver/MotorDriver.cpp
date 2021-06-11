#include "Driver/MotorDriver.h"
#include "Driver/MyLog4CXX.h"
#include "ThirdParty/jsoncpp/json.h"
#include <string.h>
#include <thread>
#include <unistd.h>
#include <sys/time.h>

const std::string LOG_TAG("root.MotorDriver::");

MotorDriver*  g_pMotorDriver = NULL;
int g_nUnit = 1;

int g_nMotorId[TOTAL_MOTOR];


MotorDriver::MotorDriver()
{
}

MotorDriver::~MotorDriver()
{
}

MotorDriver *MotorDriver::getInstance()
{
  return SingleTon<MotorDriver>::GetInstance();
}

int MotorDriver::init(int motorId[], int motorNum)
{
    g_pMotorDriver = this;
    memset(m_motorInfo, 0, sizeof(m_motorInfo));

    for (int num = 0; num < motorNum && num < (int)sizeof(g_nMotorId); num++) {
        g_nMotorId[num] = motorId[num];
    }

    TcpCltSession *session = TcpCltSession::getInstance();
    m_tcpClient = session->getSession(TCP_MAIN_BOARD);

    std::thread updateInfoThread;
    updateInfoThread = std::thread(MotorDriver::updateInfo, this);
    updateInfoThread.detach();  

    return 0;
}

std::string MotorDriver::spliceReq(const char* reqName, int transId)
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

std::string MotorDriver::spliceReq(const char* reqName, int transId, MotorCmd cmd, int pos)
{
    std::string req;
    Json::Value val;
    Json::Value array;
    val["transId"] = std::to_string(transId);
    val["reqName"] = reqName;
    val["cmd"] = cmd;
    if (cmd == GOTO_CMD) {
        val["pos"] = pos;
    }
    req = val.toStyledString();
    req.erase(std::remove_if(req.begin(), req.end(), 
                [](char c) -> bool { return (c == '\n' || c == '\t' || c == '\r' || c == ' '); }), req.end());
    return req;
}

std::string MotorDriver::spliceReq(const char* reqName, int transId, std::map<std::string, float> para)
{
    std::string req;
    Json::Value val;
    val["transId"] = std::to_string(transId);
    val["reqName"] = reqName;
    Json::Value paraVal;
    for (auto iter = para.begin(); iter != para.end(); iter++) {
        paraVal[iter->first] = iter->second;
    }
    val["para"] = paraVal;
    req = val.toStyledString();
    req.erase(std::remove_if(req.begin(), req.end(), 
                [](char c) -> bool { return (c == '\n' || c == '\t' || c == '\r' || c == ' '); }), req.end());
    return req;
}

int MotorDriver::parseRes(std::string res, TcpResp& result)
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

int MotorDriver::parseRes(std::string res, TcpResp& result, MotorInfo& info)
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
            Json::Value resval = val["result"];
            info.fCurrent = resval["current"].asInt();
            info.fMotorTemp = resval["temp"].asInt();
            info.fSpeed = resval["velc"].asInt();
            info.fCurPos = resval["pos"].asInt();
            info.nErrNum = resval["warning"].asInt();
            info.state = (MoveState)resval["moveState"].asInt();
            info.mode = (OperationMode)resval["operaMode"].asInt();
        }
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }
    JSON_CATCH
    return result.errcode;
}

int MotorDriver::parseRes(std::string res, TcpResp& result, MotorCfgPara& para)
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

        if ((!val["result"].isNull()) && (result.errcode == 0)) {
            Json::Value resval = val["result"];
            para.nSoftBegin = resval["softBegin"].asInt();
            para.nSoftEnd = resval["softEnd"].asInt();
            para.nSpeed = resval["velc"].asInt();
            para.nNormCurrent = resval["normlCur"].asInt();
            para.nMaxCurrent = resval["maxCur"].asInt();
            para.nAccel = resval["accel"].asInt();
            para.nDecel = resval["decel"].asInt();
        }
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }
    JSON_CATCH
    return result.errcode;
}

int MotorDriver::getMotorStatus(MotorIndex idx, MotorInfo& info)
{
    std::string req = "";
    struct timeval tv;
    gettimeofday(&tv, NULL);
    switch (idx) {
        case WALK_MOTOR:
            req = spliceReq("moveMotorStatus", tv.tv_usec);
            break;
        case LIFT_MOTOR:
            req = spliceReq("liftMotorStatus", tv.tv_usec);
            break;
        default:
            break;
    }
    std::string res = "";
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

    if (info.state == JOG_BACK_STATE) { info.state = BACK_STATE; }
    else if (info.state == JOG_AHEAD_STATE) { info.state = AHEAD_STATE; }
    m_motorInfo[idx] = info;

    return 0;
}

int MotorDriver::motorCtrl(MotorIndex idx, MotorCmd cmd, int pos)
{
    std::string req = "";
    struct timeval tv;
    gettimeofday(&tv, NULL);
    switch (idx) {
        case WALK_MOTOR:
            req = spliceReq("moveMotorCtrl", tv.tv_usec, cmd, pos);
            break;
        case LIFT_MOTOR:
            req = spliceReq("liftMotorCtrl", tv.tv_usec, cmd, pos);
            break;
        default:
            break;
    }

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

int MotorDriver::setMotorConfig(MotorIndex idx, std::map<std::string, float> para)
{
    std::string req = "";
    struct timeval tv;
    gettimeofday(&tv, NULL);
    switch (idx) {
        case WALK_MOTOR:
            req = spliceReq("moveMotorConfig", tv.tv_usec, para);
            break;
        case LIFT_MOTOR:
            req = spliceReq("liftMotorConfig", tv.tv_usec, para);
            break;
        default:
            break;
    }

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

int MotorDriver::getMotorConfig(MotorIndex idx, MotorCfgPara& para)
{
    std::string req = "";
    struct timeval tv;
    gettimeofday(&tv, NULL);
    switch (idx) {
        case WALK_MOTOR:
            req = spliceReq("moveMotorRead", tv.tv_usec);
            break;
        case LIFT_MOTOR:
            req = spliceReq("liftMotorRead", tv.tv_usec);
            break;
        default:
            break;
    }

    std::string res;

    int nRet = m_tcpClient->request(req, res);
    if (nRet != 0) {
        return -1;
    }

    TcpResp result;
    result.transId = std::to_string(tv.tv_usec);
    nRet = parseRes(res, result, para);
    if (nRet != 0) {
        return -1;
    }
    m_motorCfg[idx] = para;
    return 0;
}

int MotorDriver::forward(MotorIndex idx)
{
    return motorCtrl(idx, GO_AHEAD_CMD);
}

int MotorDriver::stepForward(MotorIndex idx)
{
    return motorCtrl(idx, JOG_AHEAD_CMD);
}

int MotorDriver::backward(MotorIndex idx)
{ 
    return motorCtrl(idx, GO_BACK_CMD);
}

int MotorDriver::stepBackward(MotorIndex idx)
{
    return motorCtrl(idx, JOG_BACK_CMD);
}

int MotorDriver::stop(MotorIndex idx)
{
    return motorCtrl(idx, STOP_CMD);
}

int MotorDriver::emergeStop(MotorIndex idx)
{
    return motorCtrl(idx, EMERGENCY_STOP_CMD);
}

int MotorDriver::enable(MotorIndex idx)
{
    return motorCtrl(idx, ENABLE_CMD);
}

int MotorDriver::disable(MotorIndex idx)
{
    return motorCtrl(idx, DISABLE_CMD);
}

int MotorDriver::goTo(MotorIndex idx, float pos)
{
    return motorCtrl(idx, GOTO_CMD, pos);
}

int MotorDriver::homing(MotorIndex idx)
{
    return motorCtrl(idx, HOMING_CMD);
}

int MotorDriver::setSpeed(MotorIndex idx, float speed)
{
    std::map<std::string, float> cfg;
    cfg["velc"] = speed;
    return setMotorConfig(idx, cfg);
}

int MotorDriver::setAcceleration(MotorIndex idx, float acc)
{
    std::map<std::string, float> cfg;
    cfg["accel"] = acc;
    return setMotorConfig(idx, cfg);
}

int MotorDriver::setDeceleration(MotorIndex idx, float dec)
{
    std::map<std::string, float> cfg;
    cfg["decel"] = dec;
    return setMotorConfig(idx, cfg);
}

int MotorDriver::setBeginPos(MotorIndex idx, float pos)
{
    std::map<std::string, float> cfg;
    cfg["softBegin"] = pos;
    return setMotorConfig(idx, cfg);
}

int MotorDriver::setEndPos(MotorIndex idx, float pos)
{
    std::map<std::string, float> cfg;
    cfg["softEnd"] = pos;
    return setMotorConfig(idx, cfg);
}

int MotorDriver::setNominalCurrent(MotorIndex idx, float current)
{
    std::map<std::string, float> cfg;
    cfg["normlCur"] = current;
    return setMotorConfig(idx, cfg);
}

int MotorDriver::setMaxCurrent(MotorIndex idx, float current)
{
    std::map<std::string, float> cfg;
    cfg["maxCur"] = current;
    return setMotorConfig(idx, cfg);
}

void MotorDriver::attach(Observers *pObserver)
{
  m_ObserverList.push_back(pObserver);
}

void MotorDriver::detach(Observers *pObserver)
{
  m_ObserverList.remove(pObserver);
}

void MotorDriver::notify()
{
  std::list<Observers *>::iterator it = m_ObserverList.begin();
  while (it != m_ObserverList.end())
  {
    (*it)->update(m_eSubType);
    ++it;
  }
} 

void MotorDriver::updateInfo(MotorDriver* pMotorDriver)
{
    static int lastWarn[TOTAL_MOTOR] = {0};
    printf("MotorDriver::%s(), start\n", __FUNCTION__);
    while (1)
    {
        for (unsigned int i = 0; i < sizeof(g_nMotorId) / sizeof(g_nMotorId[0]); i++) {
            MotorInfo info;
            int nRet = -1;
            switch (g_nMotorId[i]) {
                case WALK_MOTOR:
                    nRet = pMotorDriver->getMotorStatus(WALK_MOTOR, info);
                    pMotorDriver->m_eSubType = SUB_MOTOR_WARN;
                    break;
                case LIFT_MOTOR:
                    nRet = pMotorDriver->getMotorStatus(LIFT_MOTOR, info);
                    pMotorDriver->m_eSubType = SUB_LIFT_MOTOR_WARN;
                    break;
                default:
                    break;
            }
            if (nRet != 0) {
                continue;
            }

            int curWarn = pMotorDriver->getCurErrorNum((MotorIndex)g_nMotorId[i]);
            if ((curWarn != lastWarn[i]) && (curWarn != 0)) {
                pMotorDriver->notify();
                lastWarn[i] = curWarn;
            }
        }
        usleep(500000);
    }
}

