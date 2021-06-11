#include "Driver/FireDoorDriver.h"
#include <thread>
#include <unistd.h>


// #define SLAVE_ID    0x01
// #define ADDR_W      0x00
// #define ADDR_R      0x01
// #define NUMBITS     0x01

FireDoorDriver::~FireDoorDriver()
{
  // m_bThreadRun = false;
}

FireDoorDriver *FireDoorDriver::getInstance()
{
  return SingleTon<FireDoorDriver>::GetInstance();
}

// int FireDoorDriver::init(std::map<int, char* > fireDoorsMap)
// {
//   m_bThreadRun = true;
//   m_bSwitch = false;
//   m_bOn = true;
//   m_nSwitchIndex = -1;

//   resetMap(fireDoorsMap);

//   /* std::thread runThread;
//   runThread = std::thread(FireDoorDriver::run, this);
//   runThread.detach(); */

//   return 0 ;
// }

// void FireDoorDriver::resetMap(std::map<int, char* > fireDoorsMap)
// {
//   m_statusMap.clear();
//   m_fireDoorsMap.clear();
//   std::map<int, char* >::iterator iter = fireDoorsMap.begin();
//   while (iter != fireDoorsMap.end() ) {
//     m_fireDoorsMap.insert(*iter);
//     m_statusMap.insert(std::make_pair(iter->first, FD_CLOSED));
//     ++iter;
//   }
// }

// std::map<int, FireDoorStatus> *FireDoorDriver::getStatus()
// {
//   return &m_statusMap;
// }

// bool FireDoorDriver::switchStatus(int nIndex, bool bOn)
// {
//   m_bOn = bOn;
//   m_nSwitchIndex = nIndex;
//   m_bSwitch = true;
// }

int FireDoorDriver::init(int nRobotID){
  // m_pRosDriver = ROSDriver::getInstance();
  m_eSubType = SUB_FIREDOOR; 
  
/*   if(!ros::master::check())
  {
      printf("--- %s: loss connection\n", __FUNCTION__);
      return -1;
  } */

  char sCmds[64] = {0};
  sprintf(sCmds, "fire_door_controller");
  // m_pRosDriver->registerClientCallBack<fd_common_pkg::FDController>(CLIENT_FDCONTROL_CMD, sCmds);
  // std::thread runThread;
  // runThread = std::thread(FireDoorDriver::run, this);
  // runThread.detach();
  return 0;
}

int FireDoorDriver::executeCmd(FDCmdType eCmdType, std::string ip, int para, int res[]){
/*   fd_common_pkg::FDController FDClient;
  FDClient.request.cmd = eCmdType;
  FDClient.request.doorIPs = ip;
  FDClient.request.FDValue = para;

  // if(m_pRosDriver->callServer(CLIENT_FDCONTROL_CMD, FDClient)){
  if(0){
    res[0] = (int)FDClient.response.nErrorCode;
    res[1] = (int)FDClient.response.nStatus;
  }else{
    return -1;
  } */

  return 0;
}

int FireDoorDriver::getCurStatus(std::string ip){
  int res[2];
  int ret;
  ret = executeCmd(GET_STATUS_CMD, ip, 0, res);
  if(ret == -1){
    return -1;
  }else{
    return res[1];
  }
}

int FireDoorDriver::closeFireDoor(std::string ip){
  int res[2];
  int ret;
  ret = executeCmd(CLOSE_DOOR_CMD, ip, 0, res);
  if(ret == -1){
    return -1;
  }else{
    return 0;
  }
}

int FireDoorDriver::openFireDoor(std::string ip){
  int res[2];
  int ret;
  ret = executeCmd(OPEN_DOOR_CMD, ip, 1, res);
  if(ret == -1){
    return -1;
  }else{
    return 0;
  }
}

void FireDoorDriver::attach(Observers *pObserver)
{
  m_ObserverList.push_back(pObserver);
}

void FireDoorDriver::detach(Observers *pObserver)
{
  m_ObserverList.remove(pObserver);
}

void FireDoorDriver::notify()
{
  std::list<Observers *>::iterator it = m_ObserverList.begin();
  while (it != m_ObserverList.end())
  {
    (*it)->update(SUB_FIREDOOR);
    ++it;
  }
}

// void FireDoorDriver::flushStatus()
// {
//   int nRet = 0;
//   bool bStatusChanged = false;
//   FireDoorStatus tmpStatus;

//   std::map<int, char* >::iterator iterI = m_fireDoorsMap.begin();
//   while (iterI != m_fireDoorsMap.end() ) {

//     char strCmd[128];
//     sprintf(strCmd, "Config/checkIPStatus.sh %s", iterI->second);
//     nRet = system(strCmd);

//     if( nRet != 0 ) {
//       tmpStatus = FD_OFFLINE;
//     } else {

//       tmpStatus = FD_ERROR;
//       nRet = connect(iterI->second, 502);
//       if(0 == nRet) {

//         uint8_t val = 0;
//         nRet = readBits(MODBUS_TCP, SLAVE_ID, ADDR_R, NUMBITS, &val);
//         if(0 == nRet) {

//           tmpStatus = (FireDoorStatus)val;
//         }
//       }
//     }

//     if(m_statusMap[iterI->first] != tmpStatus) {

//       m_statusMap[iterI->first] = tmpStatus;
//       bStatusChanged = true;
//     }
//     ++iterI;
//   }

//   if(bStatusChanged) {
//     notify();
//   }
// }

// void FireDoorDriver::run(FireDoorDriver *pFireDoorDriver)
// {
//   printf("--- FireDoorDriver run Start...\n");

//   const int nBaseFrequence = 200000;   //0.2s

//   while (pFireDoorDriver->m_bThreadRun) {

//     if(false == pFireDoorDriver->m_bSwitch) {

//       pFireDoorDriver->flushStatus();

//     } else {

//       int nStatus = pFireDoorDriver->m_bOn ? FD_CLOSED : FD_OPENED;
//       pFireDoorDriver->writeRegister(MODBUS_TCP, SLAVE_ID, ADDR_W, nStatus);
//       pFireDoorDriver->m_bSwitch = false;
//     }

//     usleep(nBaseFrequence);
//   }
//   printf("--- FireDoorDriver run Over...\n");
// }
void FireDoorDriver::run(FireDoorDriver *pFireDoorDriver){
  printf("--- FireDoorDriver run Start...\n");
  const int nBaseFrequence = 200000;   //0.2s
  int flag = 0;
  while(1){
    if(flag == 0){
      pFireDoorDriver->closeFireDoor("192.168.10.189");
      flag = 1;
    }else{
      pFireDoorDriver->openFireDoor("192.168.10.189");
      flag = 0;
    }
    usleep(nBaseFrequence);
  }
}
