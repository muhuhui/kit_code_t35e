#include"JsonParse.h"

int picnumber;
char *datetime;
char *date = "";

void ParseToJson(const char *pData, int length)
{
	//建立json对象，并解析数据为json格式
	cJSON *json = cJSON_Parse(pData);
	//报警报文解析
	if (json->child != NULL&&strcmp(json->child->string, "ipAddress") == 0)
	{
		//建立报警JSON报文解析对象
		cJSON *JAlarm = json->child;
		//对报文进行循环查找节点，针对节点解析
		while (JAlarm != NULL)
		{
			//查找报警时间节点
			if (strcmp(JAlarm->string, "dateTime") == 0)
			{
				//成员变量赋值
				
				datetime = JAlarm->valuestring;
				//删除文件名不接受“：”字符
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
			//对非心跳报文进行保存
			if (strcmp(JAlarm->string, "eventState") == 0)
			{
				if (strcmp(JAlarm->valuestring, "active") == 0)
				{
					//创建文件名
					char chFilename[1024];
					//string转char*
					

					//保存数据报文
					//输入文件名
					sprintf(chFilename, "./alarm_file/%s.txt", date);
					//创建文件对象
					FILE *file;
					//以写方式打开文件
					if ((file = fopen(chFilename, "w+")) != NULL)
					{
						//数据写入文件
						fwrite(pData, sizeof(char), length, file);
						//关闭文件
						fclose(file);
					}
					else
					{
						//文件打开失败
						printf("Create file failed!\n");
					}
				}
			}
			//获取报警上传的人脸抓拍图片
			if (strcmp(JAlarm->string, "captureLibResult") == 0)
			{
				//建立JSON解析对象
				cJSON *captureLib = JAlarm;
				//获取json数组大小
				int count = cJSON_GetArraySize(captureLib);
				//对json数组遍历
				for (int i = 0; i < count; i++)
				{
					//获取JSON数组元素
					cJSON *Info = cJSON_GetArrayItem(captureLib, i);
					//获取JSON数组的实际节点
					Info = Info->child;
					//查找图片数据
					while (Info != NULL)
					{
						if (strcmp(Info->string, "image") == 0)
						{
							//图片数量增加
							picnumber++;
							//根据URL下载图片
							URLDownload(Info->valuestring);
						}
						if (strcmp(Info->string, "targetAttrs") == 0)
						{
							//进入子节点
							cJSON *target = Info->child;
							while (target != NULL)
							{
								//判断是否有背景图片
								if (strcmp(target->string, "bkgUrl") == 0)
								{
									//图片数量增加
									picnumber++;
									//根据URL下载图片
									URLDownload(target->valuestring);
								}
								//节点向下移动
								target = target->next;
							}
						}
						//查找人脸图片
						if (strcmp(Info->string, "faces") == 0)
						{
							cJSON *faces = Info;
							//获取JSON数组长度
							int count = cJSON_GetArraySize(faces);
							//遍历JSON数组
							for (int i = 0; i < count; i++)
							{
								//获取JSON数组元素
								cJSON *face = cJSON_GetArrayItem(faces, i);
								//获取JSON数组实际节点
								face = face->child;
								//查找图片
								while (face != NULL)
								{
									if (strcmp(face->string, "URL") == 0)
									{
										//图片数量增加
										picnumber++;
										//根据URL下载图片
										URLDownload(face->valuestring);
									}
									//节点向下移动
									face = face->next;
								}
							}
						}
						//节点向下移动
						Info = Info->next;
					}
				}
			}
			//节点向下移动
			JAlarm = JAlarm->next;
		}
	}
	else
	{
		//非报警报文，直接打印
		printf("%s\n", pData);
	}
	return;
}


/*
URL图片下载函数
实现根据报文中的URL地址从图片服务器上下载图片
调用urlmon库
输入参数：
url		图片的URl地址
*/
void URLDownload(const char* url)
{
	
	const char *username = "admin";
	const char *password = "abcd1234";

	CURL* curl = curl_easy_init();

	FILE *file;

	//判断是否为单张图片
	if (picnumber > 1)
	{
		//datetime是文本数据和图片数据对应关系的关键
		char cFilename[256] = { 0 };
		//创建图片文件名，以picnumber为图片编号
		sprintf(cFilename, "./alarm_file/alarm_%s_Pic%d.jpg", date, picnumber);
		//创建图片文件
		file = fopen(cFilename, "wb");
	}
	else
	{
		//datetime是文本数据和图片数据对应关系的关键
		char cFilename[256] = { 0 };
		//创建图片文件名
		sprintf(cFilename, "./alarm_file/alarm_%s_Pic.jpg", date);
		//创建图片文件
		file = fopen(cFilename, "wb");
	}
		
	if (NULL == curl)
	{
		printf("curl_easy_init error!\n");
		return ;
	}

	//设置接收超时时间
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30); //接收
	//设置连接超时
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
	//禁止访问超时的时候抛出超时信号,避免没有信号处理时程序退出
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
	//避免短连接服务器出现大量CLOSE_WAIT
	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
	//设置重定向
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	//设置url
	curl_easy_setopt(curl, CURLOPT_URL, url);
	//设置认证
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
	char *a = ":";
	char userpwd[256] = "";
	strcat(userpwd, username);
	strcat(userpwd, a);
	strcat(userpwd, password);
	//设置用户名和密码
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
