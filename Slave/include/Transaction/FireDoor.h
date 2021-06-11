#pragma once

#include <string>
#include "Driver/FireDoorDriver.h"

/*enum FireDoorStatus {
  FIRE_DOOR_CLOSED = 0,
  FIRE_DOOR_OPEN ,
  FIRE_DOOR_ERR, 
  FIRE_DOOR_LOST,
};*/

class FireDoor
{
	private:
		int 			id;
		std::string 	ip;
		int 			status;
		int 			position;
		std::string 	name;

	public:
		FireDoor();
		~FireDoor();

		void setId(int id);
		int getId();

		void setIp(std::string ip);
		std::string getIp();
		
		void setStatus(int     status);
		int  getStatus();	

		void setPosition(int position);
		int getPosition();

		void setName(std::string name);
		std::string getName();

};

