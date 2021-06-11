#include "Transaction/MySQLServer.h"
#include <string.h>


MySQLServer::~MySQLServer()
{
}

MySQLServer *MySQLServer::getInstance()
{
  return SingleTon<MySQLServer>::GetInstance();
}

int MySQLServer::init(std::string sServerIp, std::string sServerPort, std::string sDataBase,
                      std::string sUserName, std::string sPasswd)
{
  int nRet = 0;

  m_pMySQLDriver = MySQLDriver::getInstance();
  nRet = m_pMySQLDriver->init(sServerIp, sServerPort, sDataBase, sUserName, sPasswd);

  try {
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected()) {
      nRet = 0;
    }
  } catch(Poco::Exception& exc) {
    nRet = -1;
    std::cerr << "MySQLServer::init " << exc.displayText() << std::endl;
  }

  return nRet;
}

std::string MySQLServer::getSettingParam(std::string strName, int robortId)
{
  std::string strRet = "0";
  try {

    std::vector<std::string> vstrValues;
    char pSQL[1024];
    snprintf(pSQL, sizeof(pSQL), "SELECT parameter_value FROM parameters as a left JOIN parameter_config as b \
             on b.config_id = a.config_id where robot_id = %d and parameter_name ='%s'", \
        robortId, strName.c_str());
    // printf("pSQL %s\n",pSQL);
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement<< pSQL, into(vstrValues), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::getSettingParam " << exc.displayText() << std::endl;
      }
    }
    ses.close();

    if (vstrValues.size() > 0)
    {
      strRet = vstrValues[0];
    }
    else
    {
      strRet = "0";
    }

  } catch(Poco::Exception& exc) {

    std::cerr << "MySQLServer::getSettingParam " << exc.displayText() << std::endl;
  }

  return strRet;
}

bool MySQLServer::setSettingParam(std::string strName, std::string strValue,int robortid)
{
  try {

    std::string strSQL;
    strSQL = getSettingParam(strName,robortid);
    char pSQL[1024];
    if (strSQL.length() == 0)
    {
      snprintf(pSQL, sizeof(pSQL), "INSERT INTO parameters(config_id, robot_id, parameter_value) \
               SELECT a.config_id, %d, '%s' FROM parameter_config as a where a.parameter_name = '%s'",
          robortid,strValue.c_str(), strName.c_str());

    }
    else
    {
      snprintf(pSQL, sizeof(pSQL), "REPLACE INTO parameters(parameters.parameter_id, config_id, robot_id, parameters.parameter_value) \
               SELECT a.parameter_id, a.config_id, a.robot_id, '%s' FROM parameters as a LEFT JOIN parameter_config as b \
               on b.config_id = a.config_id where b.parameter_name = '%s' and a.robot_id = %d",
          strValue.c_str(), strName.c_str(), robortid);
    }

    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << pSQL;
          statement.execute();
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::setSettingParam " << exc.displayText() << std::endl;
      }
    }
	ses.close();

  } catch(Poco::Exception& exc) {

    std::cerr << "MySQLServer::setSettingParam " << exc.displayText() << std::endl;
    return false;
  }

  return true;
}

int MySQLServer::recordPictureInfo(PictureInfo ePictureInfo)
{
  try {

    char pSQL[1024];
    snprintf(pSQL, sizeof(pSQL), "insert into picture (time,position,path,robot_id) VALUES(now(),%f,'%s',%d)",
             ePictureInfo.position,
             ePictureInfo.path.c_str(),
             ePictureInfo.nRobotId);

    printf("pSQL %s\n",pSQL);
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Poco::Data::Statement statement(ses);
          statement << pSQL,  now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::recordPictureInfo " << exc.displayText() << std::endl;
      }
    }
	ses.close();

  } catch(Poco::Exception& exc) {

    std::cerr << "MySQLServer::recordViewPointInfo " << exc.displayText() << std::endl;
    return -1;
  }

  return 0;
}

int MySQLServer::recordViewPointInfo(ViewPointInfo eViewPointInfo)
{
  try {

    char pSQL[1024];
    snprintf(pSQL, sizeof(pSQL), "insert into task_detail_record (exec_id,point_id,create_time,temperature,humidity,combustible_gas_concent,co_concent,h2s_concent,o2_concent,thermal,camera_photo_path,thermal_pthoto_path,position,tev_result,uw_result,camera_video_path,thermal_video_path) VALUES(%d,%d,now(),%f,%f,%f,%f,%f,%f,%f,'%s','%s',%d,%d,%d,'%s','%s')",
             eViewPointInfo.nExecId,
             eViewPointInfo.nViewPointId,
             eViewPointInfo.fTemperature,
             eViewPointInfo.fHumidity,
             eViewPointInfo.fCombustible,
             eViewPointInfo.fCO,
             eViewPointInfo.fH2S,
             eViewPointInfo.fO2,
             eViewPointInfo.fThermelMaxTemp,
             eViewPointInfo.strCameraPhotoPath.c_str(),
             eViewPointInfo.strThermelPhotoPath.c_str(),
             eViewPointInfo.nposition,
             eViewPointInfo.TEV,
             eViewPointInfo.UW,
             eViewPointInfo.strCameraVideoPath.c_str(),
             eViewPointInfo.strThermelVideoPath.c_str());

    printf("pSQL %s\n",pSQL);
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Poco::Data::Statement statement(ses);
          statement << pSQL,  now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::recordViewPointInfo " << exc.displayText() << std::endl;
      }
    }
	ses.close();

  } catch(Poco::Exception& exc) {

    std::cerr << "MySQLServer::recordViewPointInfo " << exc.displayText() << std::endl;
    return -1;
  }

  return 0;
}

int MySQLServer::recordImageIdentifyResult(ViewPointInfo viewPointInfo, EquInfo equInfo, CHttpPost::IdentifyResult result)
{
  try {
    int recordId = -1;
    char pSQL[1024];
    snprintf(pSQL, sizeof(pSQL), "SELECT MAX(record_id) FROM task_detail_record WHERE exec_id = %d AND point_id = %d;", 
             viewPointInfo.nExecId, 
             viewPointInfo.nViewPointId);
    printf("pSQL %s\n",pSQL);
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Poco::Data::Statement statement(ses);
          statement << pSQL, into(recordId), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::recordImageIdentifyResult " << exc.displayText() << std::endl;
      }
    }
	ses.close();

    if(recordId == -1) { return -1; }

    snprintf(pSQL, sizeof(pSQL), "INSERT INTO vp_apparatus_record (record_id, equ_id, result, is_alarm, distinguish_time, err_code, err_msg) VALUES(%d, %d, '%s', %d, '%s', '%s','%s');",
             recordId,
             equInfo.nEquId,
             (result.value + " " + result.item_unit).c_str(),
             result.isAlarm,
             result.time.c_str(),
             result.errCode.c_str(),
             result.errMsg.c_str());

    printf("pSQL %s\n",pSQL);
    ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Poco::Data::Statement statement(ses);
          statement << pSQL,  now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::recordImageIdentifyResult " << exc.displayText() << std::endl;
      }
    }
	ses.close();

  } catch(Poco::Exception& exc) {

    std::cerr << "MySQLServer::recordImageIdentifyResult " << exc.displayText() << std::endl;
    return -1;
  }

  return 0;
}

int MySQLServer::insertExecResult(ViewPointInfo &eViewPointInfo)
{
  try {

    char pSQL[1024];
    snprintf(pSQL, sizeof(pSQL), "insert into task_exec_record (task_id,exec_time) VALUES(%d,FROM_UNIXTIME(%d))",
             eViewPointInfo.nTaskId,
             eViewPointInfo.nExecTime);

    printf("pSQL %s\n",pSQL);
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Poco::Data::Statement statement(ses);
          statement << pSQL,  now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::insertExecResult " << exc.displayText() << std::endl;
      }
    }
	ses.close();
  } catch(Poco::Exception& exc) {

    std::cerr << "MySQLServer::insertExecResult " << exc.displayText() << std::endl;
    return -1;
  }

  return 0;
}

int MySQLServer::getCurExecId(ViewPointInfo &eViewPointInfo)
{
  try {
    char pSQL[1024];
    snprintf(pSQL, sizeof(pSQL), "select MAX(exec_id) from task_exec_record WHERE task_id = %d", eViewPointInfo.nTaskId);

    printf("pSQL %s\n",pSQL);
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statementRtn(ses);
          statementRtn << pSQL, into(eViewPointInfo.nExecId), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::getCurExecId " << exc.displayText() << std::endl;
      }
    }
	ses.close();
  } catch(Poco::Exception& exc) {

    std::cerr << "MySQLServer::getCurExecId " << exc.displayText() << std::endl;
    return -1;
  }

  return 0;
}

int MySQLServer::changeExecResult(ViewPointInfo eViewPointInfo)
{
  try {

    char pSQL[1024];
    snprintf(pSQL, sizeof(pSQL), "UPDATE task_exec_record SET end_time=now(),exec_res=%d WHERE task_id=%d AND exec_id=%d",
            eViewPointInfo.nExecRes,
            eViewPointInfo.nTaskId,
            eViewPointInfo.nExecId);

    printf("pSQL %s\n",pSQL);
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Poco::Data::Statement statement(ses);
          statement << pSQL,  now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::changeExecResult " << exc.displayText() << std::endl;
      }
    }
	ses.close();

  } catch(Poco::Exception& exc) {

    std::cerr << "MySQLServer::changeExecResult " << exc.displayText() << std::endl;
    return -1;
  }

  return 0;
}

int MySQLServer::queryPointEquInfo(ViewPointInfo eViewPointInfo, std::list<EquInfo> &equInfoList)
{
  try {
    std::vector<TupleEquInfo> vTupleEquInfo;
    char pSQL[1024];
    snprintf(pSQL, sizeof(pSQL), "SELECT equ_id, inner_id, name, struct_config_id, alarm_min_threshold, alarm_max_threshold, alarm_rules, equip_type, min_range, max_range, type_id, item_unit, b_box FROM view_point_equ v, equipment e WHERE v.view_point_id=%d AND e.id=v.equ_id ORDER BY inner_id ASC;",
            eViewPointInfo.nViewPointId);

    printf("pSQL %s\n",pSQL);
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Poco::Data::Statement statement(ses);
          statement << pSQL, into(vTupleEquInfo), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::queryPointEquInfo " << exc.displayText() << std::endl;
      }
    }
    ses.close();

    EquInfo info;
    for (size_t i = 0; i < vTupleEquInfo.size(); i++) {
        info.nEquId = vTupleEquInfo[i].get<0>();
        info.nInnerId = vTupleEquInfo[i].get<1>();
        info.sEquName = vTupleEquInfo[i].get<2>();
        info.nAlarmDType = vTupleEquInfo[i].get<3>();
        info.sAlarmMin = vTupleEquInfo[i].get<4>();
        info.sAlarmMax = vTupleEquInfo[i].get<5>();
        info.nAlarmRule = vTupleEquInfo[i].get<6>();
        info.nEquType = vTupleEquInfo[i].get<7>();
        info.sMinRange = vTupleEquInfo[i].get<8>();
        info.sMaxRange = vTupleEquInfo[i].get<9>();
        info.ntypeId = vTupleEquInfo[i].get<7>();
        info.sitemUnit = vTupleEquInfo[i].get<11>();
        info.sbBox = vTupleEquInfo[i].get<12>();
        if (info.nAlarmDType > 4) {
            queryValueMap(info.nAlarmDType, info.valueMap);
        }
        else {
            info.valueMap.clear();
        }

        info.objectSet.clear();
        if (info.nAlarmRule == 6 || info.nAlarmRule == 7) {
            querySet(info.nEquId, info.objectSet);
        }
        
        equInfoList.push_back(info);
    }
  }
  catch(Poco::Exception& exc) {
    std::cerr << "MySQLServer::queryPointEquInfo " << exc.displayText() << std::endl;
    return -1;
  }

  return 0;
}

int MySQLServer::querySet(int equId, std::set<int>& objectSet) 
{
  try {
    std::vector<int> ValueInfo;
    char pSQL[1024];
    snprintf(pSQL, sizeof(pSQL), "SELECT value FROM equip_struct_config_item v, equip_struct_select s WHERE s.equip_id=%d AND s.equip_struct_config_item_id=v.id;",
            equId);
    printf("pSQL %s\n",pSQL);
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Poco::Data::Statement statement(ses);
          statement << pSQL, into(ValueInfo), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::queryValueMap " << exc.displayText() << std::endl;
      }
    }
    ses.close();

    for (size_t i = 0; i < ValueInfo.size(); i++) {
        objectSet.insert(ValueInfo[i]);
    }
  }
  catch(Poco::Exception& exc) {
    std::cerr << "MySQLServer::querySet " << exc.displayText() << std::endl;
    return -1;
  }

  return 0;
}

int MySQLServer::insertAlarm(AlarmInfo vAlarmInfo)
{
  int nNum = 0;
  try {

    std::string sqlStr = Poco::format("INSERT INTO alarm SELECT NULL, a.item_id, '%s', '%f', %d, '%s' from alarm_level_item_config as a \
                                      where a.item_name = '%s'", \
        vAlarmInfo.strAlarmTime, vAlarmInfo.dPosition, vAlarmInfo.nRobotID, vAlarmInfo.strReason,vAlarmInfo.strItemName);

    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr, now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::insertAlarm " << exc.displayText() << std::endl;
      }
    }
	ses.close();
    char pSQL[1024];
    snprintf(pSQL, sizeof(pSQL), "select MAX(alarm_id) from alarm WHERE robot_id = %d", vAlarmInfo.nRobotID);

    Session sesRtn = m_pMySQLDriver->getSession();
    if(sesRtn.isConnected())
    {
      try {
          Statement statementRtn(sesRtn);
          statementRtn << pSQL, into(nNum), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::insertAlarm " << exc.displayText() << std::endl;
      }
    }
	sesRtn.close();
  } catch(Poco::Exception& exc) {

    std::cerr << "MySQLServer::insertAlarm " << exc.displayText() << std::endl;
    return -1;
  }

  return nNum;
}

bool MySQLServer::queryAlarmNearby(AlarmInfo vAlarmInfo, int nIntervalSeconds)
{
  int nNum = 0;
  try {

    std::string sqlStr = Poco::format("select COUNT(*) from alarm as a LEFT JOIN alarm_item_config as b on b.item_id = a.item_id \
                                      where b.item_name = '%s' and abs(TIMESTAMPDIFF(SECOND, alarm_time, '%s')) < %d and robot_id = %d", \
        vAlarmInfo.strItemName, vAlarmInfo.strAlarmTime, nIntervalSeconds, vAlarmInfo.nRobotID);

    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr, into(nNum), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::queryAlarmNearby " << exc.displayText() << std::endl;
      }
    }
	ses.close();

  } catch(Poco::Exception& exc) {

    std::cerr << "MySQLServer::queryAlarmNearby " << exc.displayText() << std::endl;
    return -1;
  }

  return nNum > 0 ? true : false;
}

float MySQLServer::queryAlarmNearbydis(AlarmInfo vAlarmInfo)
{
  float pos = -1;
  try {

    string sqlStr = Poco::format("select alarm_position from alarm where alarm_id = (select MAX(alarm_id) from alarm as a LEFT JOIN alarm_level_item_config as b on b.item_id = a.item_id where b.item_name = '%s') AND robot_id = %d",\
                                 vAlarmInfo.strItemName,vAlarmInfo.nRobotID);
    //printf("sqlStr %s\n",sqlStr.c_str());
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr,into(pos), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::queryAlarmNearbydis " << exc.displayText() << std::endl;
      }
    }
	ses.close();

  } catch(Poco::Exception& exc) {

    std::cerr << "MySQLServer::queryAlarmNearbydis " << exc.displayText() << std::endl;
    return -1;
  }

  return pos;
}
void MySQLServer::getAlarmAction(std::string strItemName, std::vector<int>& actionid)
{
  try {

    std::string sqlStr = Poco::format("SELECT action_id FROM alarm_action WHERE level_id=(SELECT level_id FROM alarm_level_item_config\
                                      WHERE item_name ='%s') AND robot_id = %d;", strItemName,m_nRobotID);
    //    printf("sqlStr %s\n",sqlStr.c_str());
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr,into(actionid), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::getAlarmAction " << exc.displayText() << std::endl;
      }
    }
	ses.close();

  } catch(Poco::Exception& exc) {

    std::cerr << "MySQLServer::getAlarmAction " << exc.displayText() << std::endl;
  }
}

void MySQLServer::getAvoidObstacleByID(std::vector<TupleIsolatedZone> &vTupleIsolatedZone)
{
  try {

    std::string sqlStr = Poco::format("SELECT startpos,endpos FROM avoid_obstacle WHERE robot_id = %d;",m_nRobotID);
    //printf("getAvoidObstacle sqlStr %s\n",sqlStr.c_str());
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr,into(vTupleIsolatedZone), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::getAvoidObstacleByID " << exc.displayText() << std::endl;
      }
    }
	ses.close();

  } catch(Poco::Exception& exc) {

    std::cerr << "MySQLServer::getAvoidObstacleByID " << exc.displayText() << std::endl;
  }
}

void MySQLServer::storeRfidInfo(int rfid, int rfidtype, int rfidpos)
{
  try {

    std::string sqlStr = Poco::format("SELECT COUNT(*) FROM rfid WHERE robot_id = %d AND rfid = %d;",m_nRobotID,rfid);
    int nNum = 0;
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr,into(nNum), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::storeRfidInfo " << exc.displayText() << std::endl;
      }
    }
	ses.close();

    if(nNum == 0)
    {
      sqlStr = Poco::format("INSERT INTO rfid (robot_id,rfid,pos,type,find_time)  VALUES (%d,%d,%d,%d,now());",m_nRobotID,rfid,rfidpos,rfidtype);
      ses = m_pMySQLDriver->getSession();
      if(ses.isConnected())
      {
          try {
              Statement statement(ses);
              statement << sqlStr, now;
          } catch(Poco::Exception& exc) {
              std::cerr << "MySQLServer::storeRfidInfo " << exc.displayText() << std::endl;
          }
      }
	  ses.close();
    }
    else
    {
      sqlStr = Poco::format("UPDATE rfid SET pos = %d,type = %d,find_time = now() WHERE robot_id = %d AND rfid = %d;",rfidpos,rfidtype,m_nRobotID,rfid);
      ses = m_pMySQLDriver->getSession();
      if(ses.isConnected())
      {
          try {
              Statement statement(ses);
              statement << sqlStr, now;
          } catch(Poco::Exception& exc) {
            std::cerr << "MySQLServer::storeRfidInfo " << exc.displayText() << std::endl;
          }
      }
	  ses.close();
    }
	

  } catch(Poco::Exception& exc) {

    std::cerr << "MySQLServer::storeRfidInfo " << exc.displayText() << std::endl;
  }
}


void MySQLServer::getCharge(std::vector<TupleCharge> &vTupleCharge)
{
  try {

    string sqlStr = Poco::format("SELECT ID,ip,position,status FROM charge WHERE robot_id =%d;",m_nRobotID);
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr, into(vTupleCharge), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::getCharge " << exc.displayText() << std::endl;
      }
    }
	ses.close();

  } catch(Poco::Exception& exc) {

    std::cerr << "MySQLServer::getCharge " << exc.displayText() << std::endl;
  }
}

void MySQLServer::modifyCharge(string qIP, double dPos, string nStatus, int nID)
{
  try {

    string sqlStr = Poco::format("UPDATE charge SET IP = '%s', position = %f,status = '%s' WHERE ID = %d;",
                                 qIP, dPos,nStatus, nID);
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr, now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::modifyCharge " << exc.displayText() << std::endl;
      }
    }
	ses.close();

  } catch(Poco::Exception& exc) {

    std::cerr << "MySQLServer::modifyCharge " << exc.displayText() << std::endl;
  }
}

int MySQLServer::getFireDoorIp(int fireID, std::string &sfireIp)
{
  try {
      std::string sqlStr = Poco::format("SELECT ip FROM fire WHERE id=%d;", fireID);
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr, into(sfireIp), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::getFireDoorIp " << exc.displayText() << std::endl;
      }
	  ses.close();
	  return 0;
    }
	else
	{
		ses.close();
		return -1;
	}

  } catch(Poco::Exception& exc) {
    std::cerr << "MySQLServer::getFireDoorIp" << exc.displayText() << std::endl;
  }
  return -1;
}

int MySQLServer::queryPointEqusInfo(ViewPointInfo eViewPointInfo, EqusInfo &equsInfo)
{
  try {
    PicPathInfo vPicPathInfo;
    char pSQL[1024];
    snprintf(pSQL, sizeof(pSQL), "SELECT v.function, v.pic_url, s.camera_photo_path FROM view_point v JOIN task_detail_record s ON v.point_id=s.point_id Where v.point_id=%d AND s.exec_id=%d;",
            eViewPointInfo.nViewPointId, eViewPointInfo.nExecId);

    printf("pSQL %s\n",pSQL);
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Poco::Data::Statement statement(ses);
          statement << pSQL, into(vPicPathInfo), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::queryPointEqusInfo " << exc.displayText() << std::endl;
      }
    }
    ses.close();
    equsInfo.srefImgPath = vPicPathInfo.get<1>();
    equsInfo.sworkImgPath = vPicPathInfo.get<2>();
    equsInfo.nworkFlag = vPicPathInfo.get<0>();
    equsInfo.nworkId = eViewPointInfo.nViewPointId;
    equsInfo.meterInfo.clear();
    queryPointEquInfo(eViewPointInfo, equsInfo.meterInfo);
  }
  catch(Poco::Exception& exc) {
    std::cerr << "MySQLServer::queryPointEqusInfo " << exc.displayText() << std::endl;
    return -1;
  }

  return 0;
}

int MySQLServer::queryValueMap(int dataType, Poco::JSON::Object &valueMap) 
{
  try {
    std::vector<ValueMapInfo> vValueMapInfo;
    char pSQL[1024];
    snprintf(pSQL, sizeof(pSQL), "SELECT value, name FROM equip_struct_config_item v WHERE v.equip_struct_config_id=%d;",
            dataType);
    printf("pSQL %s\n",pSQL);
    Session ses = m_pMySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Poco::Data::Statement statement(ses);
          statement << pSQL, into(vValueMapInfo), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "MySQLServer::queryValueMap " << exc.displayText() << std::endl;
      }
    }
    ses.close();

    for (size_t i = 0; i < vValueMapInfo.size(); i++) {
        valueMap.set(std::to_string(vValueMapInfo[i].get<0>()), vValueMapInfo[i].get<1>());
    }
  }
  catch(Poco::Exception& exc) {
    std::cerr << "MySQLServer::queryValueMap " << exc.displayText() << std::endl;
    return -1;
  }

  return 0;
}

int MySQLServer::equsInfo2Json(EqusInfo equsInfo, Poco::JSON::Object &json)
{
  printf("%s() start!\n", __FUNCTION__);
  json.clear();
  json.set("transId", equsInfo.transId);
  json.set("refImgPath", equsInfo.srefImgPath.c_str());
  json.set("workImgPath", equsInfo.sworkImgPath.c_str());
  json.set("workFlag", equsInfo.nworkFlag);
  json.set("workId", equsInfo.nworkId);

  Poco::JSON::Array array;
  Poco::JSON::Object member;

  for(auto iter=equsInfo.meterInfo.begin(); iter!=equsInfo.meterInfo.end(); iter++) {
    member.set("nId", iter->nInnerId);
    member.set("udName", iter->sEquName.c_str());
    member.set("minRange", iter->sMinRange.c_str());
    member.set("maxRange", iter->sMaxRange.c_str());
    member.set("valueMap", Poco::Dynamic::Var(iter->valueMap));
    member.set("itemUnit",iter->sitemUnit.c_str());
    member.set("typeId",iter->ntypeId);
    member.set("bBox",iter->sbBox.c_str());

    array.add(member);
  } 
  json.set("meterInfo", array);

  Poco::Dynamic::Var Json(json);
  std::cout << Json.toString() << std::endl;

  return 0;
}
