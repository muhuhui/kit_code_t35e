#include "Driver/UpGradeDriver.h"
#include "ThirdParty/jsoncpp/json.h"
#include <algorithm>
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

UpGradeDriver *UpGradeDriver::getInstance()
{
  return SingleTon<UpGradeDriver>::GetInstance();
}

int UpGradeDriver::init()
{
    TcpCltSession* cltSession = TcpCltSession::getInstance();
    if (cltSession->m_cltSession.find(TCP_MAIN_BOARD_UPGRADE) == cltSession->m_cltSession.end()||
        cltSession->m_cltSession.find(TCP_PTZ_BOARD_UPGRADE) == cltSession->m_cltSession.end()||
        cltSession->m_cltSession.find(TCP_ENV_BOARD_UPGRADE) == cltSession->m_cltSession.end())
    {
        m_isConnect = false;
        return -1;
    }
    else
    {
        m_isConnect = true;
        return 0;
    }
}

int UpGradeDriver::request(std::string sReqName, std::string sReq, std::string& sRes)
{
    int nRet = -1;
    if(!m_isConnect) {
        return nRet;
    }
    TcpCltSession* cltSession = TcpCltSession::getInstance();
    if(sReqName == "reqMotionUpdate" || sReqName == "motionUpdating" || sReqName == "quitMotionUpdate")
    {
        nRet = cltSession->m_cltSession[TCP_MAIN_BOARD_UPGRADE]->request(sReq, sRes);
    }
    else if(sReqName == "reqPTZUpdate" || sReqName == "PTZUpdating" || sReqName == "quitPTZUpdate")
    {
        nRet = cltSession->m_cltSession[TCP_PTZ_BOARD_UPGRADE]->request(sReq, sRes);
    }
    else if(sReqName == "reqSensorUpdate" || sReqName == "sesorUpdating" || sReqName == "quitSesorUpdate")
    {
        nRet = cltSession->m_cltSession[TCP_ENV_BOARD_UPGRADE]->request(sReq, sRes);
    }

    std::cout << "web send: " << sReq << std::endl;
    std::cout << "mcb reply: " << sRes << std::endl;

    return nRet;
}