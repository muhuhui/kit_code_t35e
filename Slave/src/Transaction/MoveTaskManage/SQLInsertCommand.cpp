//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : T3C_SLAVE
//  @ File Name : SQLInsertCommand.cpp
//  @ Date : 2019-07-02
//  @ Author : ShunWang
//
//


#include "SQLInsertCommand.h"
#include "Transaction/Robot.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "Driver/MyLog4CXX.h"

const std::string LOG_TAG("root.SQLInsertCommand::");
extern ViewPointInfo patrolViewPointInfo;

int SQLInsertCommand::exec()
{
    auto logger = initLogger(LOG_TAG + std::string(__FUNCTION__));
	Robot *pRobot = Robot::getInstance();
	int pointNum = _paraJson["pointNum"].asInt();
	bool is_pic = _paraJson["is_pic"].asBool();
	bool is_pdd = _paraJson["is_pdd"].asBool();
	bool is_vid = _paraJson["is_vid"].asBool();
	int taskId = _paraJson["taskId"].asInt();
	int pointId = _paraJson["pointId"].asInt();

	patrolViewPointInfo.nTaskId = taskId;
	patrolViewPointInfo.nViewPointId = pointId;

	Json::Value *sharedJson = getSharedParaJson();
	int execId = (*sharedJson)[JSON_EXEC_ID].asInt();
	patrolViewPointInfo.nExecId = execId;

	if(!is_pic)
	{
		patrolViewPointInfo.strCameraPhotoPath = "";
		patrolViewPointInfo.strThermelPhotoPath = "";
	}
	if(!is_pdd)
	{
		patrolViewPointInfo.TEV = 0;
		patrolViewPointInfo.UW = 0;
	}
	if(!is_vid)
	{
		patrolViewPointInfo.strCameraVideoPath = "";
		patrolViewPointInfo.strThermelVideoPath = "";
	}

	if(_exeRet != ST_SUCCESS && _exeRet != ST_FAIL)
	{
	    switch(_exeStatus)
		{
			case INSERT_ST_BEGAIN:
			{
                int nRet = pRobot->m_pMySQLServer->recordViewPointInfo(patrolViewPointInfo);
				//结果更新
				if(pointNum & 1<<POINT_LAST) {        //本次任务最后一个巡检点
					//写本次任务执行结果到数据库
					nRet = pRobot->m_pMySQLServer->changeExecResult(patrolViewPointInfo);	
				}
				if(nRet == RET_SUCCESS) {
					_tryCnt = 0;
	                _exeStatus = INSERT_ST_FINSIH;
				}
				else {
	                if(_tryCnt <= 3) {
                        printf("SQLInsertCommand failed, try again!\n");
	            		LOG4CPLUS_INFO(logger, "failed");
                        _tryCnt++;
					}
					else {
                        printf("SQLInsertCommand failed, exit!\n");
						LOG4CPLUS_ERROR(logger, "failed exit!");
						setExeRet(ST_FAIL); 
					}
				}
			}
			case INSERT_ST_FINSIH:
			{
                setExeRet(ST_SUCCESS);
                break;
			}
			default:
			{
				break;
			}
		}
	}
	return _exeRet;
}