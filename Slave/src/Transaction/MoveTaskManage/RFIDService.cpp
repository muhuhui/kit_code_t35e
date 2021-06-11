//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : T3C_SLAVE
//  @ File Name : RFIDService.cpp
//  @ Date : 2019-07-03
//  @ Author : ShunWang
//
//


#include "Transaction/MoveTaskManage/RFIDService.h"
#include "RFID.h"
#include <list>
#include <vector>

void setRfidfromDb(RFID& rfid, TupleRFID item)
{
  rfid.setId(item.get<0>());
  rfid.setRfidNum(item.get<1>());
  rfid.setPosition(item.get<2>());
}
/*
int  RFIDService::queryRFIDById(int id,RFID& rfid)
{
  try
  {
    std::vector<TupleRFID> tupleRFID;
    MySQLDriver* mySQLDriver = MySQLDriver::getInstance();
    std::string sqlStr = Poco::format("SELECT * FROM rfid WHERE id = %d;",id);
    Session ses = mySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr, into(tupleRFID), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "RFIDService::queryRFIDById " << exc.displayText() << std::endl;
      }
    }
    ses.close();
    if(tupleRFID.size() > 0)
    {
        for (size_t i = 0; i < tupleRFID.size(); i++) {
            setRfidfromDb(rfid, tupleRFID[i]);
        }
    }
    else
    {
      return -1;
    }
  }
  catch(Poco::Exception& exc)
  {
    std::cerr << "MySQLServer::getFireByID " << exc.displayText() << std::endl;
  }
  return 0;
}

int  RFIDService::queryRFIDByRfidNum(long long rfidNum,RFID& rfid)
{
  try
  {
    std::list<TupleRFID> tupleRFID;
    MySQLDriver* mySQLDriver = MySQLDriver::getInstance();
    std::string sqlStr = Poco::format("SELECT * FROM rfid WHERE rfid_num = %Ld;",rfidNum);
    printf("send sql command: %s\n",sqlStr.c_str());
    Session ses = mySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr, into(tupleRFID), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "RFIDService::queryRFIDByRfidNum " << exc.displayText() << std::endl;
      }
      if(tupleRFID.size() > 0)
      {
        std::list<TupleRFID>::iterator iter;
        for(iter = tupleRFID.begin();iter != tupleRFID.end();iter++)
        {
          setRfidfromDb(rfid, *iter);
        }
        return 0;
      }
      else
      {
        return -1;
      }
    }
    ses.close();
  } catch(Poco::Exception& exc) {
    std::cerr << "RFIDService::queryRFIDByRfidNum " << exc.displayText() << std::endl;
  }
}

int RFIDService::queryRFIDListByRobotId(int robotId,std::list<RFID>& rfids)
{
  try
  {
    std::list<TupleRFID> tupleRFID;
    MySQLDriver* mySQLDriver = MySQLDriver::getInstance();
    std::string sqlStr = "SELECT * FROM rfid WHERE rfid_num>=0;";
    Session ses = mySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr, into(tupleRFID), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "RFIDService::queryRFIDListByRobotId " << exc.displayText() << std::endl;
      }
      std::list<TupleRFID>::iterator iter;
      for(iter = tupleRFID.begin();iter != tupleRFID.end();iter++)
      {
        RFID rfid;
        setRfidfromDb(rfid, *iter);
        rfids.push_back(rfid);
      }
    }
    ses.close();
  } catch(Poco::Exception& exc) {

    std::cerr << "RFIDService::queryRFIDListByRobotId " << exc.displayText() << std::endl;
  }
  return 0;
}
*/

int RFIDService::queryRFIDListByMapId(uint32_t mapId, std::list<RFID>& rfids)
{
  try
  {
    std::list<TupleRFID> tupleRFID;
    MySQLDriver* mySQLDriver = MySQLDriver::getInstance();
    std::string sqlStr = Poco::format("SELECT * FROM rfid WHERE map_id= %u;", mapId );
    Session ses = mySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr, into(tupleRFID), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "RFIDService::queryRFIDListByMapId " << exc.displayText() << std::endl;
      }
      for (auto iter : tupleRFID ) {
        RFID rfid;
        setRfidfromDb(rfid, iter);
        rfids.push_back(rfid);
      }
    }
    ses.close();
  } catch(Poco::Exception& exc) {
    std::cerr << "RFIDService::queryRFIDListByMapId " << exc.displayText() << std::endl;
    return 0;
  }
  return rfids.size();
}

void RFIDService::insertSelective(RFID rfid)
{
  try
  {
    MySQLDriver* mySQLDriver = MySQLDriver::getInstance();
    std::string sqlStr = Poco::format("INSERT INTO rfid (rfid_num,position)  VALUES (%Ld,%d);",
                                      rfid.getRfidNum(), rfid.getPosition());

    printf("++++++ %s()@%d,send sql command: %s +++++++\n", __FUNCTION__, __LINE__, sqlStr.c_str());
    Session ses = mySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr, now;
      } catch(Poco::Exception& exc) {
        std::cerr << "RFIDService::insertSelective " << exc.displayText() << std::endl;
      }
    }
    ses.close();
  }
  catch(Poco::Exception& exc)
  {
    std::cerr << "RFIDService::insertSelective " << exc.displayText() << std::endl;
  }
}

/*
void RFIDService::updateRFID(RFID rfid)
{
  MySQLDriver* mySQLDriver = MySQLDriver::getInstance();
  std::string sqlStr = Poco::format("UPDATE rfid SET rfid_num= %Ld,position=%d;",
                                      rfid.getRfidNum(),rfid.getPosition());
  Session ses = mySQLDriver->getSession();
  if(ses.isConnected())
  {
    try {
        Statement statement(ses);
        statement << sqlStr, now;
    } catch(Poco::Exception& exc) {
      std::cerr << "RFIDService::updateRFID " << exc.displayText() << std::endl;
    }
  }
  ses.close();

}
*/

bool RFIDService::updateRFIDbyMapId(RFID rfid, uint32_t mapId)
{
  try
  {
    MySQLDriver* mySQLDriver = MySQLDriver::getInstance();
    std::string sqlStr = Poco::format("UPDATE rfid SET map_id= %u WHERE rfid_num= %Ld AND position=%d;",
                                      mapId, rfid.getRfidNum(),rfid.getPosition());
    Session ses = mySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr, now;
      } catch(Poco::Exception& exc) {
        std::cerr << "RFIDService::updateRFIDbyMapId " << exc.displayText() << std::endl;
      }
    }
    ses.close();
    return true;
  }
  catch(Poco::Exception & e)
  {
    std::cerr << "RFIDService::updateRFIDbyMapId " << e.displayText() << std::endl;
    return false;
  }  
}

void RFIDService::updateRFIDPos(int nRobotID, long long RFIDNum, int nPos)
{
  MySQLDriver* mySQLDriver = MySQLDriver::getInstance();
  std::string sqlStr = Poco::format("UPDATE rfid SET position=%d WHERE rfid_num= %Ld;", nPos, RFIDNum);
  printf("send sql command: %s\n",sqlStr.c_str());
  Session ses = mySQLDriver->getSession();
  if(ses.isConnected())
  {
    try {
        Statement statement(ses);
        statement << sqlStr, now;
    } catch(Poco::Exception& exc) {
      std::cerr << "RFIDService::updateRFIDPos " << exc.displayText() << std::endl;
    }
  }
  ses.close();
}

void RFIDService::deleteRFIDById(int id)
{
  MySQLDriver* mySQLDriver = MySQLDriver::getInstance();
  std::string sqlStr = Poco::format("DELETE FROM rfid WHERE id= %d;",id);
  Session ses = mySQLDriver->getSession();
  if(ses.isConnected())
  {
    try {
        Statement statement(ses);
        statement << sqlStr, now;
    } catch(Poco::Exception& exc) {
      std::cerr << "RFIDService::deleteRFIDById " << exc.displayText() << std::endl;
    }
  }
  ses.close();
}

void RFIDService::deleteRFIDByRobotId(int robotId)
{
  MySQLDriver* mySQLDriver = MySQLDriver::getInstance();
  std::string sqlStr = Poco::format("DELETE FROM rfid WHERE robot_id= %d;",robotId);
  Session ses = mySQLDriver->getSession();
  if(ses.isConnected())
  {
    try {
        Statement statement(ses);
        statement << sqlStr, now;
    } catch(Poco::Exception& exc) {
      std::cerr << "RFIDService::deleteRFIDByRobotId " << exc.displayText() << std::endl;
    }
  }
  ses.close();
}

bool RFIDService::deleteRFIDByMapId(uint32_t mapId)
{
  try
  {
    MySQLDriver* mySQLDriver = MySQLDriver::getInstance();
    std::string sqlStr = Poco::format("DELETE FROM rfid WHERE map_id= %u;",mapId);
    Session ses = mySQLDriver->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr, now;
      } catch(Poco::Exception& exc) {
        std::cerr << "RFIDService::deleteRFIDByMapId " << exc.displayText() << std::endl;
      }
    }
    ses.close();
  }
  catch(Poco::Exception & e)
  {
    std::cerr << "RFIDService::deleteRFIDByMapId " << e.displayText() << std::endl;
    return false;
  }  
  return true;
}

void RFIDService::deleteRFID()
{
  MySQLDriver* mySQLDriver = MySQLDriver::getInstance();
  std::string sqlStr = "DELETE FROM rfid";
  Session ses = mySQLDriver->getSession();
  if(ses.isConnected())
  {
    try {
        Statement statement(ses);
        statement << sqlStr, now;
    } catch(Poco::Exception& exc) {
      std::cerr << "RFIDService::deleteRFID " << exc.displayText() << std::endl;
    }
  }
  ses.close();
}


