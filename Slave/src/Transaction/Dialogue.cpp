/* #include "Transaction/Dialogue.h"
#include <std_msgs/Int32.h>


Dialogue::~Dialogue()
{

}

Dialogue *Dialogue::getInstance()
{
    return SingleTon<Dialogue>::GetInstance();
}

int Dialogue::init()
{
    m_pROSDriver = ROSDriver::getInstance();

    return 0;
}

void Dialogue::registerCallBack(ResCommand pResCommandCallback)
{
    m_pResCommandCallback = pResCommandCallback;
}

int Dialogue::initRos(std::string sMasterUrl, std::string sHostUrl, int nRobotID)
{
    int nRet = 0;

    nRet = m_pROSDriver->initRos(sMasterUrl, sHostUrl, nRobotID);
    if(nRet != 0) {
        printf("--- Dialogue::initRos -1 \n");
        return -1;
    }

    printf("--- Dialogue::%s(), start.\n", __FUNCTION__);

    uint32_t queue_size = 10;
    char sCurStatusTopic[64] = {0};
    //nRobotID=37;
    sprintf(sCurStatusTopic, "OrbitalRobot/CurStatus/%d", nRobotID);

    nRet = m_pROSDriver->initPublisher<common_pkg::CurStatus>(PUB_CURSTATUS, sCurStatusTopic, queue_size);
    if(nRet != 0) {
        printf("--- Dialogue::initRos -2 \n");
        return -2;
    }
	//2020-9-9 topic拆包topic
   sprintf(sCurStatusTopic,"OrbitalRobot/GasCurStatus/%d",nRobotID);
   nRet=m_pROSDriver->initPublisher<common_pkg::CurStatus>(PUB_CURSTATUSN, sCurStatusTopic, queue_size);
	if(nRet != 0) {
        printf("--- Dialogue::initRos -2 \n");
        return -2;
    }
    //2020-10-22
    sprintf(sCurStatusTopic,"OrbitalRobot/InspectCurStatus/%d",nRobotID);
    nRet=m_pROSDriver->initPublisher<common_pkg::CurStatus>(PUB_INSPECTSTATUS, sCurStatusTopic, queue_size);
	if(nRet != 0) {
        printf("--- Dialogue::initRos -2 \n");
        return -2;
    }
    sprintf(sCurStatusTopic,"OrbitalRobot/OtherCurStatus/%d",nRobotID);
    nRet=m_pROSDriver->initPublisher<common_pkg::CurStatus>(PUB_OTHERCURSTATUS, sCurStatusTopic, queue_size);
	if(nRet != 0) {
        printf("--- Dialogue::initRos -2 \n");
        return -2;
    }
	//2020-9-22 告警PLC
	//int nRobotIDNP=90;
	//sprintf(sCurStatusTopic,"OrbitalRobot/CurStatus/%d",nRobotIDNP);
	//PLC编号值
	int PLCNUM=1;
	sprintf(sCurStatusTopic,"OrbitalRobot/PLC/%d",PLCNUM);
    nRet=m_pROSDriver->initPublisher<common_pkg::CurStatus>(PUB_CURSTATUSNP, sCurStatusTopic, queue_size);
	if(nRet != 0) {
        printf("--- Dialogue::initRos -2 \n");
        return -2;
    }
	
    char sCommandTopic[64] = {0};
    sprintf(sCommandTopic, "OrbitalRobot/Command/%d", nRobotID);
    nRet = m_pROSDriver->registerCallBack(sCommandTopic, m_pResCommandCallback);
    if(nRet != 0) {
        printf("--- Dialogue::initRos -3 \n");
        return -3;
    }

    printf("--- Dialogue::%s(), end.\n", __FUNCTION__);
    return 0;
}

bool Dialogue::isRosAlive()
{
    return m_pROSDriver->isRosAlive();
}

void Dialogue::heartBeat()
{
    m_pROSDriver->heartBeat();
}

void Dialogue::spin()
{
    m_pROSDriver->spin();
}

void Dialogue::publishCurStatus(common_pkg::CurStatus msg)
{  
    m_pROSDriver->publishMsg(PUB_CURSTATUS, msg);
}
void Dialogue::publishCurStatusN(common_pkg :: CurStatus msg)
{
    m_pROSDriver->publishMsg(PUB_CURSTATUSN, msg);
}
void Dialogue::publishCurStatusNP(common_pkg :: CurStatus msg)
{
   m_pROSDriver->publishMsg(PUB_CURSTATUSNP, msg);
}
void Dialogue::publishInspectStatus(common_pkg:: CurStatus msg)
{
   m_pROSDriver->publishMsg(PUB_INSPECTSTATUS, msg);
}
void Dialogue::publishOtherStatus(common_pkg:: CurStatus msg)
{
    m_pROSDriver->publishMsg(PUB_OTHERCURSTATUS,msg);
}
 */