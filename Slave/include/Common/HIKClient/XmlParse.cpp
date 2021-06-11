#include"XmlParse.h"

char* ParsetoXML(const char *pData, int length)
{
	//建立解析条件
	char *Event = "EventNotificationAlert";
	char *eventType = "eventType";
	char *eventState = "eventState";
	char *detectionPicturesNumber = "detectionPicturesNumber";
	char *thermalURL = "thermalURL";

	//初始化返回值
	char *returnData="";

	//报文解析的缓冲区
	char *buf="";

	//心跳报文标识
	bool active = false;

	//开始进行XML解析
	if (strstr(pData, Event)!=NULL)
	{
		//建立报文状态缓冲
		char state[ENENTSIZE];
		//获取含报文状态的数据
		buf = (char*)strstr(pData, eventState);
		if (buf != "")
		{
			//获取报文状态
			char c;
			//+1是指“>”的长度，此时偏移量所指位置为报文状态实体的起点
			int index = strlen(eventState) + 1;

			//获取报文状态实体内容
			for (int i = 0; i<strlen(buf); i++)
			{
				c = buf[index];
				//非结束标识
				if (c != '<')
				{
					state[i] = c;
				}
				else
				{
					//获取结束，末尾加上结束符
					state[i] = '\0';
					break;
				}
				index++;
			}
		}
		//根据上述的报文状态，判断是否不是心跳包
		if (strcmp(state, "inactive") != 0)
		{
			// 获取报警类型
			char *eType = (char*)malloc(sizeof(char)*ENENTSIZE);
			//获取含报警类型的数据
			buf = (char*)strstr(pData, eventType);
			if (buf != "")
			{
				// 获取报警类型
				char c;
				//+1是指“>”的长度，此时偏移量所指位置为报警类型实体内容的起点
				int index = strlen(eventType) + 1;
				for (int i = 0; i < strlen(buf); i++)
				{
					c = buf[index];
					//非结束标识
					if (c != '<')
					{
						eType[i] = c;
					}
					else
					{
						//获取结束，末尾加上结束符
						eType[i] = '\0';
						break;
					}
					index++;
				}

				//提示用户接收到报警
				printf("Alarm Recived\n");

				//返回报警类型
				returnData = eType;
			}
		}
		else
		{
			//心跳包，返回值为空
			returnData = "";
		}
		return returnData;
	}
	else
	{
		//非报警报文，直接输出
		printf("%s", pData);
		return returnData;
	}
	
}