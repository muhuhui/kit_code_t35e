//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : T3C_SLAVE
//  @ File Name : PtzArmCommand.cpp
//  @ Date : 2019-07-02
//  @ Author : ShunWang
//
//

#include "PtzArmCommand.h"
#include "Transaction/Robot.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "Driver/MyLog4CXX.h"
#include "Driver/MotorDriver.h"
#include "Driver/PTZDriver.h"
#include "Transaction/ConfigServer.h"


const std::string LOG_TAG("root.PtzArmCommand::");

int PtzArmCommand::exec()
{
    std::string logname = LOG_TAG + std::string(__FUNCTION__);
    auto logger = initLogger(logname);

    int ptzPreset[4] = {0};
    std::vector<PTZMotorStat> ptzState;
    ptzState.clear();

	//解析JSON参数
	int presetNumber = _paraJson["presetNumber"].asInt();
    int armPosition = _paraJson["armPosition"].asInt();
    int pointNum = _paraJson["pointNum"].asInt();
    ptzPreset[0] = _paraJson["x_pos"].asInt();
	ptzPreset[1] = _paraJson["y_pos"].asInt();
    ptzPreset[2] = _paraJson["z_pos"].asInt();
	ptzPreset[3] = _paraJson["zoom"].asInt();

    Json::Value *sharedJson = getSharedParaJson();
    Robot *pRobot = Robot::getInstance();
    Motor *pMotor = Motor::getInstance();
    PTZDriver::Ptr pPtzMotor = PTZDriver::getInstance();
    int PtzWait = pRobot->m_pConfigServer->getTimeInfo()->PtzRotate;

    if(sharedJson == NULL) {
        printf("PtzArmCommand::%s(), can't get json value!\n", __FUNCTION__);
        LOG4CPLUS_ERROR(logger, " can't get json value! ");
        setExeRet(ST_FAIL);
        return _exeRet;
    }

    if ((*sharedJson)[JSON_EXEC_RESULT].asInt() != ST_SUCCESS) {
        printf("PtzArmCommand::%s(), last command execute failed, exit!\n", __FUNCTION__);
        LOG4CPLUS_ERROR(logger, " last command execute failed, exit! ");
        setExeRet(ST_FAIL);
        return _exeRet;
    }

	setExeRet(ST_GOING);
    switch(_exeStatus)
	{
		case ROTLIFT_ST_BEGAIN:
		{
            //升降臂移动到预置点
            int nRet = -1, nTry = 0;
            if (pRobot->m_pCapabilities->isSupportArmMotor()){
                Motor *pMotor = Motor::getInstance();
                nRet = pMotor->armMoveTo(armPosition);
                printf("PtzArmCommand::%s(), go to armPosition: %d, nRet: %d\n", __FUNCTION__, armPosition, nRet);
                LOG4CPLUS_INFO(logger, " go to armPosition: " << armPosition << ", nRet: " << nRet);              
            }
            nRet = -1;
            while (nRet != 0 && nTry < 3) {
                nTry++;
                nRet = pPtzMotor->gotoPreset(ptzPreset, sizeof(ptzPreset)/sizeof(int));
                printf("PtzArmCommand::%s(), presetOperation, number: %d, nRet: %d\n", __FUNCTION__, presetNumber, nRet);
                LOG4CPLUS_INFO(logger, " presetOperation, number: " << presetNumber << ", nRet: " << nRet);
            }
			clearTimer();
            _exeStatus = ROTLIFT_ST_WAIT;
			break;
		}
		case ROTLIFT_ST_WAIT:
		{
            if (pRobot->m_pCapabilities->isSupportArmMotor()){                       
                if(0 == sleeepWaitTimeOut(10)){
                    if(pMotor->isInState(LIFT_MOTOR, STOP_STATE)) {
                        printf("PtzArmCommand::%s(), go to armPosition succeed, armPosition:%d\n", __FUNCTION__, armPosition);
                        LOG4CPLUS_INFO(logger, "go to armPosition succeed, armPosition:" << armPosition);
                        _exeStatus = PTZ_ST_WAIT;
                    }   
                }
            }
            else {
                if(0 == sleeepWaitTimeOut(PtzWait)) {//1s等待到达
                    int nTry = 0, nRet = 0;
                    do {
                        nRet = pPtzMotor->getPTZStatus(ptzState);
                        if (nTry >= 5) {
                            setExeRet(ST_FAIL);
                            break;
                        }
                        nTry++;
                        usleep(100000);
                    } while (nRet != 0);

                    if (ptzState.size() < 3) {
                        setExeRet(ST_FAIL);
                        break;
                    }
                    if ((fabs(ptzPreset[0] - ptzState[0].angle) <= 1) && 
                        (fabs(ptzPreset[1] - ptzState[1].angle) <= 1) && 
                        (fabs(ptzPreset[2] - ptzState[2].angle) <= 1)) {
                        _exeStatus = ROTLIFT_ST_CAPTURE;
                        printf("PtzArmCommand::%s(), go to presetOperation succeed, number:%d\n", __FUNCTION__, presetNumber);
                        LOG4CPLUS_INFO(logger, "go to presetOperation succeed, number:" << presetNumber);
                    }
                }              
            }
			break;
		}
        case PTZ_ST_WAIT:
		{
            if(0 == sleeepWaitTimeOut(PtzWait)) {//1s等待到达
                int nTry = 0, nRet = 0;
                do {
                    nRet = pPtzMotor->getPTZStatus(ptzState);
                    if (nTry >= 5) {
                        setExeRet(ST_FAIL);
                        break;
                    }
                    nTry++;
                    usleep(100000);
                } while (nRet != 0);

                if (ptzState.size() < 3) {
                    setExeRet(ST_FAIL);
                    break;
                }
                if ((fabs(ptzPreset[0] - ptzState[0].angle) <= 1) && 
                    (fabs(ptzPreset[1] - ptzState[1].angle) <= 1) && 
                    (fabs(ptzPreset[2] - ptzState[2].angle) <= 1)) {
                    _exeStatus = ROTLIFT_ST_CAPTURE;
                    printf("PtzArmCommand::%s(), go to presetOperation succeed, number:%d\n", __FUNCTION__, presetNumber);
                    LOG4CPLUS_INFO(logger, "go to presetOperation succeed, number:" << presetNumber);
                }
            }    
			break;
		}
        case ROTLIFT_ST_CAPTURE:
		{
            int focus_time = 10;
            // 放大倍率大，多等1s
            if(ptzPreset[3] > 100 ) {
                focus_time = 20;
            }
            if(0 == sleeepWaitTimeOut(focus_time)) {//对焦时间
                _exeStatus = ROTLIFT_ST_SUCCESS;
            }
            break;
        }
		case ROTLIFT_ST_SUCCESS:
		{
            clearTimer();
            printf("PtzArmCommand::%s(), Completed! preset number:%d, Arm Pos:%d, Arm Actural Pos:%f\n", __FUNCTION__, presetNumber, armPosition, pRobot->m_pMotor->getArmCurPosition());
            LOG4CPLUS_INFO(logger, " Completed, preset number: " << presetNumber << ", Arm Pos: " << armPosition << ", Arm Actural Pos: " << pRobot->m_pMotor->getArmCurPosition());
            setExeRet(ST_SUCCESS);
            break;
        }
		default:
		{
            setExeRet(ST_FAIL);
			break;
		}
	}
    return _exeRet;
}
