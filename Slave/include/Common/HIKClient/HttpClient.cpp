#pragma warning(disable:4996)
#include"HttpClient.h"

//�����Ӷ���
CURL *m_curl;
//���ݽ���������
CBuffer *pBuf;
//���ݽ��ձ�־λ
bool DataRev = false;
//������������ֹ��־λ��������
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
	//���ý��ձ�ͷ���ݵĻص�
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, head_data);
	//���ý��ձ������ݵĻص�
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_bin_data);
	//�������ݽ��յ�ʵ��
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
	//���ò�ʹ���κΰ�װ�źŴ�����򣬷�ֹ���̳�ͻ
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	//�������ӵȴ��ĳ�ʱʱ��Ϊ5s
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	//�������ݴ���ĳ�ʱʱ��Ϊ5s
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	//�������Ӹ����ض�������
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	//����Http����֤��ʽ��CURLAUTH_ANY��ʾ��libcurl�Զ�ѡ��
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
	//�����û�������
	char *a = (char*)":";
	char userpwd[256] = "";
	strcat(userpwd, info->username);
	strcat(userpwd, a);
	strcat(userpwd, info->password);
	curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd);
	//��������
	CURLcode res = curl_easy_perform(curl);
	//���ӽ������������Ӷ���
	curl_easy_cleanup(curl);
	//�������ӽ��
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
	//���ý��ձ�ͷ���ݵĻص�
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, head_data);
	//���ý��ձ������ݵĻص�
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	//�������ݽ��յ�ʵ��
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, DataBuf);
	//���ò�ʹ���κΰ�װ�źŴ�����򣬷�ֹ���̳�ͻ
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	//�������ӵȴ��ĳ�ʱʱ��Ϊ5s
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	//�������ݴ���ĳ�ʱʱ��Ϊ5s
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	//�������Ӹ����ض�������
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	//����Http����֤��ʽ��CURLAUTH_ANY��ʾ��libcurl�Զ�ѡ��
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
	//�����û�������
	char *a = (char*)":";
	char userpwd[256] = "";
	strcat(userpwd, info->username);
	strcat(userpwd, a);
	strcat(userpwd, info->password);
	curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd);
	//��������
	CURLcode res = curl_easy_perform(curl);
	//���ӽ������������Ӷ���
	curl_easy_cleanup(curl);
	//�������ӽ��
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
	//�������ӷ�ʽ
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
	//����������
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, Put);
	//���ý��ձ������ݵĻص�
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	//�������ݽ��յ�ʵ��
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, DataBuf);
	//���ò�ʹ���κΰ�װ�źŴ�����򣬷�ֹ���̳�ͻ
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	//�����������ӳ���ʱ��
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	//�������ӳ�ʱʱ��
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	//�������Ӹ����ض�������
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	//����Http����֤��ʽ��CURLAUTH_ANY��ʾ��libcurl�Զ�ѡ��
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
	//��װ�û�������
	char *a = (char*)":";
	char userpwd[256] = "";
	strcat(userpwd, info->username);
	strcat(userpwd, a);
	strcat(userpwd, info->password);
	//�����û�������
	curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd);
	//��������
	CURLcode res = curl_easy_perform(curl);
	//���ӽ������������Ӷ���
	curl_easy_cleanup(curl);
	//�������ӽ��
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
	//�������ӷ�ʽ
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	//����������
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, Post);
	//���ý��ձ������ݵĻص�
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	//�������ݽ��յ�ʵ��
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, DataBuf);
	//���ò�ʹ���κΰ�װ�źŴ�����򣬷�ֹ���̳�ͻ
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	//�����������ӳ���ʱ��
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	//�������ӳ�ʱʱ��
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	//�������Ӹ����ض�������
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	//����Http����֤��ʽ��CURLAUTH_ANY��ʾ��libcurl�Զ�ѡ��
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
	//��װ�û�������
	char *a = (char*)":";
	char userpwd[256] = "";
	strcat(userpwd, info->username);
	strcat(userpwd, a);
	strcat(userpwd, info->password);
	//�����û�������
	curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd);
	//��������
	CURLcode res = curl_easy_perform(curl);
	//���ӽ������������Ӷ���
	curl_easy_cleanup(curl);
	//�������ӽ��
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
	//�������ӷ�ʽ
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	//���ý��ձ������ݵĻص�
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	//�������ݽ��յ�ʵ��
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, DataBuf);
	//���ò�ʹ���κΰ�װ�źŴ�����򣬷�ֹ���̳�ͻ
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	//�����������ӳ���ʱ��
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	//�������ӳ�ʱʱ��
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	//�������Ӹ����ض�������
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	//����Http����֤��ʽ��CURLAUTH_ANY��ʾ��libcurl�Զ�ѡ��
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
	//�����û�������
	char *a = (char*)":";
	char userpwd[256] = "";
	strcat(userpwd, info->username);
	strcat(userpwd, a);
	strcat(userpwd, info->password);
	//�����û�������
	curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd);
	//��������
	CURLcode res = curl_easy_perform(curl);
	//���ӽ������������Ӷ���
	curl_easy_cleanup(curl);
	//�������ӽ��
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
	//��������URL
	curl_easy_setopt(m_curl, CURLOPT_URL, Url);
	//���ý��ձ������ݵĻص�
	curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, recv_data);
	//���õ������ݽ��յ�ʵ��
	curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, DataBuf);
	//����TCP������
	curl_easy_setopt(m_curl, CURLOPT_TCP_KEEPALIVE, 1L);
	//���ò�ʹ���κΰ�װ�źŴ�����򣬷�ֹ���̳�ͻ
	curl_easy_setopt(m_curl, CURLOPT_NOSIGNAL, 1);
	//�������ӵȴ���ʱʱ��Ϊ10s
	curl_easy_setopt(m_curl, CURLOPT_CONNECTTIMEOUT, 10L);
	//�������ݴ��䳬ʱʱ��Ϊ������ʱ
	curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 0);
	//�������Ӹ����ض�������
	curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1);
	//����Http����֤��ʽ��CURLAUTH_ANY��ʾ��libcurl�Զ�ѡ��
	curl_easy_setopt(m_curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
	//����û�����
	char *a = (char*)":";
	char userpwd[256] = "";
	strcat(userpwd, info->username);
	strcat(userpwd, a);
	strcat(userpwd, info->password);
	//�����û�������
	curl_easy_setopt(m_curl, CURLOPT_USERPWD, userpwd);
	//��������
	CURLcode res = curl_easy_perform(m_curl);
	//�ж��Ƿ��ɳ����������������ж�
	if (Stop)
	{
		res = (CURLcode)STOPGUARD;
	}
	//���ӽ������������Ӷ���
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
�������Ľ��ջص�����
ʵ�ֱ������ݽ���
���������
buffer	�ص����յ�����ʵ��
size	���������������ݳ���
nmemb	����������
DataBuf	ָ��������ʱ�ı���ʵ���ָ��
*/
size_t recv_data(void *buffer, size_t size, size_t nmemb, char *DataBuf)
{
	// ���ݽ��ձ�־,���������ݽ���
	DataRev = true;

	//������ʱ������
	char *pData = (char*)buffer;

	//���յ������ݴ��뻺����
	for (int i = 0; i < size*nmemb; i++)
	{
		cBuffer_Add(pBuf, pData[i]);
	}
	//���б��Ľ���
	cBuffer_Parse(pBuf);
	return nmemb;
}


//��ֹ����������
bool StopLink()
{
	if (DataRev)
	{
		//���ݱ�־λ��Ϊ�����ݽ���
		DataRev = false;
		//������־λ
		Stop = true;
		//�������ӳ�ʱʱ��Ϊ1s����������1s��ر�
		curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 1);
		//��������
		cBuffer_Clear(pBuf);
		
	}
	else
	{
		return false;
	}
	return true;
}
