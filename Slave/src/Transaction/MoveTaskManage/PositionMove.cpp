#include "Transaction/MoveTaskManage/PositionMove.h"
#include "Transaction/Robot.h"
#include "Transaction/Motor.h"
#include <thread>
#include <unistd.h>
#include <pthread.h>
#include "Driver/MyLog4CXX.h"

const std::string LOG_TAG("root.PositionMove::");

pthread_mutex_t g_mutex;

PositionMove::~PositionMove()
{

}

PositionMove *PositionMove::getInstance()
{
  return SingleTon<PositionMove>::GetInstance();
}

int PositionMove::init()
{
  auto logger = initLogger(LOG_TAG + std::string(__FUNCTION__));
  Robot *pRobot = Robot::getInstance();
  m_RobotId = pRobot->m_nRobotID;
  if (pRobot->m_pRobotCurMap != NULL) {
      m_nMapId = pRobot->m_pRobotCurMap->mapHeader().id;
  }
  m_ThreadRun = false;
  m_ready = false;

  updatePosPlan();

  m_ready = true;
  printf("Position Move detector is ready--------\n");
  LOG4CPLUS_INFO(logger, "Position Move detector is ready " );
  return 0;
}

void PositionMove::movementTrigger(PositionMove* posMove)
{
    auto logger = initLogger(LOG_TAG + std::string(__FUNCTION__));
    printf("position trigger start---------------\n");
    LOG4CPLUS_INFO(logger, " position trigger start " );
    
    Motor* mDriver = Motor::getInstance();
    MoveTaskManage* mtManage = MoveTaskManage::getInstance();
    const int t_interval = 100000;
    int currPos = 0;

    while (posMove->m_ThreadRun) 
    {
        currPos = mDriver->getCurPosition();
        pthread_mutex_lock(&g_mutex);
        std::list<PositionPlan>::iterator iter = posMove->m_PosPlan.begin();
        while (iter != posMove->m_PosPlan.end()) {
          if (currPos >= iter->position) {
              if (iter->robot_position == -1) {
                  mtManage->createPositionPlanTaskItem(*iter, AHEAD_STATE);
              }
              iter->robot_position = 1;
          }
          else {
              if(iter->robot_position == 1) {
                  mtManage->createPositionPlanTaskItem(*iter, BACK_STATE);
              }
              iter->robot_position = -1;
          }
          iter++;
        }
        pthread_mutex_unlock(&g_mutex);
        usleep(t_interval);
    }
    printf("position trigger end---------------\n");
    LOG4CPLUS_INFO(logger, " position trigger end " );

}

int PositionMove::updatePosPlan()
{
  auto logger = initLogger(LOG_TAG + std::string(__FUNCTION__));
  pthread_mutex_lock(&g_mutex);
  m_PosPlan.clear();
  printf("Position Move detector is initiating with robot %d--------\n",m_RobotId);
  LOG4CPLUS_INFO(logger, "Position Move detector is initiating with robot " << m_RobotId);
  PositionPlanService::queryPPListByRobotMap(m_RobotId, m_nMapId, m_PosPlan);
  for(std::list<PositionPlan>::iterator iter = m_PosPlan.begin(); 
      iter != m_PosPlan.end(); iter++){
    printf("get plan %d at position %d mm----------------\n", iter->getId(), iter->getPosition());
    LOG4CPLUS_INFO(logger, "get plan " << iter->getId() << " at position " << iter->getPosition() <<" mm");
  }

  if((m_PosPlan.size()>0) && (m_ThreadRun == false)){
    m_ThreadRun = true;
    std::thread runThread;
    runThread = std::thread(PositionMove::movementTrigger, this);
    runThread.detach();
  }
  pthread_mutex_unlock(&g_mutex);

  return 0;
}
