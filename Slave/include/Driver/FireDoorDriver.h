/*
 * @Author: your name
 * @Date: 2020-11-16 18:04:53
 * @LastEditTime: 2020-11-17 10:25:27
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edits
 * @FilePath: \cod\NEW_SLAVE\Slave\include\Driver\FireDoorDriver.h
 */
#ifndef FIREDOORDRIVER_H
#define FIREDOORDRIVER_H


#include "base/SingleTon.h"
#include "base/ModbusBase.h"
#include "base/Subjects.h"
#include "base/Observers.h"
// #include "base/ROSBase.h"
// #include "Driver/ROSDriver.h"
// #include "fd_common_pkg/FDController.h"
#include <list>
// #include <map>
// #include <unistd.h>


enum FDStatus {
  FD_ERROR = 0,
  FD_CLOSED = 1,
  FD_OPENED = 2,
  FD_OFFLINE = 3,
};

enum FDCmdType {
  GET_STATUS_CMD = 1,    
  CLOSE_DOOR_CMD, 
  OPEN_DOOR_CMD,       
};

class FireDoorDriver : public ModbusBase, Subjects
{
public:
  ~FireDoorDriver();
  static FireDoorDriver *getInstance();

  // int init(std::map<int, char* > fireDoorsMap);
  // void resetMap(std::map<int, char* > fireDoorsMap);
  // std::map<int, FireDoorStatus> *getStatus();
  // bool switchStatus(int nIndex, bool bOn);

  int init(int nRobotID);
  int getCurStatus(std::string ip);
  int openFireDoor(std::string ip);
  int closeFireDoor(std::string ip);

  void attach(Observers *pObserver);
  void detach(Observers *pObserver);
  void notify();

  static void run(FireDoorDriver *pFireDoorDriver);

private:
  // void flushStatus();
  int executeCmd(FDCmdType eCmdType, std::string ip, int para, int res[]);

private:
  // bool                          m_bThreadRun;

  // bool                          m_bSwitch;
  // bool                          m_bOn;
  // int                           m_nSwitchIndex;
  SubType                   m_eSubType;
  std::list<Observers*>         m_ObserverList;
  // ROSBase*                  m_pRosBase;
  // ROSDriver*                  m_pRosDriver;
  // std::map<int, FireDoorStatus> m_statusMap;
  // std::map<int, char* >         m_fireDoorsMap;

};
#endif // FIREDOORDRIVER_H
