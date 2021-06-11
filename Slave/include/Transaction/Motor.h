#ifndef MOTOR_H
#define MOTOR_H

#include "Driver/MotorDriver.h"
enum MotionType 
{
    Motion_Stop = 0,       //停止
    Motion_Forward,        //前进
    Motion_Backward,       //后退
    Motion_StepForward,    //点进
    Motion_StepBackward,   //点退
    Motion_EmergencyStop,  //紧急停止（停止）
    Motion_Goto,           //移动到某位置
    Motion_Lock,           //锁定（急停）
    Motion_Unlock,         //解锁（使能）
    Motion_Arm_Up,         //升降臂上升
    Motion_Arm_Down,       //升降臂下降
    Motion_Arm_Stop,       //升降臂停止 
    Motion_Arm_MoveTo,     //升降臂移动到某位置
    Motion_PDD_Forward,    //局放前进
    Motion_PDD_Backward,   //局放后退
    Motion_PDD_Stop,       //局放停止
};

class Motor
{
public:
    ~Motor();
    static Motor *getInstance();
    int init();
    int motionControl(MotionType motionType, float value,std::string posName="");
    int setMotionMode(RunMode mode) { return 0; }

    int getCurStatus();
    int getArmCurStatus();
    float getCurrent();
    float getArmCurrent();
    float getCurPosition(int index = 0);
    float getSpeed(int index = 0);
    float getArmSpeed(int index = 0);
    int setSpeed(float fSpeed);       
    int forward();
    int backward();
    int stepForward();
    int stepBackward();
    int stop();
    int emergencyStop();
    int armEmergencyStop();
    int goTo(float pos,std::string posName="");
    int setDefaultSpeed(float speed);
    int setEndPos(float pos);
    int setBeginPos(float pos);
    int setCurPos(float pos);
    int setAcc(float acc);
    int setDec(float dec);
    int setMaxCurrent(float current);   
    int getMotorWarn();
    int getArmMotorWarn();
    std::string getPosName();
    bool isGoingHomingStatus();
    
    int armUp();
    int armDown();
    int armStop();
    int armMoveTo(float pos);
    float getArmCurPosition();
    int setArmBeginPos(float fPos);
    int setArmEndPos(float fPos);
    float getMoveTemp();
    float getArmTemp();

    int armEnable();
    int armDisable();
    int enable();
    int disable();
    MotorInfo getMotorInfo();
    MotorInfo getArmMotorInfo();
    bool isInState(MotorIndex ind, MoveState state);
    int manualGotoHome();

    int disableSeerMotor() { return 0; }

    float m_fCurSpeed;
    float m_fPrevSpeed;
    MotorDriver*    m_pMotorDriver;
};

#endif // MOTOR_H
