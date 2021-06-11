#ifndef __HIKCLIENT_H__
#define __HIKCLIENT_H__

#include "HttpClient.h"

#define CONFIG 1
#define GET 1		//GET
#define PUT 2		//PUT
#define POST 3		//POST
#define DELETE 4	//DELETE
#define BUFFERSIZE   1024*2048


class HIKClient
{
public:
    static HIKClient* getInstance();
    void init(const char *ip, const char *port, const char *username, const char *password);
    int getTempData(char *pData, int size);
    int getTempFile(const char* pFilePath);
    int getStatusData(char *pData, int size);
    int getStatus(int* pData);
    int getMinMaxTempData(char *pData, int size);
    int getMinMaxTemp();
    int pixelParamConfig();
    int config(const char *ip, const char *port, const char *username, const char *password, const char *url, int method);
    float getMinTemp();
    float getMaxTemp();
private:
    static HIKClient* m_pInstance;
    char m_ip[32];
    char m_port[16];
    char m_username[128];
    char m_password[128];
    float m_mintemp;
    float m_maxtemp;
};

#endif
