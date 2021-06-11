/*
 * @Author: your name
 * @Date: 2020-11-16 18:04:53
 * @LastEditTime: 2020-11-17 09:41:06
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \cod\NEW_SLAVE\Slave\include\Transaction\Dialogue.h
 */
/* #ifndef DIALOGUE_H
#define DIALOGUE_H

#include "common_pkg/Command.h"
#include "common_pkg/CurStatus.h"
#include "Driver/ROSDriver.h"

enum PUBLISH_ID {
    PUB_CURSTATUS = 0,
    PUB_RECORD,
    PUB_CURSTATUSN,
    PUB_CURSTATUSNP,
    PUB_INSPECTSTATUS,
    PUB_OTHERCURSTATUS,
};

typedef boost::function<bool(common_pkg::Command::Request &, common_pkg::Command::Response &)> ResCommand;


class Dialogue
{
public:
   ~Dialogue();
    static Dialogue *getInstance();

    int init();
    void registerCallBack(ResCommand pResCommandCallback);
    int initRos(std::string sMasterUrl, std::string sHostUrl, int nRobotID);
    bool isRosAlive();
    void heartBeat();
    void spin();
    void publishCurStatus(common_pkg::CurStatus msg);
	void publishCurStatusN(common_pkg :: CurStatus msg);
    //2020-10-22
    void publishInspectStatus(common_pkg:: CurStatus msg);
    void publishOtherStatus(common_pkg:: CurStatus msg);
	//告警PLC
	void publishCurStatusNP(common_pkg :: CurStatus msg);

private:
    ROSDriver*          m_pROSDriver;

    ResCommand          m_pResCommandCallback;
};

#endif // DIALOGUE_H
 */
