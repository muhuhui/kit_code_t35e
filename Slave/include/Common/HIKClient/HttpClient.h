#pragma once
//#pragma comment (lib, "libcurl.lib")
#include "curl/curl.h"
#include "ParseData.h"
#include <stdio.h>

#define STOPGUARD  10001

typedef struct
{
	//��¼�û���
	char *username;
	//��¼����
	char *password;
}UserInfo;

//HTTP GET����
CURLcode httpGet(const char* Url, char *DataBuf, UserInfo *info);
CURLcode httpGetBin(const char* Url, FILE *file, UserInfo *info);

//HTTP PUT����
CURLcode httpPut(const char*  Url, const char* strPut, char *DataBuf, UserInfo *info);

//HTTP POST����
CURLcode httpPost(const char* Url, const char* strPost, char *DataBuf, UserInfo *info);

//HTTP DELETE����
CURLcode httpDelete(const char* Url, char *DataBuf, UserInfo *info);

//HTTP ����������
CURLcode httpLongLink(const char* Url, char *DataBuf, UserInfo *info);

//��ֹ������
bool StopLink();

//����ͷ�����ջص�����
size_t head_data(void *ptr, size_t size, size_t nmemb, char *DataBuf);

//���������ݽ��ջص�����
size_t write_data(void *ptr, size_t size, size_t nmemb, char *DataBuf);
size_t write_bin_data(void *ptr, size_t size, size_t nmemb, void* fp);

//�������ݽ��ջص�����
size_t recv_data(void *buffer, size_t size, size_t nmemb, char *DataBuf);
