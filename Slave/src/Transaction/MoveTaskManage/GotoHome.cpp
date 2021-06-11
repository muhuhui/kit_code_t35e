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


#include "GotoHome.h"
#include "Transaction/Robot.h"
#include "Driver/MyLog4CXX.h"

const std::string LOG_TAG("root.GotoHome::");

int GotoHome::exec()
{
    auto logger = initLogger(LOG_TAG + std::string(__FUNCTION__));
	//解析JSON参数
    Robot *pRobot = Robot::getInstance();

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

	if(_exeRet != ST_SUCCESS && _exeRet != ST_FAIL)
	{
	    switch(_exeStatus)
		{
			case GOTOHOME_ST_SNDCMD:
			{
                int nRet = pRobot->m_pMotor->manualGotoHome();
                printf("GotoHome::%s(), nRet: %d\n", __FUNCTION__, nRet);
	            LOG4CPLUS_INFO(logger, " go to home" << ", nRet: " << nRet);
				if(nRet == RET_SUCCESS) {
					_tryCnt = 0;
                    clearTimer();
	                _exeStatus = GOTOHOME_ST_WAITRESULT;
				}
				else {
	                if(_tryCnt <= 5) {
                        printf("GotoHome::%s(), go to home failed, try again!\n", __FUNCTION__);
	            		LOG4CPLUS_INFO(logger, " go to home failed, try again!");
                        _tryCnt++;
					}
					else {
						(*sharedJson)[JSON_EXEC_RESULT] = ST_FAIL;
                        printf("GotoHome::%s(), go to home failed, exit!\n", __FUNCTION__);
						LOG4CPLUS_ERROR(logger, " go to home failed, exit!");
						setExeRet(ST_FAIL); 
					}
				}
				break;
			}
			case GOTOHOME_ST_WAITRESULT:
			{
                if (0 == sleeepWaitTimeOut(20)) {
                    if ( pRobot->m_pMotor->isInState( LIFT_MOTOR, STOP_STATE) )
                    {
						(*sharedJson)[JSON_EXEC_RESULT] = ST_SUCCESS;
                        setExeRet(ST_SUCCESS);
                    }
                }
			}
            break;
			default:
			{
				break;
			}
			
		}
	}
	return _exeRet;
}