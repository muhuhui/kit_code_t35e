#include "Transaction/TaskService.h"

typedef 
Poco::Tuple
<
int,          //task_id
std::string,  //task_name
int,          //temp_id
int,          //robot_id
int,          //task_type
time_t,       //start_time
time_t,       //end_time
float,        //speed
int,          //repeat_unit
int          //repeat_period
> 
TupleTask;

typedef Poco::Tuple
<
int,          //point_id
std::string,  //name
int,          //area_id
int,          //arm_position
int,          //position
int,          //preset_number
int,          //industrial_camera_focus
int,          //thermal_imager_focus
std::string,  //pic_url
std::string,  //instance_name
int,           //pdd_dis
int,          //is_picture
int,          //is_video
int,           //is_pdd
int,          //is_noise
float,          //x1
float,          //y1
float,          //x2
float          //y2
> 
TupleViewPoint;

typedef Poco::Tuple
<
int,          //view_type
int           //over_type
>
TupleTemplate;

typedef Poco::Tuple
<
int,
int,
int,
int
>
TuplePresetInfo;

TaskService::~TaskService()
{
}

//单例实例�?
TaskService* TaskService::getInstance()
{
	 return SingleTon<TaskService>::GetInstance();
}

int TaskService::init()
{
  m_pMySQLDriver = MySQLDriver::getInstance();
  return 0;
}



int TaskService::queryTask(int robotId, int mapId, std::list<Task> &taskList)
{
    int nRet = 0;
	std::vector<TupleTask> vTupleTask;
	try {

    string sqlStr = Poco::format("SELECT task_id, task_name, temp_id, robot_id, task_type, UNIX_TIMESTAMP(start_time), UNIX_TIMESTAMP(end_time), speed, repeat_unit, repeat_period FROM task v, task_template t WHERE v.temp_id = t.id AND t.map_id = %d AND robot_id = %d;", mapId, robotId);
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr, into(vTupleTask), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "TaskService::queryTask " << exc.displayText() << std::endl;
      }
      Task task;
      for(int i = 0; i < vTupleTask.size(); ++i)
      {
          task.setTaskId(vTupleTask[i].get<0>());
          task.setTaskName(vTupleTask[i].get<1>());
          task.setTempId(vTupleTask[i].get<2>());
          task.setRobotId(vTupleTask[i].get<3>());
          task.setTaskType(vTupleTask[i].get<4>());
          task.setStartTime(vTupleTask[i].get<5>());
          task.setEndTime(vTupleTask[i].get<6>());
          task.setSpeed(vTupleTask[i].get<7>());
          task.setRepeatUnit(vTupleTask[i].get<8>());
          task.setRepeatPeriod(vTupleTask[i].get<9>());
          queryTemplate(task);
          std::list<ViewPoint> viewPoints;
          queryViewPointList(task.getTempId(),viewPoints); 
          task.setViewPoints(viewPoints);
          taskList.push_back(task);
      }
    }
	ses.close();
    nRet = 0;
	
  } catch(Poco::Exception& exc) {
    std::cerr << "TaskService::queryTask " << exc.displayText() << std::endl;
    nRet = -1;
  }
  return nRet;
}

int TaskService::queryTemplate(Task &task)
{
	TupleTemplate vTupleTemplate;
	try {
        string sqlStr = Poco::format("SELECT view_type,over_type FROM task_template WHERE id=%d;", task.getTempId());
        Session ses = m_pMySQLDriver->getSession();
        if(ses.isConnected()) {
          try {
              Statement statement(ses);
              statement << sqlStr, into(vTupleTemplate), now;
          } catch(Poco::Exception& exc) {
            std::cerr << "TaskService::queryTemplate " << exc.displayText() << std::endl;
          }

          task.setViewType(vTupleTemplate.get<0>());
          task.setOverType(vTupleTemplate.get<1>());
        }
		ses.close();
    } catch(Poco::Exception& exc) {
        std::cerr << "TaskService::queryTemplate " << exc.displayText() << std::endl;
        return -1;
    }
    return 0;
}

int TaskService::queryViewPointList(int taskTemplateId, std::list<ViewPoint> &viewPointList)
{
  std::vector<TupleViewPoint> vpTupleList;

  try {

    string sqlStr = Poco::format("SELECT point_id, viewpoint_name, area_id, arm_position, position, preset_number, industrial_camera_focus,thermal_imager_focus,pic_url, instance_name, pdd_dis, is_picture, is_video, is_pdd, is_noise, x1, y1, x2, y2"
    							" FROM view_point v, template_viewpoint_id t WHERE v.point_id=t.viewpoint_id AND template_id=%d;", taskTemplateId);
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr, into(vpTupleList), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "TaskService::queryViewPointList " << exc.displayText() << std::endl;
      }
      ViewPoint viewPoint;
      for(int i = 0; i < vpTupleList.size(); ++i) 
      {
        viewPoint.setPointId(vpTupleList[i].get<0>());
        viewPoint.setName(vpTupleList[i].get<1>());
        viewPoint.setAreaId(vpTupleList[i].get<2>());
        viewPoint.setArmPosition(vpTupleList[i].get<3>());
        viewPoint.setPosition(vpTupleList[i].get<4>());
        viewPoint.setPresetNumber(vpTupleList[i].get<5>());
        viewPoint.setIndustrialCameraFocus(vpTupleList[i].get<6>());
        viewPoint.setThermalImagerFocus(vpTupleList[i].get<7>());
        viewPoint.setPicUrl(vpTupleList[i].get<8>()); 
        viewPoint.setInstanceName(vpTupleList[i].get<9>());
        viewPoint.setPddDis(vpTupleList[i].get<10>());
        viewPoint.setPic((bool)vpTupleList[i].get<11>());
        viewPoint.setVideo((bool)vpTupleList[i].get<12>());
        viewPoint.setPdd((bool)vpTupleList[i].get<13>());
        viewPoint.setX1((float)vpTupleList[i].get<15>());
        viewPoint.setY1((float)vpTupleList[i].get<16>());
        viewPoint.setX2((float)vpTupleList[i].get<17>());
        viewPoint.setY2((float)vpTupleList[i].get<18>());

        PresetInfo presetInfo;
        queryPresetInfo(viewPoint.getPresetNumber(), presetInfo);
        viewPoint.setPresetInfo(presetInfo);

        viewPointList.push_back(viewPoint);
      }
    }
	ses.close();
  } catch(Poco::Exception& exc) {
    std::cerr << "TaskService::queryViewPointList " << exc.displayText() << std::endl;
    return -1;
  }
  return 0;
}

int TaskService::queryPresetInfo(int presetNumber, PresetInfo &presetInfo)
{
  TuplePresetInfo tuplePresetInfo;

  try {

    string sqlStr = Poco::format("SELECT x_pos, y_pos, z_pos, zoom FROM standalone_preset_info s, view_point v WHERE s.id=v.preset_number AND preset_number=%d;", presetNumber);
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr, into(tuplePresetInfo), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "TaskService::queryPresetInfo " << exc.displayText() << std::endl;
      }
      presetInfo.x_pos = tuplePresetInfo.get<0>();
      presetInfo.y_pos = tuplePresetInfo.get<1>();
      presetInfo.z_pos = tuplePresetInfo.get<2>();
      presetInfo.zoom = tuplePresetInfo.get<3>();
    }
	ses.close();
  } catch(Poco::Exception& exc) {
    std::cerr << "TaskService::queryPresetInfo " << exc.displayText() << std::endl;
    return -1;
  }
  return 0;
}
