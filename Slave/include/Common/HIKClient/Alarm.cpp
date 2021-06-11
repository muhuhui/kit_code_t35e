#include "Alarm.h"

//ֹͣ������ʶ
bool StopListen = false;
//�������ݽ��ձ�ʶ
bool ListenData = false;
/*
��������
ʵ��ISAPI��������ģ�飬ʹ��HTTPͨ�ţ�������ģʽ
���̣�����http����client��->������ʱ��������->��������������ǰ������->����������ѯ�߳�->��������������->������������ͨ�Ž��
��ʱ�������ƣ�������ֹ->�ж��Ƿ�Ϊ��ʱ��ֹ��TimeOut==true)->��ʱ��ֹ�������������ٽ�������->�����������������Σ���ֹ�������������ӳ�ʱ
�������
ip���豸IP
port:ͨ�Ŷ˿�
*/
void alarm_guard(const char *ip, const char *port, char *username, char *password)
{
	//������������
	//��װ������������url
	char URL[256] = "http://";
	char url[128] = "/ISAPI/Event/notification/alertStream";
	strcat(URL, ip);
	strcat(URL, ":");
	strcat(URL, port);
	strcat(URL, url);

	//���α����ϴ����ݻ���
	char *DataResponse = new char[BUFFERSIZE];
	memset(DataResponse, 0, BUFFERSIZE);

	//�û���Ϣ
	UserInfo info = {};
	info.username = username;
	info.password = password;

	//������ѯ�߳�
	pthread_t id;
	int ret = pthread_create(&id, NULL, stop_guard, NULL);

	//��������������
	CURLcode res = httpLongLink(URL, DataResponse, &info);

	//������������ͨ�Ž��
	if (res == STOPGUARD)
	{
		printf("Stop_Guard Success\n");
	}
	else 
	{
		printf("curl_easy_perform failed:%s\n", curl_easy_strerror(res));
	}
}

//������ѯ�߳�
void *stop_guard(void*)
{
	while (1)
	{
		//���û�����س��������ݽ��գ����г�������
		if (getchar() == '\n'&&StopLink())
		{
			break;
		}
	}
	return NULL;
}

void alarm_listen(const char *ip, const int port)
{

	int listenfd, connfd;

	//����һ��SOCKADDR_IN���ͱ���sin����ʾ�������˵��׽���������
	struct sockaddr_in sockaddr;
	//�������շ������˵Ļ�����
	char revData[BUFFERSIZE];
	memset(revData, 0, BUFFERSIZE);

	memset(&sockaddr, 0, sizeof(sockaddr));

	//��ַ�帳ֵ��AF_INET��ʾIPv4
	sockaddr.sin_family = AF_INET;
	//ip��ַ��ֵ��INADDR_ANY��ʾ���뱾��ip��ַ
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//�˿ںŸ�ֵ����Web���õĶ˿ں�һ��
	sockaddr.sin_port = htons(port);
	
	//��IP�Ͷ˿� 
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	bind(listenfd, (struct sockaddr *) &sockaddr, sizeof(sockaddr));

	//����ֹͣ�����߳�
	pthread_t id;
	int ret = pthread_create(&id, NULL, stop_listen, NULL);

	//��ʼ����  
	listen(listenfd, 1024);

	//��������������
	CBuffer *pBuf;
	pBuf = (CBuffer*)malloc(sizeof(CBuffer));
	cBuffer_Init(pBuf);

	//ѭ��������������  
	while (!StopListen)
	{
		//��ȡ��������������׽���������
		if ((connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1)
		{
			printf("accpet socket error: %s errno :%d\n", strerror(errno), errno);
			continue;
		}
		//ѭ����������
		while (true)
		{
			//��������
			int ret = recv(connfd, revData, BUFFERSIZE, 0);
			if (ret > 0)
			{
				ListenData = true;
				//�������ݲ�Ϊ��
				revData[ret] = 0x00;
				//�����յ��������뻺���������ڽ���
				for (int i = 0; i < ret; i++)
				{
					cBuffer_Add(pBuf, revData[i]);
				}
			}
			else 
			{
				//�ж��Ƿ������ݽ������
				if (pBuf->size > 0)
				{
					//�������ݽ���
					cBuffer_Parse(pBuf);
				}
				break;
			}
		}		
	}
}

/*
ֹͣ������ѯ����
ʵ��ֹͣ�������ܣ��û�����س���ʾֹͣ����������
*/
void *stop_listen(void*)
{
	while (1)
	{
		//���û�����س��������ݽ��գ�����ֹͣ��������
		if (getchar() == '\n'&&ListenData)
		{
			StopListen = true;
			break;
		}
	}
	return NULL;
}
