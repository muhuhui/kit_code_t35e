#pragma once
//#pragma comment (lib, "libcurl.lib")
#include "curl/curl.h"
#include "ParseData.h"
#include <stdio.h>

#define STOPGUARD  10001

typedef struct
{
	//登录用户名
	char *username;
	//登录密码
	char *password;
}UserInfo;

//HTTP GET请求
CURLcode httpGet(const char* Url, char *DataBuf, UserInfo *info);
CURLcode httpGetBin(const char* Url, FILE *file, UserInfo *info);

//HTTP PUT请求
CURLcode httpPut(const char*  Url, const char* strPut, char *DataBuf, UserInfo *info);

//HTTP POST请求
CURLcode httpPost(const char* Url, const char* strPost, char *DataBuf, UserInfo *info);

//HTTP DELETE请求
CURLcode httpDelete(const char* Url, char *DataBuf, UserInfo *info);

//HTTP 长连接请求
CURLcode httpLongLink(const char* Url, char *DataBuf, UserInfo *info);

//终止长连接
bool StopLink();

//报文头部接收回调函数
size_t head_data(void *ptr, size_t size, size_t nmemb, char *DataBuf);

//配置类数据接收回调函数
size_t write_data(void *ptr, size_t size, size_t nmemb, char *DataBuf);
size_t write_bin_data(void *ptr, size_t size, size_t nmemb, void* fp);

//报警数据接收回调函数
size_t recv_data(void *buffer, size_t size, size_t nmemb, char *DataBuf);
