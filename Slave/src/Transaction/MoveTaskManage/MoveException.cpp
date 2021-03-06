//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : T3C_SLAVE
//  @ File Name : MoveException.cpp
//  @ Date : 2019-07-07
//  @ Author : Shunwang
//
//


#include "MoveException.h"
#include "Driver/InfraredDriver.h"
#include "Driver/FireDoorDriver.h"
#include "Transaction/RadarManager.h"
#include "Driver/MotorDriver.h"
#include "Transaction/WarningManager.h"
#include "Robot.h"
#include "Driver/MyLog4CXX.h"
#include "Driver/AudioDriver.h"

const std::string LOG_TAG("root.MoveException::");

std::string getTime()
{
  time_t timep;
  time(&timep);

  char tmp[128];
  strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));

  return tmp;
}

MoveException*  MoveException::getInstance()
{
    return SingleTon<MoveException>::GetInstance();
}

 void MoveException::init()
 {
 	memset(m_warmFlag,false,sizeof(m_warmFlag));
 	//注册观察者
 	InfraredDriver* infraredDriver = InfraredDriver::getInstance();	
    infraredDriver->attach(this);
 	RadarManager* radarManager = RadarManager::getInstance();
    radarManager->attach(this);
 	FireDoorDriver* fireDoorDriver = FireDoorDriver::getInstance();
    fireDoorDriver->attach(this);
 	MotorDriver* motorDriver = MotorDriver::getInstance();
    motorDriver->attach(this);
 	
 }

//获取总的运动告警标识
bool MoveException::getTotalMoveExcepFlag()
{
	bool flag = false;
	for(int i=0;i<DEV_TOTAL;i++)
	{
		if(m_warmFlag[i] >0)
		{
			printf("*******__FUNCTION__::%s(), __LINE__ = %d,warn=%x\n", __FUNCTION__, __LINE__,flag);
        }
        flag |= m_warmFlag[i];
	}
	return flag;
}

//设置前端红外异常标识
void MoveException::setForewardInfraredExcep(bool flag)
{
	m_warmFlag[DEV_FOREWARD_INFRARED] = flag;
}
//设置前端电机异常标识
void MoveException::setForewardMotorExcep(bool flag)
{
	m_warmFlag[DEV_FOREWARD_MOTOR] = flag;
}
//获取前端红外异常
bool MoveException::getForewardInfraredExcep()
{
	return m_warmFlag[DEV_FOREWARD_INFRARED]; 
}
//获取前端电机异常
bool MoveException::getForewardMotorExcep()
{
	return m_warmFlag[DEV_FOREWARD_MOTOR]; 
}
//设置后端超声波异常
void MoveException::setForewardUSSExcep(bool flag)
{
	m_warmFlag[DEV_FOREWARD_USS] = flag;
}
//获取前端超声波异常
bool MoveException::getForewardUSSExcep()
{
	return m_warmFlag[DEV_FOREWARD_USS];
}
//设置后端超声波异常
void MoveException::setBackUSSExcep(bool flag)
{
	m_warmFlag[DEV_BACK_USS] = flag;
}
//设置下端超声波异常
void MoveException::setDownUSSExcep(bool flag)
{
	m_warmFlag[DEV_DOWN_USS] = flag;
}
//设置后端电机异常
void MoveException::setBackMotorExcep(bool flag)
{
	m_warmFlag[DEV_BACK_MOTOR] = flag;
}
//获取后端超声波异常
bool MoveException::getBackUSSExcep()
{
	return m_warmFlag[DEV_BACK_USS];
}
//获取后端电机异常
bool MoveException::getBackMotorExcep()
{
	return m_warmFlag[DEV_BACK_MOTOR];
}
//设置后端红外异常
void MoveException::setBackInfraredExcep(bool flag)
{
	m_warmFlag[DEV_BACK_INFRARED] = flag;
}
//设置防火门异常
void MoveException::setFireDoorExcep(bool flag)
{
	m_warmFlag[DEV_FIREDOOR] = flag;
}
//设置后端红外异常
bool MoveException::getBackInfraredExcep()
{
	return m_warmFlag[DEV_BACK_INFRARED];
}
//获取防火门异常
bool MoveException::getFireDoorExcep()
{
	return m_warmFlag[DEV_FIREDOOR];
}

//观察者模式的更新接口
void  MoveException::update(int nSub)
{
	auto logger = initLogger(LOG_TAG + std::string(__FUNCTION__));
	WarningManager* warningManager = WarningManager::getInstance();
    WarningService* warningService = WarningService::getInstance();
	AlarmInfo vAlarmInfo;
	std::string sCurTime = getTime();
	vAlarmInfo.strAlarmTime = sCurTime;
	vAlarmInfo.dPosition = Motor::getInstance()->getCurPosition();
	vAlarmInfo.nRobotID = warningManager->m_nRobotID;
	vAlarmInfo.strValue = "0";

    printf("%s:enter %s, nSub: %d, position: %f\n", __FILE__, __FUNCTION__, nSub,  vAlarmInfo.dPosition);
	LOG4CPLUS_INFO(logger, " start. ");

	switch(nSub)
	{
		case SUB_INFRARED: //红外状态更新
		{
		InfraredDriver* infraredDriver = InfraredDriver::getInstance();			
		InfraredWarnType infraredWarnType=infraredDriver->getWarnType();
		switch(infraredWarnType)
		{
			case INFRARED_NONE_WARN:
			{
				setForewardInfraredExcep(false);
				setBackInfraredExcep(false);
				break;
			}
			case INFRARED_FRONT_WARN:
			{
				setForewardInfraredExcep(true);
    			vAlarmInfo.strItemName = ALARM_FOREWARD_INFRARED;
    			warningManager->addAlarm(vAlarmInfo);
				setBackInfraredExcep(false);			
				break;
			}
			case INFRARED_BACK_WARN:
			{
				setForewardInfraredExcep(false);
				setBackInfraredExcep(true);
				vAlarmInfo.strItemName = ALARM_BACK_INFRARED;
				warningManager->addAlarm(vAlarmInfo);				
				break;
			}
			case INFRARED_BOTH_WARN:
			{
				setForewardInfraredExcep(true);
				setBackInfraredExcep(true);	
				vAlarmInfo.strItemName = ALARM_FOREWARD_INFRARED;
				warningManager->addAlarm(vAlarmInfo);
				vAlarmInfo.strItemName = ALARM_BACK_INFRARED;
				warningManager->addAlarm(vAlarmInfo);   						
				break;
			}
			default:
			{
				break;
			}
		}
		
			break;
		}
		case SUB_ULTRASOUND: //超声波状态更新
		{
			printf("updating radar status-------\n");
			LOG4CPLUS_INFO(logger, " updating radar status. ");

			RadarManager* radarManager = RadarManager::getInstance();
			int radarWarnStatus = radarManager->getWarnStatus();
            printf("radar warning type:%d\n", radarWarnStatus);
            LOG4CPLUS_INFO(logger, "radar warning type: " << radarWarnStatus);
            AudioDriver* pAudioDriver = AudioDriver::getInstance();
            PowerDriver* pPowerDriver = PowerDriver::getInstance();
            if (!radarManager->getWarnStatus()) {
                setForewardUSSExcep(false);
                setBackUSSExcep(false);
                pAudioDriver->setWarn(false);
                pPowerDriver->setLedWarn(false);
                break;
            }
            if (radarManager->getWarnStatus(RADAR_FRONT)) {
                setForewardUSSExcep(true);
                vAlarmInfo.strItemName = ALARM_USS;
                vAlarmInfo.strReason = ALARM_FOREWARD_USS;
                // warningManager->addAlarm(vAlarmInfo);					
                WarningService::getInstance()->insertAlarm(vAlarmInfo);
                pAudioDriver->setWarn(true);
                pPowerDriver->setLedWarn(true);
            }
            if (radarManager->getWarnStatus(RADAR_BACK)) {
                setBackUSSExcep(true);
                vAlarmInfo.strItemName = ALARM_USS;
                vAlarmInfo.strReason = ALARM_BACK_USS;
                // warningManager->addAlarm(vAlarmInfo);						
                WarningService::getInstance()->insertAlarm(vAlarmInfo);
                pAudioDriver->setWarn(true);
                pPowerDriver->setLedWarn(true);
            }
			if (radarManager->getWarnStatus(RADAR_DOWN)) {
                setDownUSSExcep(true);
                vAlarmInfo.strItemName = ALARM_USS;
                vAlarmInfo.strReason = ALARM_DOWN_USS;
                // warningManager->addAlarm(vAlarmInfo);						
                WarningService::getInstance()->insertAlarm(vAlarmInfo);
                pAudioDriver->setWarn(true);
                pPowerDriver->setLedWarn(true);
            }
			break;
		}
		case SUB_FIREDOOR: //防火门连接状态更新
		{
			// FireDoorDriver* fireDoorDriver = FireDoorDriver::getInstance();
            // // std::map<int, FireDoorStatus>* fireDoorsStatusMap =fireDoorDriver->getStatus();
			// bool flag = false;
			// //轮训所有防火门状态
            // std::map<int, FireDoorStatus >::iterator iter = fireDoorsStatusMap->begin();
            // while (iter != fireDoorsStatusMap->end())
			// {
            //     if(FD_ERROR == iter->second)
			// 	{
			// 		vAlarmInfo.strReason = "防火门Id="+std::to_string(iter->first);
			// 		vAlarmInfo.strItemName = ALARM_FIREDOOR;
    		// 		warningManager->addAlarm(vAlarmInfo);				
			// 		flag = true;
			// 	}
			// }
			// setFireDoorExcep(flag);
			setFireDoorExcep(false);
			break;
		}
		case SUB_MOTOR_WARN: //电机状态更新
		{
			Motor* pMotor = Motor::getInstance();
            int errnum = pMotor->getMotorWarn();
            if(errnum != 0)
			{
				/* setForewardMotorExcep(true);
				setBackMotorExcep(true); */
				vAlarmInfo.strItemName = ALARM_MOTOR;
                // vAlarmInfo.strReason = "行走电机"+motorWarn->strErrMsg;
                vAlarmInfo.strReason = "行走电机"+std::to_string(errnum);
                // warningManager->addAlarm(vAlarmInfo);
                warningService->insertAlarm(vAlarmInfo);
			}
			else
			{
				setForewardMotorExcep(false);
				setBackMotorExcep(false);			
			}
			break;
		}
		case SUB_LIFT_MOTOR_WARN: //升降电机状态更新
		{
			Motor* pMotor = Motor::getInstance();
            int errnum = pMotor->getArmMotorWarn();
            if(errnum == HOMING_ERROR)
			{
				vAlarmInfo.strItemName = ALARM_HOMING;
                // vAlarmInfo.strReason = motorWarn->strErrMsg;
                vAlarmInfo.strReason = "升降电机"+std::to_string(errnum);
                // warningManager->addAlarm(vAlarmInfo);
                warningService->insertAlarm(vAlarmInfo);
			}
			else if (errnum != 0)
			{
				vAlarmInfo.strItemName = ALARM_MOTOR;
                // vAlarmInfo.strReason = "升降电机"+motorWarn->strErrMsg;
                vAlarmInfo.strReason = "升降电机"+std::to_string(errnum);
                // warningManager->addAlarm(vAlarmInfo);
                warningService->insertAlarm(vAlarmInfo);	
			}
			else
			{
			    setDownUSSExcep(false);
			}
			break;
		}
		default:
		{
			break;
		}
	}
}


