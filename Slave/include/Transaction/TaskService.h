#pragma once

#include <vector>
#include "Task.h"
#include "Driver/MySQLDriver.h"



class TaskService
{
public:
  ~TaskService();
  static TaskService* getInstance();
  int init();
  int queryTask(int robotId, int mapId, std::list<Task> &taskList);
  int queryTemplate(Task &task);
  int queryViewPointList(int taskTemplateId, std::list<ViewPoint> &viewPointList);
  int queryPresetInfo(int presetNumber, PresetInfo &presetInfo);

private:
	MySQLDriver*    m_pMySQLDriver;
};

