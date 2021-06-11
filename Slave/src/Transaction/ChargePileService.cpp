#include "ChargePileService.h"
#include <stdlib.h>


ChargePileService::ChargePileService()
{
	
}

ChargePileService::~ChargePileService()
{
}

ChargePileService *ChargePileService::getInstance()
{
	return SingleTon<ChargePileService>::GetInstance();
}

void ChargePileService::init()
{
	m_pMySQLDriver = MySQLDriver::getInstance();
}

//��ѯ
int ChargePileService::queryAll(std::vector<ChargePile> &vChargePile)
{
	std::vector<TupleChargePile> vTupleChargePile;
	try
	{
	    std::string sqlStr = "SELECT id,ip,status,position,name,instance_name,map_id FROM charge";
	    Session ses = m_pMySQLDriver->getSession();
	    if(ses.isConnected())
	    {
            try {
                Statement statement(ses);
                statement << sqlStr, into(vTupleChargePile), now;
            } catch(Poco::Exception& exc) {
                std::cerr << "ChargePileService::queryAll " << exc.displayText() << std::endl;
            }
            
            for(int i = 0; i < vTupleChargePile.size(); ++i)
	        {
		    	ChargePile chargePile;
	            chargePile.setId(vTupleChargePile[i].get<0>());
	            chargePile.setIp(vTupleChargePile[i].get<1>());
	            chargePile.setStatus(vTupleChargePile[i].get<2>());
	            chargePile.setPosition(vTupleChargePile[i].get<3>());	            
	            chargePile.setName(vTupleChargePile[i].get<4>());
				chargePile.setInstanceName(vTupleChargePile[i].get<5>());	
                chargePile.setMapId(vTupleChargePile[i].get<6>());
	            vChargePile.push_back(chargePile);
	        }
	    }
		ses.close();
    } 
    catch(Poco::Exception& exc) 
    {
        std::cerr << "ChargePileService::queryAll " << exc.displayText() << std::endl;
    }
    
    return 0;
}

int ChargePileService::queryByMapId(int mapId, std::vector<ChargePile> &vChargePile)
{
    int nRet = 0;
	std::vector<TupleChargePile> vTupleChargePile;
	try
	{
	    char sqlStr[1024];
        snprintf(sqlStr, sizeof(sqlStr), "SELECT id,ip,status,position,name,instance_name,map_id FROM charge WHERE map_id = %d", mapId);
	    Session ses = m_pMySQLDriver->getSession();
	    if(ses.isConnected())
	    {
            try {
                Statement statement(ses);
                statement << sqlStr, into(vTupleChargePile), now;
            } catch(Poco::Exception& exc) {
                std::cerr << "ChargePileService::queryByMapId " << exc.displayText() << std::endl;
            }
		  
            for(int i = 0; i < vTupleChargePile.size(); ++i)
	        {
		    	ChargePile chargePile;
	            chargePile.setId(vTupleChargePile[i].get<0>());
	            chargePile.setIp(vTupleChargePile[i].get<1>());
	            chargePile.setStatus(vTupleChargePile[i].get<2>());
	            chargePile.setPosition(vTupleChargePile[i].get<3>());	            
	            chargePile.setName(vTupleChargePile[i].get<4>());
				chargePile.setInstanceName(vTupleChargePile[i].get<5>());	
                chargePile.setMapId(vTupleChargePile[i].get<6>());
	            vChargePile.push_back(chargePile);
	        }
	    }
		ses.close();
        nRet = 0;
    } 
    catch(Poco::Exception& exc) 
    {
        std::cerr << "ChargePileService::queryAll " << exc.displayText() << std::endl;
        nRet = -1;
    }
  
    return nRet;
}
