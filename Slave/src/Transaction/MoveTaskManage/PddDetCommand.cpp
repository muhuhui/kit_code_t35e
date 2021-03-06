//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : T3C_SLAVE
//  @ File Name : GotoCommand.cpp
//  @ Date : 2019-06-27
//  @ Author : ShunWang
//
//


#include "PddDetCommand.h"
#include "Driver/PTZDriver.h"
#include "Transaction/Robot.h"
#include "Driver/MyLog4CXX.h"
#include <vector>

const std::string LOG_TAG("root.PddDetCommand::");
extern ViewPointInfo patrolViewPointInfo;

int PddDetCommand::exec()
{
    auto logger = initLogger(LOG_TAG + std::string(__FUNCTION__));
	//解析JSON参数
    int pos = _paraJson["pddDis"].asInt();
    int isReturn = _paraJson["isReturn"].asInt();
    int taskId = _paraJson["taskId"].asInt();
    int pointId = _paraJson["pointId"].asInt();
    Robot *pRobot = Robot::getInstance();
    PTZDriver::Ptr pPtzDriver = PTZDriver::getInstance();

    Json::Value *sharedJson = getSharedParaJson();
    if(sharedJson == NULL) {
        printf("PddDetCommand::%s(), can't get json value!\n", __FUNCTION__);
        LOG4CPLUS_ERROR(logger, " can't get json value! ");
        setExeRet(ST_FAIL);
        return _exeRet;
    }
    int execId = (*sharedJson)[JSON_EXEC_ID].asInt();

	if(_exeRet != ST_SUCCESS && _exeRet != ST_FAIL)
	{
	    switch(_exeStatus)
		{
			case PDDGOTO_ST_SNDCMD:
			{
                int nRet = 0;

                if(isReturn) {
                    nRet = pPtzDriver->ctrlPTZ(DISCHARGE_MOTOR, TURN_HOME, 0);
                    if(nRet == 0) {
                        _tryCnt = 0;
                        clearTimer();
                        _exeStatus = PDDGOTO_ST_BACKRESULT;
                    }
                }
                else {
                    nRet = pPtzDriver->ctrlPTZ(DISCHARGE_MOTOR, TURN_TO, pos);
                    printf("PddDetCommand::%s(), go to %d, nRet: %d\n", __FUNCTION__, pos, nRet);
                    LOG4CPLUS_INFO(logger, " go to " << pos << ", nRet: " << nRet);
                    if(nRet == 0) {
                        _tryCnt = 0;
                        clearTimer();
                        _exeStatus = PDDGOTO_ST_WAITRESULT;
                    }
                }

				if(nRet != 0) {
	                if(_tryCnt <= 3) {
                        printf("PddDetCommand::%s(), go to %d failed, try again!\n", __FUNCTION__, pos);
	            		LOG4CPLUS_INFO(logger, " go to " << pos << " failed, try again!");
                        _tryCnt++;
					}
					else {
						setExeRet(ST_FAIL); 
                        printf("PddDetCommand::%s(), go to %d failed, exit!\n", __FUNCTION__, pos);
						LOG4CPLUS_ERROR(logger, " go to " << pos << " failed, exit!");
					}
				}
				break;
			}

			case PDDGOTO_ST_WAITRESULT:
            {
                if(0 != sleeepWaitTimeOut(10)) { break; }
                std::vector<PTZMotorStat> ptzState;
                pPtzDriver->getPTZStatus(ptzState);
                if(ptzState.size() < 4) {
                    setExeRet(ST_FAIL); 
                    printf("PddDetCommand::%s(), __LINE__ = %d: ptzState size insufficient, return!\n", __FUNCTION__, __LINE__);
                    LOG4CPLUS_ERROR(logger, " ptzState size insufficient, return! ");
                }

                if(ptzState[4].moveStat == STOPPED && abs(ptzState[4].angle-pos) < 3)
                {
                    //插入任务运行日志记录
                    patrolViewPointInfo.nTaskId = taskId;
                    patrolViewPointInfo.nExecId = execId;
                    patrolViewPointInfo.nViewPointId = pointId;

                    patrolViewPointInfo.fTemperature = pRobot->m_pSensorBoardEx->getEnvInfo().fTemperature;
                    patrolViewPointInfo.fHumidity = pRobot->m_pSensorBoardEx->getEnvInfo().fHumidity;
                    patrolViewPointInfo.fCombustible = pRobot->m_pSensorBoardEx->getGasInfo().fCH4;
                    patrolViewPointInfo.fCO = pRobot->m_pSensorBoardEx->getGasInfo().fCO;
                    patrolViewPointInfo.fH2S = pRobot->m_pSensorBoardEx->getGasInfo().fSO2;
                    patrolViewPointInfo.fO2 = pRobot->m_pSensorBoardEx->getGasInfo().fO2;

                    patrolViewPointInfo.nposition = Motor::getInstance()->getCurPosition();
                    _exeStatus = PDDGOTO_ST_BACK;
                }
                break;
            }

            case PDDGOTO_ST_BACK:
            {
                int nRet = pPtzDriver->ctrlPTZ(DISCHARGE_MOTOR, TURN_TO, 0);
                LOG4CPLUS_INFO(logger, " pddDet go to home, nRet: " << nRet);
                _exeStatus = PDDGOTO_ST_BACKRESULT;
                break;
            }

            case PDDGOTO_ST_BACKRESULT:
            {
                if(0 != sleeepWaitTimeOut(10)) { break; }
                std::vector<PTZMotorStat> ptzState;
                pPtzDriver->getPTZStatus(ptzState);
                if(ptzState.size() < 4) {
                    setExeRet(ST_FAIL); 
                    printf("PddDetCommand::%s(), __LINE__ = %d: ptzState size insufficient, return!\n", __FUNCTION__, __LINE__);
                    LOG4CPLUS_ERROR(logger, " ptzState size insufficient, return! ");
                }

                if(ptzState[3].moveStat == STOPPED && abs(ptzState[3].angle-0) < 3)
                {
                    _exeStatus = PDDGOTO_ST_FINSIH;
                }
                break;
            }

            case PDDGOTO_ST_FINSIH:
            {
                clearTimer();
                (*sharedJson)[JSON_EXEC_RESULT] = ST_SUCCESS;
                LOG4CPLUS_INFO(logger, "PddDetCommand succeed!");
                setExeRet(ST_SUCCESS);
                break;
            }

            default:
            {
                setExeRet(ST_FAIL);
                break;
            }
		}
	}
	return _exeRet;
}
