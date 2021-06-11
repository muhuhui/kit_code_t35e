/*================================================================
*   Copyright (C) 2020 Tetraelc Ltd. All rights reserved.
*   
*   File name: ComParse.cpp
*   Author: JiangJin
*   Create Date: 2020-06-12
*   Description: 
*
================================================================*/


#include "Transaction/ComParse.h"
#include <time.h>

using namespace COMPkg;

COMParse *COMParse::getInstance()
{
    return SingleTon<COMParse>::GetInstance();
}

std::string COMParse::getReqName(std::string req)
{
    std::string reqName = "";
    Json::Reader reader;
    Json::Value val;
    if (reader.parse(req, val, false)) {
        if (!val["reqName"].isNull()) {
            reqName = val["reqName"].asString();
        }
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }
    return reqName;
}

int COMParse::getCommandType(std::string req)
{
    int nType = NULL_TYPE;
    Json::Reader reader;
    Json::Value val;
    if (reader.parse(req, val, false)) {
        if (!val["type"].isNull()) {
            nType = val["type"].asInt();
        }
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }
    return nType;
}

std::string COMParse::getCommandTransId(std::string req)
{
    std::string sTransId;
    Json::Reader reader;
    Json::Value val;
    if (reader.parse(req, val, false)) {
        if (!val["sTransId"].isNull()) {
            sTransId = val["sTransId"].asString();
        }
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }

    return sTransId;
}

int COMParse::parseControlCommand(std::string req, COMPkg::Control::Request& ctrl)
{
    int nRet = -1;
    Json::Reader reader;
    Json::Value val;
    Json::Value ctrlVal;
    if (reader.parse(req, val, false)) {
        if (!val["control"].isNull()) {
            ctrlVal = val["control"];
            if (!ctrlVal["nCommand"].isNull()) {
                nRet = 0;
                ctrl.nCommand = ctrlVal["nCommand"].asInt();
            }
            if (!ctrlVal["nValue"].isNull()) {
                ctrl.nValue = ctrlVal["nValue"].asInt();
            }
            if (!ctrlVal["fValue"].isNull()) {
                ctrl.fValue = ctrlVal["fValue"].asFloat();
            }
            if(!ctrlVal["posName"].isNull())
            {
				ctrl.posName = ctrlVal["posName"].asString();
            }
        }
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }
    return nRet;
}

int COMParse::parseSyncDownCommand(std::string req, COMPkg::SyncDown::Request& sync)
{
    int nRet = -1;
    Json::Reader reader;
    Json::Value val;
    Json::Value syncVal;
    if (reader.parse(req, val, false)) {
        if (!val["syncDown"].isNull()) {
            syncVal = val["syncDown"];
            if (!syncVal["nTaskId"].isNull() && 
                !syncVal["nTaskType"].isNull()) {
                nRet = 0;
                sync.nTaskId = syncVal["nTaskId"].asInt();
                sync.nTaskType = syncVal["nTaskType"].asInt();
            }
        }
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }

    return nRet;
}

int COMParse::parseSettingCommand(std::string req, COMPkg::Setting::Request& setting)
{
    int nRet = -1;
    Json::Reader reader;
    Json::Value val;
    Json::Value settingArray;
    if (reader.parse(req, val, false)) {
        if (!val["setting"].isNull()) {
            settingArray = val["setting"];
            printf("setting size: %d\n", settingArray.size());
            for (unsigned int i = 0; i < settingArray.size(); i++) {
                printf("setting[%d]: %d %s %d\n", i, 
                        settingArray[i]["nKey"].asInt(), 
                        settingArray[i]["strValue"].asString().c_str(), 
                        settingArray[i]["mode"]["value"].asInt());
                General gen;
                gen.strValue = settingArray[i]["strValue"].asString();
                gen.nKey = settingArray[i]["nKey"].asInt();
                if (!settingArray[i]["mode"].isNull()) {
                    gen.mode.value = settingArray[i]["mode"]["value"].asInt();
                    gen.mode.bindMap = settingArray[i]["mode"]["bindMap"].asInt();
                    gen.mode.isCancel = settingArray[i]["mode"]["isCancel"].asInt();
                    gen.mode.mapName = settingArray[i]["mode"]["mapName"].asString();
                }
                setting.generalList.push_back(gen);
            }
            nRet = 0;
        }
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }

    return nRet;
}

int COMParse::spliceCommandRes(std::string sTransId, int nErrcode, std::string sFeedback, std::string &res)
{
    Json::Value val;

    val["sTransId"] = sTransId;
    val["nErrcode"] = nErrcode;
    val["sFeedback"] = sFeedback;
    res = val.toStyledString();
    res.erase(std::remove_if(res.begin(), res.end(), 
                [](char c) -> bool { return (c == '\n' || c == '\t' || c == '\r' || c == ' '); }), res.end());

    return 0;
}

int COMParse::spliceCurStatus(COMPkg::CurStatus curStatus, std::string& sCurStatus)
{
    Json::Value val;
    Json::Value curStatusVal;
    Json::Value batVal;
    Json::Value motorVal;
    Json::Value radarVal;
    Json::Value PddVal;

    //局放
    PddVal["fTev"] = curStatus.pdd.tev;
    PddVal["fUw"] = curStatus.pdd.uw;
    curStatusVal["pdd"] = PddVal;

    //外部
    curStatusVal["nRobotId"] = curStatus.nRobotId;
    curStatusVal["fMaxTemp"] = curStatus.fMaxTemp;
    curStatusVal["runMode"] = curStatus.nMode;
    
    //电机状态
    motorVal["nStatus"] = curStatus.motor.nStatus;
    motorVal["nAvoidSta"] = curStatus.motor.nAvoidingStatus;
    motorVal["fSpeed"] = curStatus.motor.fSpeed;
    motorVal["fArmSpeed"] = curStatus.motor.fArmSpeed;
    motorVal["fYSpeed"] = curStatus.motor.fYSpeed;
    motorVal["fAngleSp"] = curStatus.motor.fAngleSpeed;
    motorVal["fCurPos"] = curStatus.motor.fCurrentPosition;
    motorVal["fYCurPos"] = curStatus.motor.fYCurrentPosition;
    motorVal["fAngleCurPos"] = curStatus.motor.fAngleCurrentPosition;
    motorVal["fArmPos"] = curStatus.motor.fArmPosition;
    motorVal["fMoveTemp"] = curStatus.motor.fMoveTemp;
    motorVal["fArmTemp"] = curStatus.motor.fArmTemp;
    motorVal["fMoveCur"] = curStatus.motor.fMoveCurrent;
    motorVal["fLiftCur"] = curStatus.motor.fLiftCurrent;
    motorVal["nPddSta"] = curStatus.motor.nPddStatus;
    motorVal["nPddDis"] = curStatus.motor.nPddDis;
	motorVal["nDisplayRFID"] = curStatus.motor.checkRFIDNUM;
    curStatusVal["motor"] = motorVal;

    //电池
    batVal["nChargeStatus"] = curStatus.battery.nChargeStatus; 
    batVal["fVoltage"] = curStatus.battery.fVoltage; 
    batVal["fCurrent"] = curStatus.battery.fCurrent; 
    batVal["fQuantity"] = curStatus.battery.fQuantity; 
    batVal["fTemperature"] = curStatus.battery.fTemperature; 
    curStatusVal["battery"] = batVal;

    //超声波
    radarVal["fForwardDistance"] = curStatus.radar.fForwardDistance;
    radarVal["fBackDistance"] = curStatus.radar.fBackDistance;
    radarVal["fDownDistance"] = curStatus.radar.fDownDistance;
    curStatusVal["radar"] = radarVal;

    time_t tm;
    time(&tm);
    curStatusVal["nTransId"] = (unsigned int)tm;
    val["curStatus"] = curStatusVal;
    sCurStatus = val.toStyledString();
    sCurStatus.erase(std::remove_if(sCurStatus.begin(), sCurStatus.end(), 
                [](char c) -> bool { return (c == '\n' || c == '\t' || c == '\r' || c == ' '); }), sCurStatus.end());
    return 0;
}

//告警PLC
int COMParse::splicePlcStatus(int Typeid, int vlaue, int machineid, std :: string & scallPlc)
{
   Json::Value val;
   Json::Value syncVal;
   syncVal["robotId"]=machineid;
   syncVal["kindType"]=Typeid;
   syncVal["nValue"]=vlaue;
   val["HEADN"]=syncVal;
   scallPlc=val.toStyledString();
   scallPlc.erase(std::remove_if(scallPlc.begin(), scallPlc.end(), 
					[](char c) -> bool { return (c == '\n' || c == '\t' || c == '\r' || c == ' '); }), scallPlc.end());
   return 0; 
}

int COMParse::spliceTaskStatus(COMPkg::TaskStatus taskStatus, std::string& sTaskStatus)
{
    Json::Value val;
    Json::Value taskStatusVal;

    //外部
    taskStatusVal["nRobotId"] = taskStatus.nRobotId;

    //任务
    taskStatusVal["nTaskId"] = taskStatus.nTaskId;
    taskStatusVal["nViewPointId"] = taskStatus.nViewPointId;
    taskStatusVal["nExecId"] = taskStatus.nExecId;
    taskStatusVal["nTaskType"] = taskStatus.nTaskType;

    time_t tm;
    time(&tm);
    taskStatusVal["nTransId"] = (unsigned int)tm;
    val["taskStatus"] = taskStatusVal;
    sTaskStatus = val.toStyledString();
    sTaskStatus.erase(std::remove_if(sTaskStatus.begin(), sTaskStatus.end(), 
                [](char c) -> bool { return (c == '\n' || c == '\t' || c == '\r' || c == ' '); }), sTaskStatus.end());
    return 0;
}

int COMParse::spliceAirStatus(COMPkg::AirStatus airStatus, std::string& sAirStatus)
{
    Json::Value val;
    Json::Value airStatusVal;
    Json::Value envVal;
    Json::Value plltVal;
    Json::Value gasVal;

    //外部
    airStatusVal["nRobotId"] = airStatus.nRobotId;

    //气体
    gasVal["fCH4"] = airStatus.gas.fCH4;
    gasVal["fO2"] = airStatus.gas.fO2;
    gasVal["fCO"] = airStatus.gas.fCO;
    gasVal["fH2S"] = airStatus.gas.fSO2;
    gasVal["fSmog"] = airStatus.gas.fSmog;
    airStatusVal["gas"] = gasVal;

    //环境
	envVal["fHumidity"] = airStatus.env.fHumidity;
    envVal["fTemperature"] = airStatus.env.fTemperature;
    envVal["fIlluminance"] = airStatus.env.fIlluminance;
    airStatusVal["env"] = envVal;
    
    //气体颗粒
	plltVal["fPm1_0"] = airStatus.pllt.fPm1_0;
    plltVal["fPm2_5"] = airStatus.pllt.fPm2_5;
    plltVal["fPm10"] = airStatus.pllt.fPm10;
    plltVal["fSF6"] = airStatus.pllt.fSF6;
    airStatusVal["pllt"] = plltVal;

    time_t tm;
    time(&tm);
    airStatusVal["nTransId"] = (unsigned int)tm;
    val["airStatus"] = airStatusVal;
    sAirStatus = val.toStyledString();
    sAirStatus.erase(std::remove_if(sAirStatus.begin(), sAirStatus.end(), 
                [](char c) -> bool { return (c == '\n' || c == '\t' || c == '\r' || c == ' '); }), sAirStatus.end());
    return 0;
}
