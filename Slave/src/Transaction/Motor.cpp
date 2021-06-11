#include "Transaction/Motor.h"
#include "Transaction/Robot.h"
#include "Transaction/MoveTaskManage/MoveTaskManage.h"
#include "Transaction/MySQLServer.h"
#include "Transaction/ConfigServer.h"
#include "Transaction/MoveTaskManage/RFIDService.h"
#include "Driver/MyLog4CXX.h"

const std::string LOG_TAG("root.Motor::");

Motor::~Motor()
{

}

//单例化
Motor *Motor::getInstance()
{
    return SingleTon<Motor>::GetInstance();
}

//初始化
int Motor::init()
{
    std::string logname = LOG_TAG + std::string(__FUNCTION__);
    auto logger = initLogger(logname);
    LOG4CPLUS_INFO(logger, " begin.");

    SettingService *settingService = SettingService::getInstance();
    m_pMotorDriver = MotorDriver::getInstance();
    ConfigServer* pConfigServer = ConfigServer::getInstance();
    int motorId[] = { WALK_MOTOR, LIFT_MOTOR };
    m_pMotorDriver->init(motorId, sizeof(motorId) / sizeof(motorId[0]));

    //从数据库获取速度参数
    float speed = settingService->m_setting.getDefaultSpeed();
    float beginPos = settingService->m_setting.getBeginPos();
    float endPos = settingService->m_setting.getEndPos();
    /* float acc = settingService->m_setting.getAcc();
    float dec = settingService->m_setting.getDec(); */
    float armBeginPos = settingService->m_setting.getArmBeginPos();
    float armEndPos = settingService->m_setting.getArmEndPos();

    setDefaultSpeed(speed);
    setBeginPos(beginPos);
    setEndPos(endPos);
    /* setAcc(acc);
    setDec(dec); */
    setArmBeginPos(armBeginPos);
    setArmEndPos(armEndPos);

    m_fCurSpeed = speed;
    m_fPrevSpeed = speed;

    LOG4CPLUS_INFO(logger, " end.");

    return 0;
}

//运动控制
int Motor::motionControl(MotionType motionType, float value,std::string posName)
{
	MoveTaskManage* moveTaskManage = MoveTaskManage::getInstance();
	return moveTaskManage->manulMoveControl((int &)motionType, value,posName);
}

/*************** 行走电机部分 ***************/

//获取当前状态
int Motor::getCurStatus()
{
	return m_pMotorDriver->getCurStatus(WALK_MOTOR);
}

//获取当前位置
float Motor::getCurPosition(int index)
{
	return m_pMotorDriver->getCurPos(WALK_MOTOR);
}

//获取当前速度
float Motor::getSpeed(int index)
{
	return m_pMotorDriver->getCurSpeed(WALK_MOTOR);
}

//设置速度
int Motor::setSpeed(float fSpeed)
{
	return m_pMotorDriver->setSpeed(WALK_MOTOR, fSpeed);
}

//前进
int Motor::forward()
{
    int nRet = RET_SUCCESS;
    RadarManager *pRadar = RadarManager::getInstance();
    if (pRadar->getWarnStatus(RADAR_FRONT)) {
        nRet = RET_ULTRA_DECT;
    }
    else {
        nRet = m_pMotorDriver->forward(WALK_MOTOR);
    }
    return nRet;
}

//后退
int Motor::backward()
{
    int nRet = RET_SUCCESS;
    RadarManager *pRadar = RadarManager::getInstance();
    if (pRadar->getWarnStatus(RADAR_BACK)) {
        nRet = RET_ULTRA_DECT;
    }
    else {
        nRet = m_pMotorDriver->backward(WALK_MOTOR);
    }
    return nRet;
}

//点进
int Motor::stepForward()
{
    int nRet = RET_SUCCESS;
    RadarManager *pRadar = RadarManager::getInstance();
    if (pRadar->getWarnStatus(RADAR_FRONT)) {
        nRet = RET_ULTRA_DECT;
    }
    else {
        nRet = m_pMotorDriver->stepForward(WALK_MOTOR);
    }
    return nRet;
}

//点退
int Motor::stepBackward()
{
    int nRet = RET_SUCCESS;
    RadarManager *pRadar = RadarManager::getInstance();
    if (pRadar->getWarnStatus(RADAR_BACK)) {
        nRet = RET_ULTRA_DECT;
    }
    else {
        nRet = m_pMotorDriver->stepBackward(WALK_MOTOR);
    }
    return nRet;
}

//停止
int Motor::stop()
{
    return m_pMotorDriver->stop(WALK_MOTOR);
}

//急停
int Motor::emergencyStop()
{
    return m_pMotorDriver->emergeStop(WALK_MOTOR);
}

int Motor::armEmergencyStop()
{
    return m_pMotorDriver->emergeStop(LIFT_MOTOR);
}

//行走到某个位置
int Motor::goTo(float pos, std::string posName)
{
	return m_pMotorDriver->goTo(WALK_MOTOR, pos);
}

//设置默认速度
int Motor::setDefaultSpeed(float speed)
{
    return m_pMotorDriver->setSpeed(WALK_MOTOR, speed);
}

//设置终点位置
int Motor::setEndPos(float pos)
{
    return m_pMotorDriver->setEndPos(WALK_MOTOR, pos);
}
//设置起点位置
int Motor::setBeginPos(float pos)
{
    return m_pMotorDriver->setBeginPos(WALK_MOTOR, pos);
}

//设置加速度
int Motor::setAcc(float acc)
{
    return m_pMotorDriver->setAcceleration(WALK_MOTOR, acc);
}

//设置减速度
int Motor::setDec(float dec)
{
    return m_pMotorDriver->setDeceleration(WALK_MOTOR, dec);
}

//设置当前位置
int Motor::setCurPos(float pos)
{
    return 0;
}

//设置限流值（最大可达到的电流）
int Motor::setMaxCurrent(float current)
{
    return m_pMotorDriver->setMaxCurrent(WALK_MOTOR, current);
}

//获得驱动器告警信息
int Motor::getMotorWarn()
{
	return m_pMotorDriver->getCurErrorNum(WALK_MOTOR);
}

/*************** 升降电机部分 ***************/

int Motor::getArmMotorWarn()
{
	return m_pMotorDriver->getCurErrorNum(LIFT_MOTOR);
}
//升降臂上升
int Motor::armUp()
{
    return m_pMotorDriver->forward(LIFT_MOTOR);
}

//升降臂下降
int Motor::armDown()
{
    return m_pMotorDriver->backward(LIFT_MOTOR);
}

//升降臂停止
int Motor::armStop()
{
    return m_pMotorDriver->stop(LIFT_MOTOR);
}

//升降臂移动到某位置
int Motor::armMoveTo(float pos)
{
    return m_pMotorDriver->goTo(LIFT_MOTOR, pos);
}

//获取升降臂当前位置
float Motor::getArmCurPosition()
{
    return m_pMotorDriver->getCurPos(LIFT_MOTOR);
}

int Motor::getArmCurStatus()
{
    return m_pMotorDriver->getCurStatus(LIFT_MOTOR);
}

float Motor::getMoveTemp()
{
    return m_pMotorDriver->getMotorTemp(LIFT_MOTOR);
}

float Motor::getArmTemp()
{
    return m_pMotorDriver->getMotorTemp(LIFT_MOTOR);
}

//设置升降臂起点位置
int Motor::setArmBeginPos(float fPos)
{
    return m_pMotorDriver->setBeginPos(LIFT_MOTOR, fPos);
}

//设置升降臂终点位置
int Motor::setArmEndPos(float fPos)
{
    return m_pMotorDriver->setEndPos(LIFT_MOTOR, fPos);
}

bool Motor::isInState(MotorIndex ind, MoveState state)
{
    return (state == m_pMotorDriver->getCurStatus(ind));
}

/*************** 通用电机部分 ***************/
//读取电机电流
float Motor::getCurrent()
{
    return m_pMotorDriver->getCurrent(WALK_MOTOR);
}

float Motor::getArmCurrent()
{
    return m_pMotorDriver->getCurrent(LIFT_MOTOR);
}

std::string Motor::getPosName()
{
    return "";
    // return m_pMotorDriver->getPosName();
}

int Motor::manualGotoHome()
{
    return m_pMotorDriver->homing(LIFT_MOTOR);
}

bool Motor::isGoingHomingStatus()
{
   return (m_pMotorDriver->getCurStatus(LIFT_MOTOR) == HOMING_STATE);
}

MotorInfo Motor::getMotorInfo()
{
   return m_pMotorDriver->getMotorInfo(WALK_MOTOR);
}

MotorInfo Motor::getArmMotorInfo()
{
   return m_pMotorDriver->getMotorInfo(LIFT_MOTOR);
}

int Motor::enable()
{
   return m_pMotorDriver->enable(WALK_MOTOR);
}

int Motor::disable()
{
   return m_pMotorDriver->disable(WALK_MOTOR);
}

int Motor::armEnable()
{
   return m_pMotorDriver->enable(LIFT_MOTOR);
}

int Motor::armDisable()
{
   return m_pMotorDriver->disable(LIFT_MOTOR);
}

//获取当前速度
float Motor::getArmSpeed(int index)
{
	return m_pMotorDriver->getCurSpeed(LIFT_MOTOR);
}
