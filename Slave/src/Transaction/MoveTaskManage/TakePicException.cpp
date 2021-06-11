#include "Transaction/MoveTaskManage/TakePicException.h"
#include "Transaction/MySQLServer.h"

void TakePicException::exceptionExit()
{
    MySQLServer* mySQLServer = MySQLServer::getInstance();
    ViewPointInfo viewPointInfo;
	int taskId = _paraJson["taskId"].asInt();
    Json::Value *sharedJson = getSharedParaJson();
    if(sharedJson == NULL) {
        printf("TakePicException::%s(), can't get json value!\n", __FUNCTION__);
        return;
    }
    int execId = (*sharedJson)[JSON_EXEC_ID].asInt();

    if(execId < 0) {
        printf("TakePicException::%s(), __LINE__ = %d: no exec id, return!\n", __FUNCTION__, __LINE__);
        return;
    }

    viewPointInfo.nTaskId = taskId;
    viewPointInfo.nExecId = execId;
    viewPointInfo.nExecRes = EXEC_FAIL;
    mySQLServer->changeExecResult(viewPointInfo);
}
