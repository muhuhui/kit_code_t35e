#include"JsonParse.h"

int picnumber;
char *datetime;
char *date = "";

void ParseToJson(const char *pData, int length)
{
	//����json���󣬲���������Ϊjson��ʽ
	cJSON *json = cJSON_Parse(pData);
	//�������Ľ���
	if (json->child != NULL&&strcmp(json->child->string, "ipAddress") == 0)
	{
		//��������JSON���Ľ�������
		cJSON *JAlarm = json->child;
		//�Ա��Ľ���ѭ�����ҽڵ㣬��Խڵ����
		while (JAlarm != NULL)
		{
			//���ұ���ʱ��ڵ�
			if (strcmp(JAlarm->string, "dateTime") == 0)
			{
				//��Ա������ֵ
				
				datetime = JAlarm->valuestring;
				//ɾ���ļ��������ܡ������ַ�
				int len = sizeof(datetime);
				for (int i = 0; i < sizeof(datetime); i++)
				{
					if (datetime[i] == ':')
					{
						len--;
						for (int j = i; j < sizeof(datetime)-1; j++)
							datetime[j] = datetime[j + 1];
					}
				}
				date = (char*)malloc(sizeof(char)*len);
				for (int i = 0; i < len; i++)
				{
					date[i] = datetime[i];
				}
			}
			//�Է��������Ľ��б���
			if (strcmp(JAlarm->string, "eventState") == 0)
			{
				if (strcmp(JAlarm->valuestring, "active") == 0)
				{
					//�����ļ���
					char chFilename[1024];
					//stringתchar*
					

					//�������ݱ���
					//�����ļ���
					sprintf(chFilename, "./alarm_file/%s.txt", date);
					//�����ļ�����
					FILE *file;
					//��д��ʽ���ļ�
					if ((file = fopen(chFilename, "w+")) != NULL)
					{
						//����д���ļ�
						fwrite(pData, sizeof(char), length, file);
						//�ر��ļ�
						fclose(file);
					}
					else
					{
						//�ļ���ʧ��
						printf("Create file failed!\n");
					}
				}
			}
			//��ȡ�����ϴ�������ץ��ͼƬ
			if (strcmp(JAlarm->string, "captureLibResult") == 0)
			{
				//����JSON��������
				cJSON *captureLib = JAlarm;
				//��ȡjson�����С
				int count = cJSON_GetArraySize(captureLib);
				//��json�������
				for (int i = 0; i < count; i++)
				{
					//��ȡJSON����Ԫ��
					cJSON *Info = cJSON_GetArrayItem(captureLib, i);
					//��ȡJSON�����ʵ�ʽڵ�
					Info = Info->child;
					//����ͼƬ����
					while (Info != NULL)
					{
						if (strcmp(Info->string, "image") == 0)
						{
							//ͼƬ��������
							picnumber++;
							//����URL����ͼƬ
							URLDownload(Info->valuestring);
						}
						if (strcmp(Info->string, "targetAttrs") == 0)
						{
							//�����ӽڵ�
							cJSON *target = Info->child;
							while (target != NULL)
							{
								//�ж��Ƿ��б���ͼƬ
								if (strcmp(target->string, "bkgUrl") == 0)
								{
									//ͼƬ��������
									picnumber++;
									//����URL����ͼƬ
									URLDownload(target->valuestring);
								}
								//�ڵ������ƶ�
								target = target->next;
							}
						}
						//��������ͼƬ
						if (strcmp(Info->string, "faces") == 0)
						{
							cJSON *faces = Info;
							//��ȡJSON���鳤��
							int count = cJSON_GetArraySize(faces);
							//����JSON����
							for (int i = 0; i < count; i++)
							{
								//��ȡJSON����Ԫ��
								cJSON *face = cJSON_GetArrayItem(faces, i);
								//��ȡJSON����ʵ�ʽڵ�
								face = face->child;
								//����ͼƬ
								while (face != NULL)
								{
									if (strcmp(face->string, "URL") == 0)
									{
										//ͼƬ��������
										picnumber++;
										//����URL����ͼƬ
										URLDownload(face->valuestring);
									}
									//�ڵ������ƶ�
									face = face->next;
								}
							}
						}
						//�ڵ������ƶ�
						Info = Info->next;
					}
				}
			}
			//�ڵ������ƶ�
			JAlarm = JAlarm->next;
		}
	}
	else
	{
		//�Ǳ������ģ�ֱ�Ӵ�ӡ
		printf("%s\n", pData);
	}
	return;
}


/*
URLͼƬ���غ���
ʵ�ָ��ݱ����е�URL��ַ��ͼƬ������������ͼƬ
����urlmon��
���������
url		ͼƬ��URl��ַ
*/
void URLDownload(const char* url)
{
	
	const char *username = "admin";
	const char *password = "abcd1234";

	CURL* curl = curl_easy_init();

	FILE *file;

	//�ж��Ƿ�Ϊ����ͼƬ
	if (picnumber > 1)
	{
		//datetime���ı����ݺ�ͼƬ���ݶ�Ӧ��ϵ�Ĺؼ�
		char cFilename[256] = { 0 };
		//����ͼƬ�ļ�������picnumberΪͼƬ���
		sprintf(cFilename, "./alarm_file/alarm_%s_Pic%d.jpg", date, picnumber);
		//����ͼƬ�ļ�
		file = fopen(cFilename, "wb");
	}
	else
	{
		//datetime���ı����ݺ�ͼƬ���ݶ�Ӧ��ϵ�Ĺؼ�
		char cFilename[256] = { 0 };
		//����ͼƬ�ļ���
		sprintf(cFilename, "./alarm_file/alarm_%s_Pic.jpg", date);
		//����ͼƬ�ļ�
		file = fopen(cFilename, "wb");
	}
		
	if (NULL == curl)
	{
		printf("curl_easy_init error!\n");
		return ;
	}

	//���ý��ճ�ʱʱ��
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30); //����
	//�������ӳ�ʱ
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
	//��ֹ���ʳ�ʱ��ʱ���׳���ʱ�ź�,����û���źŴ���ʱ�����˳�
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
	//��������ӷ��������ִ���CLOSE_WAIT
	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
	//�����ض���
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	//����url
	curl_easy_setopt(curl, CURLOPT_URL, url);
	//������֤
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
	char *a = ":";
	char userpwd[256] = "";
	strcat(userpwd, username);
	strcat(userpwd, a);
	strcat(userpwd, password);
	//�����û���������
	curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd);
	//curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)file);
	CURLcode res = curl_easy_perform(curl);

	if (NULL != curl)
	{
		curl_easy_cleanup(curl);
		curl = NULL;
	}
	if (CURLE_OK != res)
	{
		printf("curl_easy_perform failed:%s\n", curl_easy_strerror(res));
		fclose(file);
		return ;
	}
	fclose(file);
	return;
}

size_t write_file(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}
