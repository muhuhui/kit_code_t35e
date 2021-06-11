#include"XmlParse.h"

char* ParsetoXML(const char *pData, int length)
{
	//������������
	char *Event = "EventNotificationAlert";
	char *eventType = "eventType";
	char *eventState = "eventState";
	char *detectionPicturesNumber = "detectionPicturesNumber";
	char *thermalURL = "thermalURL";

	//��ʼ������ֵ
	char *returnData="";

	//���Ľ����Ļ�����
	char *buf="";

	//�������ı�ʶ
	bool active = false;

	//��ʼ����XML����
	if (strstr(pData, Event)!=NULL)
	{
		//��������״̬����
		char state[ENENTSIZE];
		//��ȡ������״̬������
		buf = (char*)strstr(pData, eventState);
		if (buf != "")
		{
			//��ȡ����״̬
			char c;
			//+1��ָ��>���ĳ��ȣ���ʱƫ������ָλ��Ϊ����״̬ʵ������
			int index = strlen(eventState) + 1;

			//��ȡ����״̬ʵ������
			for (int i = 0; i<strlen(buf); i++)
			{
				c = buf[index];
				//�ǽ�����ʶ
				if (c != '<')
				{
					state[i] = c;
				}
				else
				{
					//��ȡ������ĩβ���Ͻ�����
					state[i] = '\0';
					break;
				}
				index++;
			}
		}
		//���������ı���״̬���ж��Ƿ���������
		if (strcmp(state, "inactive") != 0)
		{
			// ��ȡ��������
			char *eType = (char*)malloc(sizeof(char)*ENENTSIZE);
			//��ȡ���������͵�����
			buf = (char*)strstr(pData, eventType);
			if (buf != "")
			{
				// ��ȡ��������
				char c;
				//+1��ָ��>���ĳ��ȣ���ʱƫ������ָλ��Ϊ��������ʵ�����ݵ����
				int index = strlen(eventType) + 1;
				for (int i = 0; i < strlen(buf); i++)
				{
					c = buf[index];
					//�ǽ�����ʶ
					if (c != '<')
					{
						eType[i] = c;
					}
					else
					{
						//��ȡ������ĩβ���Ͻ�����
						eType[i] = '\0';
						break;
					}
					index++;
				}

				//��ʾ�û����յ�����
				printf("Alarm Recived\n");

				//���ر�������
				returnData = eType;
			}
		}
		else
		{
			//������������ֵΪ��
			returnData = "";
		}
		return returnData;
	}
	else
	{
		//�Ǳ������ģ�ֱ�����
		printf("%s", pData);
		return returnData;
	}
	
}