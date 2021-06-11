//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : T3C_SLAVE
//  @ File Name : MoveControlCommand.cpp
//  @ Date : 2019-07-02
//  @ Author : ShunWang
//
//


#include "MoveControlCommand.h"
#include "Driver/MotorDriver.h"
#include "Transaction/SettingService.h"
#include "Motor.h"
#include "Driver/MyLog4CXX.h"

const std::string LOG_TAG("root.MoveControlCommand::");

int MoveControlCommand::exec()
{
    std::string logname = LOG_TAG + std::string(__FUNCTION__);
    auto logger = initLogger(logname);
	//解析JSON参数
	int ctrl = _paraJson["ctrl"].asInt();
    float fValue = _paraJson["value"].asFloat();
    std::string posName =  _paraJson["posName"].asString();

    Json::Value *sharedJson = getSharedParaJson();
    if(sharedJson == NULL) {
        printf("GotoHome::%s(), can't get json value!\n", __FUNCTION__);
        LOG4CPLUS_ERROR(logger, " can't get json value! ");
        setExeRet(ST_FAIL);
        return _exeRet;
    }

	if ((*sharedJson)[JSON_EXEC_RESULT].asInt() != ST_SUCCESS) {
        (*sharedJson)[JSON_EXEC_RESULT] = ST_FAIL;
        printf("PtzRotateCommand::%s(), last command execute failed, exit!\n", __FUNCTION__);
        LOG4CPLUS_ERROR(logger, " last command execute failed, exit! ");
        setExeRet(ST_FAIL);
        return _exeRet;
    }

    Motor *pMotor = Motor::getInstance();
    SettingService *settingService = SettingService::getInstance();
	int ret = 0;
	//printf("_exeRet = %d, __FILE__ =%s, __LINE__=%d\n",_exeRet,__FILE__,__LINE__);
	if(_exeRet != ST_SUCCESS && _exeRet != ST_FAIL)
	{
	    switch(_exeStatus)
        {
            case MOVE_COMMAND:
                printf("ctrl = %d, __FILE__ =%s, __LINE__=%d\n",ctrl,__FILE__,__LINE__);
                LOG4CPLUS_INFO(logger, " command, ctrl: " << ctrl );
                switch(ctrl)
                {
                    case Motion_Stop:
                    {
                        ret = pMotor->stop();
                        break;
                    }
                    case Motion_Forward:
                    {
			            // pMotor->armMoveTo(0);
                        ret = pMotor->forward();
                        break;
                    }
                    case Motion_Backward:
                    {
			            // pMotor->armMoveTo(0);
                        ret = pMotor->backward();
                        break;
                    }
                    case Motion_StepBackward:
                    {
                        ret = pMotor->stepBackward();
                        break;
                    }
                    case Motion_StepForward:
                    {
                        ret = pMotor->stepForward();
                        break;
                    }
                    case Motion_EmergencyStop:
                    {
                        ret = pMotor->emergencyStop();
                        break;
                    }
                    case Motion_Goto:
                    {
                        printf("move control command: move to %f\n", fValue);
                        LOG4CPLUS_INFO(logger, " move control command: move to " << fValue );
                        ret = pMotor->goTo(fValue,posName);
                        break;
                    }
                    case Motion_Lock:
                    {
                        printf("move control command: lock\n");
                        ret = pMotor->stop();
                        settingService->m_setting.setMotorLock(true);
                        break;
                    }
                    case Motion_Unlock:
                    {
                        printf("move control command: unlock\n");
                        break;
                    }
                    case Motion_Arm_Up:
                    {
                        printf("move control command: arm up\n");
                        // pMotor->m_pMotorDriver->arm_enable();
                        ret = pMotor->armUp();
                        break;
                    }
                    case Motion_Arm_Down:
                    {
                        printf("move control command: arm down\n");
                        // pMotor->m_pMotorDriver->arm_enable();
                        ret = pMotor->armDown();
                        break;
                    }
                    case Motion_Arm_Stop:
                    {
                        printf("move control command: arm stop\n");
                        ret = pMotor->armStop();
                        // pMotor->m_pMotorDriver->arm_disable();
                        break;
                    }
                    case Motion_Arm_MoveTo:
                    {
                        printf("move control command: arm move to %f\n", fValue);
                        LOG4CPLUS_INFO(logger, " move control command: arm move to " << fValue );
                        ret = pMotor->armMoveTo(fValue);
                        break;
                    }
                    default:
                    {
                        break;
                    }			
                }
                if(ret == RET_SUCCESS)
                {
                    // if(ctrl == Motion_Goto || 
                    //    ctrl == Motion_Forward || 
                    //    ctrl == Motion_Backward)
                    // {
                    //     _exeStatus = MOVE_CHECK;
                    // }
                    // else {
                    //     setExeRet(ST_SUCCESS);
                    // }
                    _tryCnt = 0;
                    switch (ctrl)
                    {
                        case Motion_Goto:
                        case Motion_Forward:
                        case Motion_Backward:
                        {
                            _exeStatus = MOVE_CHECK;
                            LOG4CPLUS_INFO(logger, " move control command check :" << ctrl );
                            break;
                        }
                        case Motion_Arm_Up:
                        case Motion_Arm_Down:
                        case Motion_Arm_MoveTo:
                        {
                            _exeStatus = LIFT_CHECK;
                            LOG4CPLUS_INFO(logger, " move control command check :" << ctrl );
                            break;
                        }
                        default:
                        {
                            (*sharedJson)[JSON_EXEC_RESULT] = ST_SUCCESS;
                            setExeRet(ST_SUCCESS);
                            break;
                        }
                    }
                }
                else
                {
                    _tryCnt++;
                    if (_tryCnt >= 3) {
                        (*sharedJson)[JSON_EXEC_RESULT] = ST_FAIL;
                        setExeRet(ST_FAIL);	
                    }
                }
                break;
            case MOVE_CHECK:
                //printf("motion control check\n");
                if(0 == sleeepWaitTimeOut(10)) {
                    if(pMotor->getCurStatus() == STOP_STATE) {
                        (*sharedJson)[JSON_EXEC_RESULT] = ST_SUCCESS;
                        setExeRet(ST_SUCCESS);
                    }
                }
                break;
            case LIFT_CHECK:
                if (0 == sleeepWaitTimeOut(10)) {
                    if (pMotor->getArmCurStatus() == STOP_STATE
                        && fabs(pMotor->getArmCurPosition() - fValue) < 10) {
                        (*sharedJson)[JSON_EXEC_RESULT] = ST_SUCCESS;
                        setExeRet(ST_SUCCESS);
                    }
                    _tryCnt++;
                    if (_tryCnt > 60) {
                        setExeRet(ST_FAIL);
                    }
                }
                break;
            default:
                break;
		}
	}
	return _exeRet;
}
