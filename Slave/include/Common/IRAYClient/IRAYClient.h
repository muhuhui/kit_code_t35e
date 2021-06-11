#ifndef __IRAYCLIENT_H__
#define __IRAYCLIENT_H__

#include "LinuxDef.h"
#include "IRNet.h"
#include <mutex>
#include <vector>

class IRAYClient
{
private:
	static IRAYClient* m_pInstance;
	IRNETHANDLE m_hchann = IRNETHANDLE(-1);
	char m_ip[32];
	char m_port[16];
	char m_username[128];
	char m_password[128];
	void* m_temp_raw_data_ptr;
	int m_temp_raw_data_size;
	int m_temp_raw_head_size;
	float m_min_temp;
	float m_max_temp;
	std::mutex m_lck;
	CHANNEL_CLIENTINFO m_client_info;
	enum {
		STREAM_NO_IMAGE = 0, //image
		STREAM_NO_NON,		 //unkown
		STREAM_NO_TEMP		 //temperature
	};
	void temp_extract(std::vector<float> corners);

public:

	static IRAYClient* getInstance();
	int client_init(const char* ip, const char* port, const char* usr, const char* pwd);
	int client_set_opt();
	int client_get_temp_raw_dat();//get raw data of a temperature frame
	int dump_raw_dat();
	int dump_centigrade_dat();
	IRNETHANDLE client_open();
	int client_start_nomal_cap();
	int client_close();
	void* client_set_dat_ptr(char* addr);
	int client_set_dat_siz(int size);
	int client_set_head_size(int size);
	int write_into_file(const char* file_name);
	int get_local_temp(std::vector<float> corners);
	float getMaxTemp(){return m_max_temp;}
	float getMinTemp(){return m_min_temp;}
};

#endif
