#include "FireDoorService.h"
#include <stdlib.h>


FireDoorService::FireDoorService()
{
	m_pMySQLDriver = MySQLDriver::getInstance();
}

FireDoorService::~FireDoorService()
{
}

//查询所有防火门
int FireDoorService::queryAll(std::vector<FireDoor> &vFireDoor)
{
	  std::vector<TupleFire> vTupleFire;
	  try
	  {
		  std::string sqlStr = "SELECT id,ip,status,position,name FROM fire";
		  Session ses = m_pMySQLDriver->getSession();
		  if(ses.isConnected())
		  {
            try {
                Statement statement(ses);
                statement << sqlStr, into(vTupleFire), now;
            } catch(Poco::Exception& exc) {
              std::cerr << "FireDoorService::queryAll " << exc.displayText() << std::endl;
            }
			
			for(int i = 0; i < vTupleFire.size(); ++i)
			  {
				  FireDoor fireDoor;
				  fireDoor.setId(vTupleFire[i].get<0>());
				  fireDoor.setIp(vTupleFire[i].get<1>());
				  fireDoor.setStatus(vTupleFire[i].get<2>());
				  fireDoor.setPosition(vTupleFire[i].get<3>()); 			  
				  fireDoor.setName(vTupleFire[i].get<4>());   
				  vFireDoor.push_back(fireDoor);
			  }
		  }
		  ses.close();
	
	} 
	catch(Poco::Exception& exc) 
	{
	
	  std::cerr << "FireDoorService::queryAll " << exc.displayText() << std::endl;
	}
	
	return 0;

}


