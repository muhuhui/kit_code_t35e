#ifndef ROBOT_H
#define ROBOT_H

#include "Transaction/ConfigServer.h"
#include "Transaction/Capabilities.h"
#include "Transaction/Motor.h"
#include "Transaction/Task.h"
#include "Transaction/IndustrialCamera.h"
#include "Transaction/ThermalImager.h"
#include "Transaction/MySQLServer.h"
#include "Transaction/MoveTaskManage/MoveTaskManage.h"
#include "Transaction/MoveTaskManage/PositionMove.h"
#include "Transaction/WarningManager.h"
#include "Transaction/SettingService.h"
#include "Transaction/TaskService.h"
#include "Transaction/MapService.h"
#include "Transaction/ChargePileService.h"
#include "Driver/BatteryDriver.h"
#include "Driver/SensorBoardEx.h"
#include "Driver/PowerDriver.h"
#include "Driver/FireDoorDriver.h"
#include "Driver/ChargePileDriver.h"
#include "Transaction/RadarManager.h"
#include "Driver/HallDriver.h"
#include "Driver/MyLog4CXX.h"
#include "Transaction/ComParse.h"
#include "Driver/PDDMotorDriver.h"
#include "Driver/PartialDischargeDetectorDriver.h"
#include "Driver/MotorDriver.h"
#include "Driver/ZMQDriver.h"
#include "Driver/BusinessInformation.h"
#include "Driver/LowConsumeManage.h"
#include "Driver/PTZDriver.h"
#include "Driver/UpGradeDriver.h"
#include "Driver/VersionQuery.h"
#include "Transaction/ImageProcessManager.h"

enum GeneralMsgType 
{
    INT_TYPE = 0,
    FLOAT_TYPE,
    STRING_TYPE
};

#define NKEY 248

enum GeneralMsgKey 
{
    CAMERA_IP = 0,                //相机IP
    CAMERA_PORT = 1,              //相机端口 
    THERMAL_IP = 2,               //热像仪IP
    THERMAL_PORT = 3,             //热像仪端口
    DEFAULT_SPEED = 4,            //默认速度
    ORBIT_END_POS = 6,            //终点位置
    ORBIT_BEGIN_POS = 7,          //起点位置           
    CHARGE_SWITCH = 9,            //充电开关设置
    CLEAR_FRID = 11,              //清除RFID
    SET_RUN_MODE = 12,            //设置运行状态
    CHARGEPILE_POSITION = 14,     //添加充电桩
    CGARGEPILE_MIN_POWER = 15,    //充电最小电量
    MOTOR_CONTROL_LOCK = 16,      //运动控制锁定
    ORBIT_CUR_POS = 17,           //更改当前位置
    ORBIT_ACC = 18,               //运动加速度
    ORBIT_DEC = 19,               //运动减速度
    ROBOT_REBOOT = 20,            //机器重启
    CHANGE_MAP = 21,              //修改绑定地图
    PTZ_SWITCH = 22,              //云台转向开关
    ULSOUND_SWITCH = 23,          //雷达软开关
    SET_MOVE_WARN = 24,           //行走声光告警开关
    ULSOUND_SWITCH_BACK = 25,          //后方雷达软开关
    ULSOUND_SWITCH_DOWN = 26,          //下方雷达软开关
    PTZ_SLEEP_SWITCH = 27,        //云台休眠开关
    PTZ_MANUAL_SWITCH = 28,       //云台手动开关
    FACTORY_TEST = 29,            //工厂测试开关
    SOUND_WARN_TYPE = 30,         //声音告警类型
    VOLTAGE_FIT = 31,             //电压拟合
    ARM_CUR_POS = 32,             //升降臂速度
    DRIVER_MSG_SWITCH = 33,       //驱动板调试信息开关
    HOME_SWITCH = 34,       //巡检前归零

    BATTERY_HIGH_THRESHOLD = 99,  //电池高阈值，高于等于该值，不反复充电(服务版在用)
    BATTERY_MAX_QUANTITY = 100,   //充电停止的最大电量
    ULSOUND_THRESHOLD = 101,      //超声波有效测量距离
    POSPLAN_UPDATE = 102,         //更新位置触发任务
    ARM_BEGIN_POS = 103,          //升降臂起点位置
    ARM_END_POS = 104,            //升降臂终点位置
    ARM_LIFT_MOVING = 105,
    ULSOUND_BACK_THRESHOLD = 106,      //超声波后方有效测量距离
    ULSOUND_DOWN_THRESHOLD = 107,      //超声波下方有效测量距离
    LIFT_MANUAL_SWITCH=108,       //手动控制升降臂开关
};

enum ControlCommand {
    EMERGENCE_CMD = 0,          /*0:non-emergence, 1:emergence*/
    INSPECTION_PAUSE_CMD = 1,   /*0:non-pause, 1:pause*/
    MANUAL_MOTOR_CMD = 2,       /*0:pause, 1:inchForward, 2:ichBackward, 3:fastForward, 4:fastBackward*/
    RECHARGE_CMD = 3,           /*0:NULL, 1:recharge*/
    OEND_POINT_CMD = 4,         /*position of the track end point*/
    CLOSE_FIRE_DOOR = 5,              //closefiredoor/*0:close fire door, 1:open fire door, 2:get current status of the nearest fire door; nErrcode - -1:error, 0:closed, 1:opening, 2:middle*/
    CHARGEING_PILE = 6,         /*0:close charging pile, 1:open charging pile, 2:get current status of the charging pile; nErrcode - -1:error, 0:closed, 1:opening, 2:middle*/
    RUN_MODE = 7,               /*0:Inspection mode, 1: manual mode*/

    ECECUTE_TASK_NOW = 8,       /*nValue: TaskId*/
    OPEN_FIRE_DOOR = 9,            //openfiredoor  /*0:close fire door, 1:open fire door, 2:get current status of the nearest fire door; nErrcode - -1:error, 0:closed, 1:opening, 2:middle*/

    POWER_SWITCH = 10,
    CLOUD_PLATFORM_SWITCH = 11,
    MOTOR_SWITCH = 12,
    CLEAR_WARN = 13,
    RADAR_SWITCH_CMD = 14,
    INQUERY_POS_CMD = 15,
    READ_MOTOR_DATA_CMD = 16, 
    WRITE_MOTOR_DATA_CMD = 17,
	CHARGE_SWITCH_CMD=18,
    VERSION_QUERY_CMD = 19,
    HOME_CMD = 20,
    CHARGE_FLAG = 100,
};

enum PtzControlCommand {
    VISUAL_CMD = 1,            /*VISUAL nCommand*/
    VISUAL_STOP_CMD = 0,           /*VISUAL nValue*/
    VISUAL_UP_CMD = 1,
    VISUAL_DOWN_CMD = 2,

    HORIZON_CMD = 2,           /*HORIZON nCommand*/
    HORIZON_STOP_CMD = 0,           /*VISUAL nValue*/
    HORIZON_LEFT_CMD = 1,
    HORIZON_RIGHT_CMD = 2,

    INFRADE_CMD = 3,            /*INFRADE nCommand*/
    INFRADE_STOP_CMD = 0,           /*INFRADE nValue*/
    INFRADE_UP_CMD = 1,
    INFRADE_DOWN_CMD = 2,

    DISCHARGE_CMD = 4,            /*DISCHARGE nCommand*/
    DISCHARGE_STOP_CMD = 0,           /*DISCHARGE nValue*/
    DISCHARGE_FORWARD_CMD = 1,
    DISCHARGE_BACKWARD_CMD = 2,
 
    PTZ_COORD_CMD = 0,          /*PTZ coordinate get mCommand*/
};

enum FireDoorOptType {
    FIRE_DOOR_OPT_NONE = -1,
    FIRE_DOOR_OPT_CLOSED = 0,
    FIRE_DOOR_OPT_OPEN = 1,
};

struct ViewPointsRecord {

    int     nViewPointID;

    float   fTemperature;
    float   fHumidity;
    float   fCombustibleGas;
    float   fCO;
    float   fH2S;
    float   fO2;
    float   fThermalMaxTemperature;

    std::string  strCameraPhotoPath;
    std::string  strThermalPhotoPath;
};

struct TasksRecord {

    int     nTaskID;

    time_t  lStartTime;
    time_t  lStopTime;

    std::list<ViewPointsRecord> viewPointsRecord;
};

struct RechargeMonitorInfo {

    bool    bWatchDog;

    int     nClockCnt;
    int     nOffset;
    float   fBasePos;
};

struct RechargeHallMonitorInfo {

  bool    bWatchDog;
  bool    bHall;
  bool    bRunning;

  int     nHallTimeOut;

  int     nCurrentCnt;
  int     nCurrentTimeOut;
};
class Robot
{
public:
    int                 	m_nRobotID;           		//机器自身设备Id
    RobotMap::Ptr           m_pRobotCurMap;             //机器绑定当前地图
    ~Robot();
    static Robot *getInstance();
    int init();
    static void taskRotation(Robot *pRobot);
    static void ptzheartBeatThread(Robot* pRobot);
    static void heartBeatThread(Robot* pRobot);
    static void noOperationDetectThread(Robot *pRobot);
    static void armStandStillDetectThread(Robot *pRobot);
    static void motorCurrentProtectThread(Robot *pRobot);
    static void selfInspection(Robot *pRobot);
    void timingPublishRunData(void);
    static void lowBatteryDetectThread(Robot *pRobot);
    static void antiBatDischargeDetectThread(Robot *pRobot);
    static void powerSavingThread(Robot *pRobot);
    static void UpGrade(Robot *pRobot, std::string sReqName, std::string sReq, std::string& sRes);
    void getSortedChargingPilePos(float fCurPosition, std::list<float> &sortedPos, std::list<float> pilePos);
    float getNearestChargingPilePos(float nCurPosition);
    void setPowerSavingStatus(int num) { m_saving |= (1 << num); }
    void clearPowerSavingStatus(int num) { m_saving &= (~ (1 << num)); }
    ImageRecCommand *m_ImageRec;
	
	
    IndustrialCamera*  		m_pIndustrialCamera;  		//相机
    ThermalImager*      	m_pThermalImager;     		//热像仪
    int                 	m_nPhotoIndex;
    std::string         	m_sPhotoRootPath;      		//拍照存放的根目录
    std::string         	m_sPhotoRootPathDate;
    std::string         	m_sVideoRootPath;      		//视频存放路径
    std::string getTime();
    std::string getDate();
    int mkdirsByDate();
    int cacuCurDaySecond();
    time_t cacuCurSecond();
    void cacuDay(int time, int &day, int &curDaySecond);
    void cacuHour(int time, int &hour, int &curHourSecond);
    int updateBindMapInfo(int mapId);
    //多地图支持
    int getMapInfoFromDriver(RobotMap::Header & newMap, std::vector<RfidMapping> & rfidMap);
    int insertMapInfoIntoDB(RobotMap::Header & newMap, std::vector<RfidMapping> & rfidMap);
    int getMapInfoFromDB(const uint32_t mapId,                     //map id
                            RobotMap::Ptr &map,                    //map info
                            std::vector<RfidMapping>& rfidMap,     //RFID
                            std::list<float> &vChargePilePos,      //充电桩
                            std::list<Task> &taskList,             //巡检任务
                            std::list<PositionPlan> &posPlan       //定点动作
    );
    int setMapInfoIntoDriver(std::vector<RfidMapping>& rfid);
    int switchMap(int mapId);
    int bindSimpleMap(int mapId);

private:
    bool responseSetting(COMPkg::Setting::Request &setting, COMPkg::Setting::Response &res);
    bool responseControl(COMPkg::Control::Request &control, COMPkg::Control::Response &res);
    bool responsePTZControl(COMPkg::Control::Request &control, COMPkg::Control::Response &res);
    bool responseTasksSyncDown(COMPkg::SyncDown::Request &tasksReq, COMPkg::SyncDown::Response &res);
    void tasksRefresh(); //任务更新

    static void responseCommand(std::string sReq, std::string &sRes);
    static void upLoadPhotos(Robot *pRobot);

public:
	ChargePileService*      chargePileService;
    WarningManager*         m_pWarningManager;
    SettingService*         m_pSettingService;        
    TaskService*            m_pTaskService;
    ConfigServer*       	m_pConfigServer;      		//配置参数服务
    MySQLServer*        	m_pMySQLServer;       		//数据库操作服务器
    MoveTaskManage*     	m_pMoveTaskManage;    		//运动管理模块 
    Motor*                  m_pMotor;                   //运动模块
    PDDMotorDriver*         m_pPDD;                     //局放运动模块
    PartialDischargeDetectorDriver*  m_pDec;            //局放模块

    //来自驱动层的成员
    SensorBoardEx::Ptr      m_pSensorBoardEx;           //传感器驱动，包括气体、环境、污染物信息
    BatteryDriver*          m_pBatteryDriver;           //电池信息驱动
    PowerDriver*			m_pPowerDriver;             //电源控制驱动
    FireDoorDriver*         m_pFireDoorDriver;          //防火门驱动，支持多个防火门 
	ChargePileDriver*       m_pChargePileDriver;        //充电桩驱动，支持多个充电桩
    HallDriver*             m_pHallDriver;              //霍尔开关驱动，检测充电桩
	RadarManager*           m_pRadarManager;            //超声波   
    ZMQDriver*              m_pZmqDriver;               //消息中间件
    BusinessInfor*          m_pBusinessInfor;           //业务驱动
    PTZDriver::Ptr          m_pPTZDriver;               //云台驱动
	
    bool                	m_bReconnectCamera;   		//检测相机是否连接
    bool                	m_bReconnectThermel;  		//检测热像仪是否连接
    long long               m_saving;                   //省电模式

    std::list<float>    	m_chargingPilesPos;    		//充电桩详情
    std::list<string>       m_chargingPilesName;        //充电桩名称
    std::list<Task>     	m_taskList;            		//任务详情
    Capabilities*           m_pCapabilities;
    ImageProcessManager*    m_pImageProcessManager;
public:
    pthread_mutex_t mutex;
};

#endif // ROBOT_H
