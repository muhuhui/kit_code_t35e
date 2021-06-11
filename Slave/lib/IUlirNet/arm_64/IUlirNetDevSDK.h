/*******************************************************************
������
********************************************************************/


#ifndef _IULIRNetDevSDK_H_
#define _IULIRNetDevSDK_H_

#define IULIRNetDevSDK_API  extern "C"__declspec(dllexport)

enum TI615_CMD
{
	CAPTUREGRAYIMAGE = 0x1501,//�Ҷ�					
	CAPTUREJPGIMAGE = 0x1502,//jpg
	CAPTUREBMPIMAGE = 0x1503,//BMP

};

//����������
enum REALSTREAMTYPE
{
	STREAM_TYPE_AVI = 1,			//AVI����
	STREAM_TYPE_GRAYDATA = 2		//�Ҷ�����
};

//�ص���������
enum STREAMTYPE
{
	AVI_HEAD = 0,//avi����ͷ
	AVI_DATA = 1,//avi����
	GRAY_HEAD = 2,//�Ҷ�ͷ
	GRAY_DATA = 3,//�Ҷ�����
};

//���ӷ�ʽ
enum CONNECTTYPE
{
	CONNECT_TYPE_ULIRNET = 4
};
//�¶ȱ�������
enum ALARMTYPE
{
	ALARM_ABOVE = 0,	//�¶ȳ��걨��
	ALARM_BLEW = 1,		//�¶ȹ��ͱ���
	ALARM_DIF = 2,		//�²�걨��
};

//FTP�ļ����� ״̬ 
enum FTP_FILE_STATE
{
	FTP_DOWNLOAD_SUCCEED = 1,	//���سɹ�
	FTP_DOWNLOAD_UNDERWAY = 2,	//��������
	FTP_DOWNLOAD_FAILED = 3		//����ʧ��
};

//һЩ�쳣���͵Ķ���
enum EXPECTION
{
	EX_CMD_BREAK = 1,		//�������ӶϿ�
	EX_CMD_RECONNECT = 2,	//���������������ӳɹ�
	EX_STREAM_BREAK = 3,	//��Ƶ���ӶϿ�
	EX_STREAM_RECONNECT = 4,//��Ƶ�����������ӳɹ�
	EX_ALARM_BREAK = 5,		//�������ӶϿ�
	EX_ALARM_RECONNECT = 6,	//���������������ӳɹ�
};

//��о����
enum ULIR_CONFIG_CMD
{
	ULIR_LINK_CONNECT = 0x0000,					//����						NULL
	ULIR_CFG_GET_DEVICEINFO = 0x0200,			//��ȡ�豸��Ϣ				NET_DEV_DEVICEINFO
	ULIR_CFG_GET_SYSCONFIG = 0x0210,			//��ȡϵͳ����				NET_DEV_SYSCONFIG
	ULIR_CFG_GET_TEMP_PARA = 0x0211,			//��ȡȫ�����²���			_MeasureTempPara
	ULIR_CFG_SET_TEMP_CALIBPARA = 0x0212,		//���ò��²���(�ڲ�ʹ��)		NET_DEV_TEMPCALIBPARA
	ULIR_CFG_SET_TEMP_USERPARA = 0x0213,		//���ò��²���(���û�����)	NET_DEV_TEMPUSERPARA
	ULIR_CFG_GET_REALTIME_PARA = 0x0214,		//��ȡʵʱ���²���			NET_DEV_TEMPREALTIMEPARA
	ULIR_MEASURE_SET_POINT = 0x1010,			//���õ��¶�					NET_DEV_POINTPARA
	ULIR_MEASURE_SET_RECT = 0x1011,				//�����������				NET_DEV_RECTPARA
	ULIR_MEASURE_SET_LINE = 0x1012,				//�����߲���					NET_DEV_LINEPARA
	ULIR_MEASURE_SET_OBJECT = 0x1013,			//���ö���ʹ�ܱ��			NET_DEV_TEMPOBJECT
	ULIR_MEASURE_SET_REFERENCE = 0x1014,		//���òο��¶�				NET_DEV_TEMPREFERENCE
	ULIR_MEASURE_SET_TEMPUNIT = 0x1015,			//���ò��µ�λ				int
	ULIR_MEASURE_SET_TEMPRANGE = 0x1016,		//�л����µ�					NET_DEV_TEMPRANGR
	ULIR_MEASURE_GET_POINT = 0x1020,			//��ȡ���¶�					NET_DEV_POINTPARA
	ULIR_MEASURE_GET_RECT = 0x1021,				//��ȡ�������				NET_DEV_RECTPARA
	ULIR_MEASURE_GET_LINE = 0x1022,				//��ȡ�߲���					NET_DEV_LINEPARA
	ULIR_MEASURE_GET_OBJECT = 0x1023,			//��ȡ����ʹ�ܱ��			NET_DEV_TEMPOBJECT
	ULIR_MEASURE_GET_MAXTEMP = 0x1027,			//��ȡͼ������¶�			int
	ULIR_MEASURE_GET_RECTTEMP = 0x1028,			//��ȡͼ�� �¶���Ϣ			NET_DEV_RECTTEMP
	ULIR_MEASURE_SET_FOURSETSTATE = 0x1029,		//�����ķ�ͼ״̬				NET_DEV_FOURSET
	ULIR_MEASURE_GET_FOURSETSTATE = 0x102a,		//��ȡ�ķ�ͼ״̬				NET_DEV_FOURSET
	ULIR_MEASURE_GET_FOURSETTEMP = 0x102b,		//��ȡ�ķ�ͼ�¶�				NET_DEV_RECTTEMP*4
	ULIR_IMAGE_SET_ISOTEMP = 0x1110,			//����ɫ						NET_DEV_ISOTHERMTEMP
	ULIR_IMAGE_SET_ALARM = 0x1111,				//���ñ���					NET_DEV_ALARMTEMP
	ULIR_IMAGE_SET_COLORRULER = 0x1112,			//�л�ɫ��					int
	ULIR_IMAGE_SET_REVERSE = 0x1113,			//ͼ����					int
	ULIR_IMAGE_SET_MANUAL = 0x1114,				//�ֶ����Զ�ģʽ����			NET_DEV_MANUALPARA
	ULIR_SYS_SET_LANGUAGE = 0x1210,				//����ϵͳ����				int
	ULIR_SYS_SET_TIME = 0x1211,					//����ϵͳʱ��				char[13]
	ULIR_SYS_SET_DEFAULT = 0x1212,				//�ָ���������				NULL
	ULIR_SYS_SET_CAPTURETIME = 0x1215,			//����ץ��ʱ��				int
	ULIR_SYS_SET_AUTORECTIFY = 0x1218,			//�����Զ�����ʱ��			int
	ULIR_SYS_SET_MONITOR_MODE = 0x1220,			//������Ƶ�����ʽ			int
	ULIR_SYS_GET_ALARM_LVINFO = 0x1221,			//��ȡ�����ȼ��¶�			int*
	ULIR_SYS_SET_ALARM_LVINFO = 0x1222,			//���ñ����ȼ��¶�			int*
	ULIR_SYS_SET_ALARM_INFO = 0x1223,			//���ñ�����Ϣ		NET_DEV_ALARMTEMPWITHALARMLV /  NET_DEV_ALARMTEMP
	ULIR_KEY_SET_FREEZE = 0x1317,				//ͼ�񶳽�					int	
	ULIR_KEY_GET_FOCUS_VALUE = 0x1318,			//��ȡ���ಽ��ֵ	int
	ULIR_KEY_SET_FOCUS_VALUE = 0x1319,			//���ý��ಽ����
	ULIR_KEY_ZOOM = 0x1326,						//ͼ��Ŵ�		Int(1�����Ŵ�, 2:�Ŵ�2��, 4:�Ŵ�4��)
	ULIR_SYS_GET_OSD = 0x1700,					//��ȡϵͳ��OSD�������		NET_DEV_OSDSETTING
	ULIR_SYS_SET_OSD = 0x1701,					//����ϵͳ��osd�������		NET_DEV_OSDSETTING
};

enum ULIR_ACTION_CMD
{
	ULIR_KEY_AUTO_RECTIFY = 0x1310,				//����
	ULIR_KEY_AUTO_FOCUS = 0x1311,				//�Զ�����
	ULIR_KEY_FOCUS_NEAR = 0x1312,				//�ֶ�����
	ULIR_KEY_FOCUS_FAR = 0x1313,				//�ֶ�Զ��
	ULIR_KEY_FOCUS_STOP = 0x1314,				//ֹͣ����
	ULIR_KEY_CAPTURE = 0x1320,					//��оץͼ��jpg)
	ULIR_KEY_RECODEAVI = 0x1321,				//¼��(avi)
	ULIR_KEY_REODEIR = 0x1322,					//¼��(�Ҷ�����)
	ULIR_KEY_STOPREODE = 0x1323,				//ֹͣ¼��
	ULIR_KEY_RECTIFY_DOWN = 0x1335, 			//��Ƭ����
	ULIR_KEY_RECTIFY_UP = 0x1336, 				//��Ƭ����
};

//������
enum ULIR_NETDEV_ERROR
{
	NETDEV_NOERROR  = 0,			//û�д���
	NETDEV_NOCONNECT = 11,			//�˿�δ����
	NETDEV_CHANNEL_ERROR = 12,		//ͨ���Ŵ���
	NETDEV_PARAMETER_ERROR = 13,	//��������
	NETDEV_ALLOC_RESOURCE_ERROR = 14,//��Դ�������
	NETDEV_SDK_INIT_ERROR = 15,		//SDK��ʼ��ʧ��
	
	NETDEV_SET_CMD_ERROR = 20,		//���÷����������
	NETDEV_SEND_ERROR = 21,			//���豸����ʧ��
	NETDEV_RECV_ERROR = 22,			//���豸��������ʧ��
	NETDEV_RECVDATA_ERROR = 23,		//���豸���յ���������

	/************************************************/
	NETDEV_WSASTARTUP_FAILED = 30,	//�����ʼ��ʧ��
	NETDEV_CREATESOCK_FAILED = 31,	//�����˿�ʧ��
	NETDEV_BIND_FAILED = 32,		//�˿ڰ�ʧ��
	NETDEV_WSAJOINLEAF_FAILED = 33, //�����鲥ʧ��
	NETDEV_SETPACKAGE_FAILED = 34,  //�������ݰ�ʧ��
	NETDEV_DEVCONNECT_FAILED = 35 , //����ʧ��
	NETDEV_MAX_USERNUM = 36 ,		//�����豸�û��ﵽ����
	NETDEV_NOT_SUCC_LOGIN = 37 ,	//�û�δ����

	/************************************************/
	NETDEV_RE_ERR_INTER = 60 ,				// �豸�ڲ�����
	NETDEV_RE_ERR_MAXCONNECT = 61 ,			// �ﵽ�����������

	NETDEV_RE_ERR_BADCMD = 70 ,				// ������޴�����
	NETDEV_RE_ERR_BADARG = 71,				// ������

	NETDEV_RE_ERR_SD = 80,					// δ��⵽SD��

	NETDEV_RE_ERR_STAT_CAP = 90,            // �����Զ�����״̬
	NETDEV_RE_ERR_STAT_REC = 91,            // ����¼��״̬
	NETDEV_RE_ERR_STAT_PLAY_AVI = 92,       // AVI���ݴ���״̬
	NETDEV_RE_ERR_STAT_PLAY_GRAYDATA = 93,  // �Ҷ����ݴ���״̬

	NETDEV_RE_ERR_SET_IP = 100,				// ����ipʧ��
	NETDEV_RE_ERR_SET_ROUTE = 101,          // ����ip���鲥·������ʧ�ܣ������豸
	NETDEV_RE_ERR_SET_POINT = 102,          // ���õ����ʧ��
	NETDEV_RE_ERR_SET_RECT = 103,           // �����������ʧ��
	NETDEV_RE_ERR_SET_LINE = 104,           // �����߲���ʧ��
	NETDEV_RE_ERR_SET_HIDE_MENU = 105,      // �������ز˵�ʧ��
	NETDEV_RE_ERR_SET_TEMP_USERPARA = 106,  // ���ò��²���ʧ��
	NETDEV_RE_ERR_SET_OBJECT = 107,			// ���¶���ʹ��ʧ��
	NETDEV_RE_ERR_SET_TEMPUNIT = 108,		// �����¶ȵ�λʧ��
	NETDEV_RE_ERR_SET_ALARM = 109,			// ���ñ���ʧ��
	NETDEV_RE_ERR_SET_COLORRULER = 110,		// ����ɫ��ʧ��
	NETDEV_RE_ERR_SET_TIME = 111,			// ����ʱ��ʧ��

	/***************FTP������******************/
	NETDEV_FTP_LOGON_FAILED = 200,			//��¼ʧ��
	NETDEV_FTP_OPEN_LOCALFILE_FAILED = 201,	//�򿪱����ļ�ʧ��
	NETDEV_FTP_FILE_ERR = 202,				//�ļ�����
	NETDEV_FTP_NET_ERR = 203,				//�������
	NETDEV_FTP_CREATESOCK_FAILED = 204,		//���ܴ���SOCKET
	NETDEV_FTP_LOADING_FILE = 205,			//���������ļ�
	NETDEV_FTP_MAX_LOADNUM = 206,			//��ǰ�����Ѿ��ﵽ����
	NETDEV_FTP_CREAT_THREAD_ERR = 207,		//�����߳�ʧ��
	NETDEV_FTP_DELETEFILE_FAILED = 208,		//ɾ���ļ�ʧ��
	NETDEV_FTP_SENDCMD_FAILED = 209,		//��������ʧ��
	NETDEV_FTP_RECVCMD_FAILED = 210,		//��ȡ��Ӧʧ��
};

//Com�ڽṹ
typedef struct
{

	int nComID;			//���ں�
	int nByteSize;		//����λ4,5��6��7��8
	int nStopBit;		//ֹͣλ 1��1.5,2 ��Ӧ��ֵ�ֱ�Ϊ0��1��2
	int nParity ;		//У�飬�� 0����У�� 1��żУ�� 2
	int nBaudRate;		//������
	int nBuffer;		//��������С
	int nWaitTime;		//�ȴ�ʱ��

}NET_DEV_COMINFO;

//(���ֿ��޸�)���²����ṹ
typedef struct 
{
	int Radiance100;		//������100��[1,100]��������Ļ�ķ�����
	int Distance100;		//����100��[10,100000]��
	int Humidity100;		//ʪ��100��[1,100];
	int CorrectFact100;		//����ϵ��100��[1,100]��
	int CorrectTemp100;		//�����¶�100��[-10000,10000]
	int transmissivity100;		//͸����[1,100]
	int Eviment100;			//�����¶�
	char reserve[24];			//24���ֽڵ�Ԥ���ռ�
}NET_DEV_TEMPUSERPARA;

//ʵʱ���²���
typedef struct
{
	int nShiderTemp100;       //��Ƭ�¶ȵ�100����nShiderTemp100��
	int nLensTemp100;         //ǻ���¶ȵ�100����nLensTemp100��
	int nAmbientTemp100;      //�����¶ȵ�100����nAmbientTemp100��
	int nOpenMachineTimeLong; //����ʱ���100����nOpenMachineTimeLong��
	int nShiderTemp100_zero;  //����ʱ�̵�Ƭ�¶ȵ�100����nShiderTemp100						//_zero��
	int nLensTemp100_zero;    //ǻ���¶ȵ�100����nLensTemp100_zero��
	int nBarrierGray;         //��Ƭ�Ҷȵ�100����nBarrierGray
	int nDetectorTemp100;     //̽�����¶ȵ�100����nDetectorTemp100��
	char reserve[32];           //32���ֽڵ�Ԥ���ռ�
}NET_DEV_TEMPREALTIMEPARA;

//���ò��µ�ṹ
typedef struct 
{
	int point_id;			//���µ���[0,9]��
	int p_xcoord;			//��x���ꣻ
	int p_ycoord;			//��y����;
	int p_radiance100;		//�������100��[1,100];
	int refer_flag;			//��Ϊ�ο�[0,1]
	short temp100;
	char reserve[14];         //16���ֽڵ�Ԥ���ռ�
}NET_DEV_POINTPARA;

//���ò�������ṹ
typedef struct 
{
	int rect_id;			//������[0,4];
	int temptype;			//�����������[0,2]:����0=����£�1=����£�2=ƽ����;
	int origin_xcoord;		//����x;������㣨���Ͻǵ㣩��x����
	int origin_ycoord;		//����y;������㣨���Ͻǵ㣩��y����
	int rect_wide;			//����w;����Ŀ�ȡ�
	int rect_high;			//����h;����ĸ߶ȡ�
	int r_radiance100;		//������100��[1,100];
	int refer_flag;			//��Ϊ�ο�[0,1];
	short mintemp100;
	short maxtemp100;
	short avgtemp100;
	short MaxX;
	short MaxY;
	short MinX;
	short MinY;
	char reserve[2];          //16���ֽڵ�Ԥ���ռ�
}NET_DEV_RECTPARA;

//���ò����߽ṹ
typedef struct 
{
	int line_id;			//�߱��[0��1]:0Ϊˮƽ�� 1Ϊ��ֱ��;
	int l_xcoord;			//����x;
	int l_ycoord;			//����y;
	int l_radiance100;		//������100��[1,100]
	short temp100;
	char reserve[14];         //16���ֽڵ�Ԥ���ռ�
}NET_DEV_LINEPARA;

//ÿһ�ֽڱ�ʾ���¶����״̬��1��ʾʹ�ܣ�0��ʾ��ʹ�ܡ���0λ����9λ��ʾ�����0-9��
//��10λ��ʾˮƽ�ߵ�11λ��ʾ��ֱ�ߣ��������ã�����ͬʱΪ1����
//��12λ����16λ��ʾ�������0-4������λԤ����
typedef struct 
{
	char point0_enable;
	char point1_enable;
	char point2_enable;
	char point3_enable;
	char point4_enable;
	char point5_enable;
	char point6_enable;
	char point7_enable;
	char point8_enable;
	char point9_enable;
	char line0_enable;
	char line1_enable;
	char rect0_enable;
	char rect1_enable;
	char rect2_enable;
	char rect3_enable;
	char rect4_enable;
	char reserve[15];       //16���ֽڵ�Ԥ���ռ�
}NET_DEV_TEMPOBJECT;


//ȫͼ�¶���Ϣ
typedef struct 
{
	short nMinTemp100;
	short nMaxTemp100;
	short nAvgTemp100;
	short nMinx;		//���������x
	short nMiny;		//���������y
	short nMaxx;		//���������x
	short nMaxy;		//���������y
	char reserve[10];

}NET_DEV_RECTTEMP;

//�ķ�ͼ״̬
typedef struct 
{
	char active;
	char reserve[15];

}NET_DEV_FOURSET;


//���¶�����Ϊ�ο�
typedef struct 
{
	int enable;				//ʹ��λ[0,1]: 0=�رղο��¶� 1=���òο��¶�
	int ReferTemp100;		//�ο��¶ȵ�100����
}NET_DEV_TEMPREFERENCE;

//�����¶ȵ�λ
typedef struct 
{
	int TempRangeID;		//���µ�λ[0,2]��
	int LensId;				//��ͷ[0,1]
	char reserve[8];			//8���ֽڵ�Ԥ���ռ�
}NET_DEV_TEMPRANGR;

//���õ���ɫ
typedef struct
{
	int IsoTemp100;				//�����¶�100��[-27315,120000]��
	int IsoTempHigh100;			//���¸߶�100��[10,120000]��
	int IsoTempColorId;			//����ɫ��ID[0,10]����ɫ��������RGBֵ��������Ϣ����
	char reserve[8];				//8���ֽڵ�Ԥ���ռ�
}NET_DEV_ISOTHERMTEMP;

//���ñ����¶�(�ޱ����ȼ�)
typedef struct 
{
	int AlarmSwitch;		//��������[0,2]��0�رձ�����1����±�����2����±���;
	int AlarmTemp100;		//�����¶�100����[-27315,120000]��
	int AlarmColorId;		//����ɫ[0-10]: 0=�ر� 1=��ɫ 2=��ɫ 3=��ɫ 4=��ɫ 5=��ɫ 6=��ɫ 7= ��ɫ 8=�ƺ� 9=��ɫ 10=��ɫ
	char reserve[8];			//8���ֽڵ�Ԥ���ռ�
}NET_DEV_ALARMTEMP;

//���ñ����¶�(�б����ȼ�)
typedef struct 
{
	int AlarmSwitch;		//��������[0,2]��0�رձ�����1����±�����2����±���;
	int AlarmLevel;			//�����ȼ�
	int AlarmColorId;		//����ɫ[0-10]: 0=�ر� 1=��ɫ 2=��ɫ 3=��ɫ 4=��ɫ 5=��ɫ 6=��ɫ 7= ��ɫ 8=�ƺ� 9=��ɫ 10=��ɫ
	char reserve[8];			//8���ֽڵ�Ԥ���ռ�
}NET_DEV_ALARMTEMPWITHALARMLV;

//ģʽ����
typedef struct 
{
	int ManualType;			//����[0, 1]��0 = �Զ���1 = �ֶ���
	int TempHighest;		//  ����ֶ�ģʽ: �ֶ����������100��    
	int TempLowest;			// �ֶ����������100�� �Զ�ģʽ��Ϊ��Ϊ0
	char reserve[16];			//16���ֽڵ�Ԥ���ռ�
}NET_DEV_MANUALPARA;

//���ز˵��ṹ
typedef struct
{
	int ShowGrayEnable;		//[0,1] 0=����ʾ�Ҷ� 1=��ʾ�Ҷȣ�
	int CollectEnable;		//[0, 1] 0=���ɼ���������  1=�ɼ���������
	int CorrectTemp100;		//�ڲ������¶ȵ�100��
	int CorrectFact100;		//�ڲ�����ϵ����100��
	char reserve[32];			//32���ֽڵ�Ԥ���ռ�
}NET_DEV_HIDEMENUPARA;

//�豸��Ϣ
typedef struct
{
	char DeviceType[32];		//�ڲ��ͺ� "TI615"
	char OEMDeviceType[32];		//�����ͺ� "IRI-615"
	char DataProtocal[32];		//����Э�� "Ethernet1000M"
	char SerialNumber[20];		//���к� "9008801013"
	char ProduceTime[16];		//�������� "2015-01-01" 
	char SoftwareVersion[16];	//����汾 "Ver1.1"
	char HardwareVersion[16];	//Ӳ���汾 "HW169"
	int  TempCalcType;			//�����㷨��� 21
	int  ImageCalcType;			//ͼ���㷨��� 5
	int  DetectorWidth;			//̽�������ؿ�	640
	int  DetectorHeight;		//̽������		480
	int  BitsPerPixel;			//����λ�� 	14
	int  BytesPerPixel;			//�����ֽ���	2
	int  ImageWidth;			//ͼ���	640
	int  ImageHeight;			//ͼ���	480 49

	//ɫ��
	char ColorRulerName[64];	// "/home/xia/dm6446/ColorRuler.dat"
	int  ColurRulerNum;			// ɫ����Ŀ 12
	int  ColorNumPerRuler;		// ɫ����ɫ�� 512
	int  BytesPerColor;			// ÿ����ɫ�ֽ��� 4
	char ColorRulerType[8];		// "YUV444" 21

	// ����
	int LangNum;				// 10
	char LangArray[32];			// ����ID���� 0Ӣ�� 1���ļ��� 2���ķ��� 3 4 5 6 7 8 9 ���32������	
	int PointNum;				// ������ 10
	int LineNum;				// ������ 2
	int RectNum;				// �������� 10 8

	int TempRangeNum;				// ��ǰ�豸��λ�� 3
	int TempRangeMaxTemp100[5];		// ��λ���ֵ100����Ԥ��5����λ
	int TempRangeMinTemp100[5];		// ��λ��Сֵ100����Ԥ��5����λ
	int LensNum;					// ��ǰ�豸��ͷ�� 2
	int LensDiameter[5];			// ��ͷֱ�����Ժ���Ϊ��λ��Ԥ��5����ͷ
	char Reserved[116];
} NET_DEV_DEVICEINFO;					// 512 bytes

//ϵͳ��Ϣ
typedef struct
{
	// temppara
	int Emissivity100;			// ������
	int Repair100; 				// �û�����ϵ��100��
	int RepairTemp100;			// �û������¶�100��
	int Distance100;			// ����100��
	int Humidity100;			// ʪ��100��
	int ObjectStatus;			// ��������ѡ�б�־λ

	// analysis
	int TempRangeID;			// ��λ
	int OtherLensID;			// ��ͷ
	int TempClass;				// ��λ + 2
	int TempUnit;				// ���µ�λ
	int RefeTempType;			// �ο��¶����� 0 1 2 ..
	int RefeTemp100;			// �ο��¶�100��

	// image
	int AlarmColorID;			// ����ɫ
	int AlarmSwitch;			// �����¶ȿ��� 0�ر� 1����� 2�����
	int AlarmTemp100;			// �����¶�100��
	int IsothermColorID;		// ����ɫ
	int IsothermTemp100;		// �����¶�100��
	int IsothermHight100;		// ���¸߶�100��
	int ColorRulerID;			// ɫ��ID
	int ReverseImage;			// ͼ����
	int ManualType;				// 1�ֶ� 0�Զ�
	int TempHigh100;			// ��ǰ�����100��
	int TempLow100;				// ��ǰ�����100��

	// set
	int LanguageID;				// ����ID
	int AutoZeroTime;			//�Զ�����ʱ��
	int AutoCapTime;			//�Զ�ץ��ʱ��
	int BlackBodyTemp100;		//�����¶�
	int ZoomRate;				//ͼ��Ŵ���
	int MonitorMode;			//��Ƶ��ʽ
	char TimeString[16];		// 20150122105333 
	int ShowGrayEnable;			// �Ҷ���ʾʹ��
	int CollectEnable;			// �ɼ�ʹ��
	int InterCorrectTemp100;	// �ڲ�����
	int InterCorrectFact100;	// �ڲ���ϵ
	int AlarmLevel;				//��ǰ�����ȼ�
	int AlarmLevelNum;			//�����ȼ�����9
	int Transmissivity100;		//͸����
	int Eviment100;				//�����¶�
	char Reserve[108];			// ����
} NET_DEV_SYSCONFIG;					// 256 bytes

//(�����⿪��)���²����ṹ
typedef struct 
{
	int AllK10;				// ��ϵ��
	int ZeroParaA100;		// ��A
	int ZeroParaB1000;		// ��B
	int InterTemp100;		// �ڲ�����100��
	char Reserve[16];
} NET_DEV_TEMPCALIBPARA;			

//�豸Ip��Ϣ
typedef struct	//�豸��Ϣ�ṹ
{

	int Id;
	char Ip[16];
	char netmask[16];
	char gateway[16];
	char reserve[64];

}NET_DEV_DEVIPINFO; 


//�豸ʱ��
typedef struct	
{

	int nYear;
	int nMonth;
	int nDay;
	int nHour;
	int nMinute;

}NET_DEV_DEVTIME; 

//�Ҷ������������ݰ�ͷ�ṹ
typedef struct
{
	int nTag;					// FILEATTR_TAG;
	int nDesLen;				// �ṹ���С

	char btFileVersion[8];		// �ļ��汾��
	char btCreateDate[20];		// ��������
	char btCreateTime[20];		// ����ʱ��
	int nRawFrameSize;			// ÿ֡ԭʼ���ݴ�С
	unsigned int nFileLength;	//�����ļ�����
	int nFPS;					// ֡��
	int nTotalFrames;			// ��֡��

	// product
	int nDectorImageBits;		// ̽����λ��
	int nDectorImageH;			// ̽������ֱ�ֱ���
	int nDectorImageW;			// ̽����ˮƽ�ֱ���
	char btSoftVer[20];			// 1.0.1
	char btProduct[20];			// TI615x

	// color
	int nColorRuleID;			// ɫ��ID
	int nAlarmType;				// ��������
	int bAlarm;					// �Ƿ񱨾�	
	int nAlarmTemp100;			// �����¶�
	int nColorAlarmID;			// ����ɫID
	int nIsothermalLow100;		// ����ɫ����
	int nIsothermalHigh100;		// ����ɫ����
	int nIsothermalID;			// ����ɫID

	// measure temp	
	int nTempParaSize;			// ���²�����С
	int nCalcType;				// �����㷨����
	char nReserved[160];			// ����
} NET_DEV_RAWFILEHEAD;


//osdʹ��Ӧ��
struct NET_DEV_OSDSETTING
{
	char  color_ruler;			//0: ����ʾ1:��ʾ
	char  max_temp_obj;			//0: ����ʾ1:��ʾ
	char  user_setting_obj;		//0: ����ʾ1:��ʾ
	char  time;					//0: ����ʾ1:��ʾ
	char  min_temp_obj;
	char  avg_temp_obj;
	char  reserve[14];
};


/**************************�ص�����*****************************************************************/
//��ȡʵʱ����
typedef void (*GetRealStreamCallBack)(int dwDataType, char* pDataBuffer, int dwDataSize,int *dwUser, short shPort);
/**************************�ص�����*****************************************************************/
typedef void(*fExceptionCallBack)(int dwType,long lHandle,void *pUser);
/**************************�ص�����*****************************************************************/
typedef void (*fAlarmCallBack)(short sHandle,int dwAlarmType,char *pBuffer,int dwBuferSize,int *dwUser);

//-----------------------�ӿں�������-----------------------------

	//��ȡ�����汾��
	void  NetDev_GetVersion(unsigned char* version);

	//��ʼ�������
	 int NetDev_Init();
	//����ʼ�������
	 int NetDev_Cleanup();

	//�����豸  ʧ�ܣ�����-1�� 
	//���ӷ�ʽ�ο�ö��CONNECTTYPE��
	short NetDev_Connect(char *szIP, int dwConnectType);
	//�Ͽ�����
	int NetDev_DisConnect(short shHandle);

	//���������쳣��Ϣ���쳣���;��嶨���EXPECTIONö��
	int NetDev_SetExceptionCallBack(fExceptionCallBack cbExceptionCallBack,void* pUser);
	////��ȡ��ͼ����
	//int NetDev_GetImage(short shHandle,BYTE **pBuffer,DWORD *dwBufSize, LPVOID lpOutRealTimePara = NULL);
	
	//���豸�Ķ���
	int  NetDev_DoAction(short shHandle,int dwActionType);
	//�����豸
	int  NetDev_SetConfig(short shHandle, int dwCmdType, void* lpInBuffer, int dwInBufSize, void* lpOutBuffer , int dwOutBufferSize);
	//��ȡ�豸��Ϣ
	int  NetDev_GetConfig(short shHandle, int dwCmdType, void* lpOutBuffer, int dwOutBufferSize, int *lpBytesReturned);
	//��ȡ������
	int NetDev_GetLastError();
	//��ȡ������Ϣ
	char* NetDev_GetErrorInfo(int dwErrorID);

	//float NetDev_GetTempFromGray(short shHandle, short nGray, float fEmissivity, _MeasureTempPara *pTempPara,int iType);

	//���������豸����
	int NetDev_SearchDev(int *dwDevNum);
	//��ȡ�豸��Ϣ
	int NetDev_GetDevIpInfo(int dwDevIndex, NET_DEV_DEVIPINFO* pDevInfo);
	//�޸��豸IP��Ϣ
	int NetDev_SetDevIpInfo(NET_DEV_DEVIPINFO* pDevInfo);

	//ץ��ͼ
	int NetDev_CaptureGrayImage(short shHandle, char* szInBuf, int dwInBufSize, int *dwRealSize);				
	//ץjpg
	int NetDev_CaptureJPEGPicture(short shHandle,char *szInBuf,int dwInBufSize, int *dwRealSize);
	//ץbmp
	int NetDev_CaptureBMPPicture(short shHandle,char *szInBuf,int dwInBufSize,int *dwRealSize);

	//��ʼ��ȡ����
	//���Ͷ����REALSTREAMTYPE
	int NetDev_StartRealStream(short shHandle, int dwStreamType);       	 
	//������ȡ����
	int NetDev_StopRealStream(short shHandle);  
	//����ʵʱ���ݻص�
	int NetDev_SetRealStreamCallBack(short shHandle, GetRealStreamCallBack funRealStream, int *dwUser);
	//֪ͨ�豸������ؼ�֡
	int NetDev_MakeKeyFrame(short shHandle );   

	//��������ͨ��
	int NetDev_OpenAlarmChan(short shHandle);
	//��������ͨ��
	int NetDev_CloseAlarmChan(short shHandle);
	//���ñ����ص�����
	int NetDev_SetAlarmCallback(short shHandle, fAlarmCallBack funAlarm, int *dwUser);

#endif

