/*================================================================
*   Copyright (C) 2020 Tetraelc Ltd. All rights reserved.
*   
*   File name: COMParse.h
*   Author: JiangJin
*   Create Date: 2020-06-12
*   Description: 
*
================================================================*/


#pragma once

#include "base/SingleTon.h"
#include "ThirdParty/jsoncpp/json.h"
#include <string>
#include <vector>
#include <algorithm>

namespace COMPkg {

/*****************topic********************/
struct Battery {
    int nChargeStatus;
    float fVoltage;
    float fCurrent;
    float fQuantity;
    float fTemperature;
};

struct Gas {
    float fCH4;
    float fO2;
    float fCO;
    float fSO2;
    float fSmog;
    float fCO2;
};

struct Motor {
    int nStatus;
    int nAvoidingStatus;
    float fSpeed;
    float fArmSpeed;
    float fYSpeed;
    float fAngleSpeed;
    float fCurrentPosition;
    float fYCurrentPosition;
    float fAngleCurrentPosition;
    float fArmPosition;
    float fMoveCurrent;
    float fMoveTemp;
    float fArmTemp;
    float fLiftCurrent;
    int   nPddStatus;
    int nPddDis;
	//获得RFIDNUM实时显示
	std::string checkRFIDNUM;
};

struct Radar {
    float fForwardDistance;
    float fBackDistance;
    float fDownDistance;
};

struct Env {
    float fHumidity;
    float fTemperature;
    float fIlluminance;
};

struct Pllt {
    float fPm1_0;
    float fPm2_5;
    float fPm10;
    float fSF6;
};

struct Pdd {
    float tev;
    float uw;
};

struct CurStatus {
    int nRobotId;
    std::string sSoftVersion;
    int nMode;
    float fMaxTemp;
    Motor motor;
    Battery battery;
    Radar radar;
    Pdd pdd;
};

struct TaskStatus {
    int nRobotId;
    int nTaskId;
    int nTaskType;
    int nViewPointId;
    int nExecId;
};

struct AirStatus {
    int nRobotId;
    Gas gas;
    Pllt pllt;
    Env env;
};

struct callPLC{
    bool gasFlag;
    bool tempFlag;
    bool breakdownFlag;
    bool divulgeFlag;
    bool stoneFlag;
};


/*****************service********************/
struct Mode {
    int value;
    int bindMap;
    int isCancel;
    std::string mapName;
};

struct General {
    int nKey;
    std::string strValue;
    Mode mode;
};

struct ViewPoint {
    int nTaskId;
    int nPointId;
    int nPresetNumber;
    int nIndustrialCameraFocus;
    int nThermalImagerFocus;
    float fPosition;
};

struct Task {
    int nRobotId;
    int nTaskId;
    int nStatus;
    float fSpeed;
    long long lStartTime;
    long long lEndTime;
    ViewPoint viewPoint;
};

struct Setting {
    struct Request {
        std::string sTransId;
        std::vector<General> generalList;
    };
    struct Response {
        std::string sTransId;
        int nErrcode;
        std::string sFeedback;
    };
};

struct Control {
    struct Request {
        std::string sTransId;
        int nCommand;
        int nValue;
        float fValue;
        std::string  posName;
    };
    struct Response {
        std::string sTransId;
        int nErrcode;
        std::string sFeedback;
    };
};

struct SyncDown {
    struct Request {
        std::string sTransId;
        int nTaskId;
        int nTaskType;
        std::vector<Task> tasks;
    };

    struct Response {
        std::string sTransId;
        int nErrcode;
    };
};

}

enum COM_COMMAND_TYPE {
    NULL_TYPE = -1,
    CURSTATUS_TYPE = 0,
    CONTROL_TYPE = 1,
    SETTING_TYPE = 2,
    SYNCDOWN_TYPE = 3,
    PTZ_CONTROL_TYPE = 4
};

class COMParse
{
public:
    static COMParse *getInstance();

    std::string getReqName(std::string req);
    int getCommandType(std::string req);
    std::string getCommandTransId(std::string req);

    int parseControlCommand(std::string req, COMPkg::Control::Request& ctrl);
    int parseSyncDownCommand(std::string req, COMPkg::SyncDown::Request& sync);
    int parseSettingCommand(std::string req, COMPkg::Setting::Request& setting);

    int spliceCommandRes(std::string sTransId, int nErrcode, std::string sFeedback, std::string &res);

    int spliceCurStatus(COMPkg::CurStatus curStatus, std::string& sCurStatus);
    int spliceTaskStatus(COMPkg::TaskStatus taskStatus, std::string& sTaskStatus);
    int spliceAirStatus(COMPkg::AirStatus airStatus, std::string& sAirStatus);

    int splice(COMPkg::CurStatus curStatus, std::string& sCurStatus);
    
    //告警PLC
    int splicePlcStatus(int Typeid,int vlaue,int machineid,std::string &scallPlc);
};


