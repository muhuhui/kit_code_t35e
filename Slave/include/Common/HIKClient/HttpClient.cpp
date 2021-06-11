#pragma warning(disable:4996)
#include"HttpClient.h"

//长连接对象
CURL *m_curl;
//数据解析缓冲区
CBuffer *pBuf;
//数据接收标志位
bool DataRev = false;
//长连接正常终止标志位，即撤防
bool Stop = false;

CURLcode httpGetBin(const char* Url, FILE* file, UserInfo *info)
{
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		printf("curl_easy_init error!\n");
		return CURLE_FAILED_INIT;
	}
	curl_easy_setopt(curl, CURLOPT_URL, Url);
	//设置接收标头数据的回调
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, head_data);
	//设置接收报文数据的回调
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_bin_data);
	//设置数据接收的实体
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
	//设置不使用任何安装信号处理程序，防止进程冲突
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	//设置连接等待的超时时间为5s
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	//设置数据传输的超时时间为5s
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	//设置连接跟踪重定向启用
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	//设置Http的验证方式，CURLAUTH_ANY表示由libcurl自动选择
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
	//设置用户名密码
	char *a = (char*)":";
	char userpwd[256] = "";
	strcat(userpwd, info->username);
	strcat(userpwd, a);
	strcat(userpwd, info->password);
	curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd);
	//进行连接
	CURLcode res = curl_easy_perform(curl);
	//连接结束，清理连接对象
	curl_easy_cleanup(curl);
	//返回连接结果
	return res;
}

CURLcode httpGet(const char* Url, char *DataBuf, UserInfo *info)
{
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		printf("curl_easy_init error!\n");
		return CURLE_FAILED_INIT;
	}
	curl_easy_setopt(curl, CURLOPT_URL, Url);
	//设置接收标头数据的回调
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, head_data);
	//设置接收报文数据的回调
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	//设置数据接收的实体
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, DataBuf);
	//设置不使用任何安装信号处理程序，防止进程冲突
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	//设置连接等待的超时时间为5s
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	//设置数据传输的超时时间为5s
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	//设置连接跟踪重定向启用
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	//设置Http的验证方式，CURLAUTH_ANY表示由libcurl自动选择
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
	//设置用户名密码
	char *a = (char*)":";
	char userpwd[256] = "";
	strcat(userpwd, info->username);
	strcat(userpwd, a);
	strcat(userpwd, info->password);
	curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd);
	//进行连接
	CURLcode res = curl_easy_perform(curl);
	//连接结束，清理连接对象
	curl_easy_cleanup(curl);
	//返回连接结果
	return res;
}

CURLcode httpPut(const char*  Url, const char* Put, char *DataBuf, UserInfo *info)
{
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		printf("curl_easy_init error!\n");
		return CURLE_FAILED_INIT;
	}
	curl_easy_setopt(curl, CURLOPT_URL, Url);
	//设置连接方式
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
	//设置请求报文
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, Put);
	//设置接收报文数据的回调
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	//设置数据接收的实体
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, DataBuf);
	//设置不使用任何安装信号处理程序，防止进程冲突
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	//设置请求连接超市时间
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	//设置连接超时时间
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	//设置连接跟踪重定向启用
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	//设置Http的验证方式，CURLAUTH_ANY表示由libcurl自动选择
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
	//组装用户名密码
	char *a = (char*)":";
	char userpwd[256] = "";
	strcat(userpwd, info->username);
	strcat(userpwd, a);
	strcat(userpwd, info->password);
	//设置用户名密码
	curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd);
	//进行连接
	CURLcode res = curl_easy_perform(curl);
	//连接结束，清理连接对象
	curl_easy_cleanup(curl);
	//返回连接结果
	return res;
}

CURLcode httpPost(const char* Url, const char* Post, char *DataBuf, UserInfo *info)
{
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		printf("curl_easy_init error!\n");
		return CURLE_FAILED_INIT;
	}
	curl_easy_setopt(curl, CURLOPT_URL, Url);
	//设置连接方式
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	//设置请求报文
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, Post);
	//设置接收报文数据的回调
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	//设置数据接收的实体
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, DataBuf);
	//设置不使用任何安装信号处理程序，防止进程冲突
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	//设置请求连接超市时间
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	//设置连接超时时间
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	//设置连接跟踪重定向启用
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	//设置Http的验证方式，CURLAUTH_ANY表示由libcurl自动选择
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
	//组装用户名密码
	char *a = (char*)":";
	char userpwd[256] = "";
	strcat(userpwd, info->username);
	strcat(userpwd, a);
	strcat(userpwd, info->password);
	//设置用户名密码
	curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd);
	//进行连接
	CURLcode res = curl_easy_perform(curl);
	//连接结束，清理连接对象
	curl_easy_cleanup(curl);
	//返回连接结果
	return res;
}

CURLcode httpDelete(const char* Url, char *DataBuf, UserInfo *info)
{
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		printf("curl_easy_init error!\n");
		return CURLE_FAILED_INIT;
	}
	curl_easy_setopt(curl, CURLOPT_URL, Url);
	//设置连接方式
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	//设置接收报文数据的回调
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	//设置数据接收的实体
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, DataBuf);
	//设置不使用任何安装信号处理程序，防止进程冲突
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	//设置请求连接超市时间
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	//设置连接超时时间
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	//设置连接跟踪重定向启用
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	//设置Http的验证方式，CURLAUTH_ANY表示由libcurl自动选择
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
	//设置用户名密码
	char *a = (char*)":";
	char userpwd[256] = "";
	strcat(userpwd, info->username);
	strcat(userpwd, a);
	strcat(userpwd, info->password);
	//设置用户名密码
	curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd);
	//进行连接
	CURLcode res = curl_easy_perform(curl);
	//连接结束，清理连接对象
	curl_easy_cleanup(curl);
	//返回连接结果
	return res;
}

CURLcode httpLongLink(const char* Url, char *DataBuf, UserInfo *info)
{
	pBuf = (CBuffer*)malloc(sizeof(CBuffer));
	cBuffer_Init(pBuf);
	m_curl = curl_easy_init();
	if (NULL == m_curl)
	{
		printf("curl_easy_init error!\n");
		return CURLE_FAILED_INIT;
	}
	//设置连接URL
	curl_easy_setopt(m_curl, CURLOPT_URL, Url);
	//设置接收报文数据的回调
	curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, recv_data);
	//设置单次数据接收的实体
	curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, DataBuf);
	//设置TCP长连接
	curl_easy_setopt(m_curl, CURLOPT_TCP_KEEPALIVE, 1L);
	//设置不使用任何安装信号处理程序，防止进程冲突
	curl_easy_setopt(m_curl, CURLOPT_NOSIGNAL, 1);
	//设置连接等待超时时间为10s
	curl_easy_setopt(m_curl, CURLOPT_CONNECTTIMEOUT, 10L);
	//设置数据传输超时时间为永不超时
	curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 0);
	//设置连接跟踪重定向启用
	curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1);
	//设置Http的验证方式，CURLAUTH_ANY表示由libcurl自动选择
	curl_easy_setopt(m_curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
	//填充用户密码
	char *a = (char*)":";
	char userpwd[256] = "";
	strcat(userpwd, info->username);
	strcat(userpwd, a);
	strcat(userpwd, info->password);
	//设置用户名密码
	curl_easy_setopt(m_curl, CURLOPT_USERPWD, userpwd);
	//进行连接
	CURLcode res = curl_easy_perform(m_curl);
	//判断是否由撤防操作导致连接中断
	if (Stop)
	{
		res = (CURLcode)STOPGUARD;
	}
	//连接结束，清理连接对象
	curl_easy_cleanup(m_curl);
	return res;
}

size_t head_data(void *buffer, size_t size, size_t nmemb, char *DataBuf)
{
	int realsize = size*nmemb;
	//char *pData = (char*)buffer;
	//char *pType = "";
	return realsize;
}

size_t write_data(void *buffer, size_t size, size_t nmemb, char *DataBuf)
{
	char *pData = (char*)buffer;
	strcat(DataBuf, pData);
	return nmemb;
}

size_t write_bin_data(void *ptr, size_t size, size_t nmemb, void *fp)
{
    int written; 

    char *pData = (char*)ptr;
    written = fwrite(pData, size, nmemb, (FILE *)fp);
//    printf("ptrlen: %d, size: %d, nmemb: %d, fp: %p\n", (int)strlen(pData), (int)size, (int)nmemb, fp);

	return written;
}


/*
报警报文接收回调函数
实现报警数据接收
输入参数：
buffer	回调接收的数据实体
size	单个缓冲区的数据长度
nmemb	缓冲区个数
DataBuf	指向接收完成时的报文实体的指针
*/
size_t recv_data(void *buffer, size_t size, size_t nmemb, char *DataBuf)
{
	// 数据接收标志,设置有数据接收
	DataRev = true;

	//建立临时缓冲区
	char *pData = (char*)buffer;

	//接收到的数据存入缓冲区
	for (int i = 0; i < size*nmemb; i++)
	{
		cBuffer_Add(pBuf, pData[i]);
	}
	//进行报文解析
	cBuffer_Parse(pBuf);
	return nmemb;
}


//终止布防长连接
bool StopLink()
{
	if (DataRev)
	{
		//数据标志位设为无数据接收
		DataRev = false;
		//撤防标志位
		Stop = true;
		//设置连接超时时间为1s，即连接在1s后关闭
		curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 1);
		//清理缓冲区
		cBuffer_Clear(pBuf);
		
	}
	else
	{
		return false;
	}
	return true;
}
