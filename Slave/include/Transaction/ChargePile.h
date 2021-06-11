#pragma once

#include <string>



class ChargePile
{
	private:
		int 			id;
		std::string 	ip;
		int 			status;
		int 			position;
		std::string 	name;
		std::string 	instance_name;
        int             mapId;

	public:
		ChargePile();
		~ChargePile();

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

		void setInstanceName(std::string name);
		std::string getInstanceName();

        void setMapId(int mapId);
        int getMapId();
};

