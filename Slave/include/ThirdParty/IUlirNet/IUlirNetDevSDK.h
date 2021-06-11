/*******************************************************************
ÃèÊö£º
********************************************************************/


#ifndef _IULIRNetDevSDK_H_
#define _IULIRNetDevSDK_H_

#define IULIRNetDevSDK_API  extern "C"__declspec(dllexport)

enum TI615_CMD
{
	CAPTUREGRAYIMAGE = 0x1501,//»Ò¶È					
	CAPTUREJPGIMAGE = 0x1502,//jpg
	CAPTUREBMPIMAGE = 0x1503,//BMP

};

//Êý¾ÝÁ÷ÀàÐÍ
enum REALSTREAMTYPE
{
	STREAM_TYPE_AVI = 1,			//AVIÊý¾Ý
	STREAM_TYPE_GRAYDATA = 2		//»Ò¶ÈÊý¾Ý
};

//»Øµ÷Êý¾ÝÀàÐÍ
enum STREAMTYPE
{
	AVI_HEAD = 0,//aviÊý¾ÝÍ·
	AVI_DATA = 1,//aviÊý¾Ý
	GRAY_HEAD = 2,//»Ò¶ÈÍ·
	GRAY_DATA = 3,//»Ò¶ÈÊý¾Ý
};

//Á¬½Ó·½Ê½
enum CONNECTTYPE
{
	CONNECT_TYPE_ULIRNET = 4
};
//ÎÂ¶È±¨¾¯ÀàÐÍ
enum ALARMTYPE
{
	ALARM_ABOVE = 0,	//ÎÂ¶È³¬±ê±¨¾¯
	ALARM_BLEW = 1,		//ÎÂ¶È¹ýµÍ±¨¾¯
	ALARM_DIF = 2,		//ÎÂ²î³¬±ê±¨¾¯
};

//FTPÎÄ¼þÏÂÔØ ×´Ì¬ 
enum FTP_FILE_STATE
{
	FTP_DOWNLOAD_SUCCEED = 1,	//ÏÂÔØ³É¹¦
	FTP_DOWNLOAD_UNDERWAY = 2,	//ÕýÔÚÏÂÔØ
	FTP_DOWNLOAD_FAILED = 3		//ÏÂÔØÊ§°Ü
};

//Ò»Ð©Òì³£ÀàÐÍµÄ¶¨Òå
enum EXPECTION
{
	EX_CMD_BREAK = 1,		//ÃüÁîÁ¬½Ó¶Ï¿ª
	EX_CMD_RECONNECT = 2,	//ÃüÁîÁ¬½ÓÖØÐÂÁ¬½Ó³É¹¦
	EX_STREAM_BREAK = 3,	//ÊÓÆµÁ¬½Ó¶Ï¿ª
	EX_STREAM_RECONNECT = 4,//ÊÓÆµÁ¬½ÓÖØÐÂÁ¬½Ó³É¹¦
	EX_ALARM_BREAK = 5,		//±¨¾¯Á¬½Ó¶Ï¿ª
	EX_ALARM_RECONNECT = 6,	//±¨¾¯Á¬½ÓÖØÐÂÁ¬½Ó³É¹¦
};

//»úÐ¾ÃüÁî
enum ULIR_CONFIG_CMD
{
	ULIR_LINK_CONNECT = 0x0000,					//Á¬½Ó						NULL
	ULIR_CFG_GET_DEVICEINFO = 0x0200,			//»ñÈ¡Éè±¸ÐÅÏ¢				NET_DEV_DEVICEINFO
	ULIR_CFG_GET_SYSCONFIG = 0x0210,			//»ñÈ¡ÏµÍ³ÅäÖÃ				NET_DEV_SYSCONFIG
	ULIR_CFG_GET_TEMP_PARA = 0x0211,			//»ñÈ¡È«²¿²âÎÂ²ÎÊý			_MeasureTempPara
	ULIR_CFG_SET_TEMP_CALIBPARA = 0x0212,		//ÉèÖÃ²âÎÂ²ÎÊý(ÄÚ²¿Ê¹ÓÃ)		NET_DEV_TEMPCALIBPARA
	ULIR_CFG_SET_TEMP_USERPARA = 0x0213,		//ÉèÖÃ²âÎÂ²ÎÊý(¶ÔÓÃ»§¿ª·Å)	NET_DEV_TEMPUSERPARA
	ULIR_CFG_GET_REALTIME_PARA = 0x0214,		//»ñÈ¡ÊµÊ±²âÎÂ²ÎÊý			NET_DEV_TEMPREALTIMEPARA
	ULIR_MEASURE_SET_POINT = 0x1010,			//ÉèÖÃµãÎÂ¶È					NET_DEV_POINTPARA
	ULIR_MEASURE_SET_RECT = 0x1011,				//ÉèÖÃÇøÓò²âÎÂ				NET_DEV_RECTPARA
	ULIR_MEASURE_SET_LINE = 0x1012,				//ÉèÖÃÏß²âÎÂ					NET_DEV_LINEPARA
	ULIR_MEASURE_SET_OBJECT = 0x1013,			//ÉèÖÃ¶ÔÏóÊ¹ÄÜ±ê¼Ç			NET_DEV_TEMPOBJECT
	ULIR_MEASURE_SET_REFERENCE = 0x1014,		//ÉèÖÃ²Î¿¼ÎÂ¶È				NET_DEV_TEMPREFERENCE
	ULIR_MEASURE_SET_TEMPUNIT = 0x1015,			//ÉèÖÃ²âÎÂµ¥Î»				int
	ULIR_MEASURE_SET_TEMPRANGE = 0x1016,		//ÇÐ»»²âÎÂµµ					NET_DEV_TEMPRANGR
	ULIR_MEASURE_GET_POINT = 0x1020,			//»ñÈ¡µãÎÂ¶È					NET_DEV_POINTPARA
	ULIR_MEASURE_GET_RECT = 0x1021,				//»ñÈ¡ÇøÓò²âÎÂ				NET_DEV_RECTPARA
	ULIR_MEASURE_GET_LINE = 0x1022,				//»ñÈ¡Ïß²âÎÂ					NET_DEV_LINEPARA
	ULIR_MEASURE_GET_OBJECT = 0x1023,			//»ñÈ¡¶ÔÏóÊ¹ÄÜ±ê¼Ç			NET_DEV_TEMPOBJECT
	ULIR_MEASURE_GET_MAXTEMP = 0x1027,			//»ñÈ¡Í¼Ïñ×î¸ßÎÂ¶È			int
	ULIR_MEASURE_GET_RECTTEMP = 0x1028,			//»ñÈ¡Í¼Ïñ ÎÂ¶ÈÐÅÏ¢			NET_DEV_RECTTEMP
	ULIR_MEASURE_SET_FOURSETSTATE = 0x1029,		//ÉèÖÃËÄ·ÖÍ¼×´Ì¬				NET_DEV_FOURSET
	ULIR_MEASURE_GET_FOURSETSTATE = 0x102a,		//»ñÈ¡ËÄ·ÖÍ¼×´Ì¬				NET_DEV_FOURSET
	ULIR_MEASURE_GET_FOURSETTEMP = 0x102b,		//»ñÈ¡ËÄ·ÖÍ¼ÎÂ¶È				NET_DEV_RECTTEMP*4
	ULIR_IMAGE_SET_ISOTEMP = 0x1110,			//µÈÎÂÉ«						NET_DEV_ISOTHERMTEMP
	ULIR_IMAGE_SET_ALARM = 0x1111,				//ÉèÖÃ±¨¾¯					NET_DEV_ALARMTEMP
	ULIR_IMAGE_SET_COLORRULER = 0x1112,			//ÇÐ»»É«±ê					int
	ULIR_IMAGE_SET_REVERSE = 0x1113,			//Í¼Ïñ·´Ïò					int
	ULIR_IMAGE_SET_MANUAL = 0x1114,				//ÊÖ¶¯¡¢×Ô¶¯Ä£Ê½ÉèÖÃ			NET_DEV_MANUALPARA
	ULIR_SYS_SET_LANGUAGE = 0x1210,				//ÉèÖÃÏµÍ³ÓïÑÔ				int
	ULIR_SYS_SET_TIME = 0x1211,					//ÉèÖÃÏµÍ³Ê±¼ä				char[13]
	ULIR_SYS_SET_DEFAULT = 0x1212,				//»Ö¸´³ö³§ÉèÖÃ				NULL
	ULIR_SYS_SET_CAPTURETIME = 0x1215,			//ÉèÖÃ×¥ÅÄÊ±¼ä				int
	ULIR_SYS_SET_AUTORECTIFY = 0x1218,			//ÉèÖÃ×Ô¶¯µ÷ÁãÊ±¼ä			int
	ULIR_SYS_SET_MONITOR_MODE = 0x1220,			//ÉèÖÃÊÓÆµÊä³öÖÆÊ½			int
	ULIR_SYS_GET_ALARM_LVINFO = 0x1221,			//»ñÈ¡±¨¾¯µÈ¼¶ÎÂ¶È			int*
	ULIR_SYS_SET_ALARM_LVINFO = 0x1222,			//ÉèÖÃ±¨¾¯µÈ¼¶ÎÂ¶È			int*
	ULIR_SYS_SET_ALARM_INFO = 0x1223,			//ÉèÖÃ±¨¾¯ÐÅÏ¢		NET_DEV_ALARMTEMPWITHALARMLV /  NET_DEV_ALARMTEMP
	ULIR_KEY_SET_FREEZE = 0x1317,				//Í¼Ïñ¶³½á					int	
	ULIR_KEY_GET_FOCUS_VALUE = 0x1318,			//»ñÈ¡½¹¾à²½½øÖµ	int
	ULIR_KEY_SET_FOCUS_VALUE = 0x1319,			//ÉèÖÃ½¹¾à²½½øÖÆ
	ULIR_KEY_ZOOM = 0x1326,						//Í¼Ïñ·Å´ó		Int(1£º²»·Å´ó, 2:·Å´ó2±¶, 4:·Å´ó4±¶)
	ULIR_SYS_GET_OSD = 0x1700,					//»ñÈ¡ÏµÍ³µÄOSDÉèÖÃÇé¿ö		NET_DEV_OSDSETTING
	ULIR_SYS_SET_OSD = 0x1701,					//ÉèÖÃÏµÍ³µÄosdÉèÖÃÇé¿ö		NET_DEV_OSDSETTING
};

enum ULIR_ACTION_CMD
{
	ULIR_KEY_AUTO_RECTIFY = 0x1310,				//µ÷Áã
	ULIR_KEY_AUTO_FOCUS = 0x1311,				//×Ô¶¯µ÷½¹
	ULIR_KEY_FOCUS_NEAR = 0x1312,				//ÊÖ¶¯½ü½¹
	ULIR_KEY_FOCUS_FAR = 0x1313,				//ÊÖ¶¯Ô¶½¹
	ULIR_KEY_FOCUS_STOP = 0x1314,				//Í£Ö¹µ÷½¹
	ULIR_KEY_CAPTURE = 0x1320,					//»úÐ¾×¥Í¼£¨jpg)
	ULIR_KEY_RECODEAVI = 0x1321,				//Â¼Ïñ(avi)
	ULIR_KEY_REODEIR = 0x1322,					//Â¼Ïñ(»Ò¶ÈÊý¾Ý)
	ULIR_KEY_STOPREODE = 0x1323,				//Í£Ö¹Â¼Ïñ
};

//´íÎóÂë
enum ULIR_NETDEV_ERROR
{
	NETDEV_NOERROR  = 0,			//Ã»ÓÐ´íÎó
	NETDEV_NOCONNECT = 11,			//¶Ë¿ÚÎ´Á¬½Ó
	NETDEV_CHANNEL_ERROR = 12,		//Í¨µÀºÅ´íÎó
	NETDEV_PARAMETER_ERROR = 13,	//²ÎÊý´íÎó
	NETDEV_ALLOC_RESOURCE_ERROR = 14,//×ÊÔ´·ÖÅä´íÎó
	NETDEV_SDK_INIT_ERROR = 15,		//SDK³õÊ¼»¯Ê§°Ü
	
	NETDEV_SET_CMD_ERROR = 20,		//ÉèÖÃ·¢ËÍÃüÁî´íÎó
	NETDEV_SEND_ERROR = 21,			//ÏòÉè±¸·¢ËÍÊ§°Ü
	NETDEV_RECV_ERROR = 22,			//´ÓÉè±¸½ÓÊÕÊý¾ÝÊ§°Ü
	NETDEV_RECVDATA_ERROR = 23,		//´ÓÉè±¸½ÓÊÕµÄÊý¾ÝÓÐÎó

	/************************************************/
	NETDEV_WSASTARTUP_FAILED = 30,	//ÍøÂç³õÊ¼»¯Ê§°Ü
	NETDEV_CREATESOCK_FAILED = 31,	//´´½¨¶Ë¿ÚÊ§°Ü
	NETDEV_BIND_FAILED = 32,		//¶Ë¿Ú°ó¶¨Ê§°Ü
	NETDEV_WSAJOINLEAF_FAILED = 33, //¼ÓÈë×é²¥Ê§°Ü
	NETDEV_SETPACKAGE_FAILED = 34,  //ÉèÖÃÊý¾Ý°üÊ§°Ü
	NETDEV_DEVCONNECT_FAILED = 35 , //Á¬½ÓÊ§°Ü
	NETDEV_MAX_USERNUM = 36 ,		//µÇÈëÉè±¸ÓÃ»§´ïµ½ÉÏÏÞ
	NETDEV_NOT_SUCC_LOGIN = 37 ,	//ÓÃ»§Î´µÇÈë

	/************************************************/
	NETDEV_RE_ERR_INTER = 60 ,				// Éè±¸ÄÚ²¿´íÎó
	NETDEV_RE_ERR_MAXCONNECT = 61 ,			// ´ïµ½×î´óÁ¬½ÓÉÏÏÞ

	NETDEV_RE_ERR_BADCMD = 70 ,				// ÃüÁî´í£¬ÎÞ´ËÃüÁî
	NETDEV_RE_ERR_BADARG = 71,				// ²ÎÊý´í

	NETDEV_RE_ERR_SD = 80,					// Î´¼ì²âµ½SD¿¨

	NETDEV_RE_ERR_STAT_CAP = 90,            // ´¦ÓÚ×Ô¶¯ÅÄÕÕ×´Ì¬
	NETDEV_RE_ERR_STAT_REC = 91,            // ´¦ÓÚÂ¼Ïñ×´Ì¬
	NETDEV_RE_ERR_STAT_PLAY_AVI = 92,       // AVIÊý¾Ý´«Êä×´Ì¬
	NETDEV_RE_ERR_STAT_PLAY_GRAYDATA = 93,  // »Ò¶ÈÊý¾Ý´«Êä×´Ì¬

	NETDEV_RE_ERR_SET_IP = 100,				// ÉèÖÃipÊ§°Ü
	NETDEV_RE_ERR_SET_ROUTE = 101,          // ÉèÖÃip£¬×é²¥Â·ÓÉÉèÖÃÊ§°Ü£¬ÖØÆôÉè±¸
	NETDEV_RE_ERR_SET_POINT = 102,          // ÉèÖÃµã²âÎÂÊ§°Ü
	NETDEV_RE_ERR_SET_RECT = 103,           // ÉèÖÃÇøÓò²âÎÂÊ§°Ü
	NETDEV_RE_ERR_SET_LINE = 104,           // ÉèÖÃÏß²âÎÂÊ§°Ü
	NETDEV_RE_ERR_SET_HIDE_MENU = 105,      // ÉèÖÃÒþ²Ø²Ëµ¥Ê§°Ü
	NETDEV_RE_ERR_SET_TEMP_USERPARA = 106,  // ÉèÖÃ²âÎÂ²ÎÊýÊ§°Ü
	NETDEV_RE_ERR_SET_OBJECT = 107,			// ²âÎÂ¶ÔÏóÊ¹ÄÜÊ§°Ü
	NETDEV_RE_ERR_SET_TEMPUNIT = 108,		// ÉèÖÃÎÂ¶Èµ¥Î»Ê§°Ü
	NETDEV_RE_ERR_SET_ALARM = 109,			// ÉèÖÃ±¨¾¯Ê§°Ü
	NETDEV_RE_ERR_SET_COLORRULER = 110,		// ÉèÖÃÉ«±êÊ§°Ü
	NETDEV_RE_ERR_SET_TIME = 111,			// ÉèÖÃÊ±¼äÊ§°Ü

	/***************FTP´íÎóÂë******************/
	NETDEV_FTP_LOGON_FAILED = 200,			//µÇÂ¼Ê§°Ü
	NETDEV_FTP_OPEN_LOCALFILE_FAILED = 201,	//´ò¿ª±¾µØÎÄ¼þÊ§°Ü
	NETDEV_FTP_FILE_ERR = 202,				//ÎÄ¼þ´íÎó
	NETDEV_FTP_NET_ERR = 203,				//ÍøÂç´íÎó
	NETDEV_FTP_CREATESOCK_FAILED = 204,		//²»ÄÜ´´½¨SOCKET
	NETDEV_FTP_LOADING_FILE = 205,			//ÕýÔÚÏÂÔØÎÄ¼þ
	NETDEV_FTP_MAX_LOADNUM = 206,			//µ±Ç°ÏÂÔØÒÑ¾­´ïµ½ÉÏÏÞ
	NETDEV_FTP_CREAT_THREAD_ERR = 207,		//´´½¨Ïß³ÌÊ§°Ü
	NETDEV_FTP_DELETEFILE_FAILED = 208,		//É¾³ýÎÄ¼þÊ§°Ü
	NETDEV_FTP_SENDCMD_FAILED = 209,		//·¢ËÍÃüÁîÊ§°Ü
	NETDEV_FTP_RECVCMD_FAILED = 210,		//»ñÈ¡ÏìÓ¦Ê§°Ü
};

//Com¿Ú½á¹¹
typedef struct
{

	int nComID;			//´®¿ÚºÅ
	int nByteSize;		//Êý¾ÝÎ»4,5£¬6£¬7£¬8
	int nStopBit;		//Í£Ö¹Î» 1£¬1.5,2 ¶ÔÓ¦µÄÖµ·Ö±ðÎª0£¬1£¬2
	int nParity ;		//Ð£Ñé£¬ÎÞ 0£¬ÆæÐ£Ñé 1£¬Å¼Ð£Ñé 2
	int nBaudRate;		//²¨ÌØÂÊ
	int nBuffer;		//»º³åÇø´óÐ¡
	int nWaitTime;		//µÈ´ýÊ±¼ä

}NET_DEV_COMINFO;

//(²¿·Ö¿ÉÐÞ¸Ä)²âÎÂ²ÎÊý½á¹¹
typedef struct 
{
	int Radiance100;		//·øÉäÂÊ100±¶[1,100]£»Õû¸öÆÁÄ»µÄ·øÉäÂÊ
	int Distance100;		//¾àÀë100±¶[10,100000]£»
	int Humidity100;		//Êª¶È100±¶[1,100];
	int CorrectFact100;		//ÐÞÕýÏµÊý100±¶[1,100]£»
	int CorrectTemp100;		//ÐÞÕýÎÂ¶È100±¶[-10000,10000]
	int transmissivity100;		//Í¸ÉäÂÊ[1,100]
	int Eviment100;			//»·¾³ÎÂ¶È
	char reserve[24];			//24¸ö×Ö½ÚµÄÔ¤Áô¿Õ¼ä
}NET_DEV_TEMPUSERPARA;

//ÊµÊ±²âÎÂ²ÎÊý
typedef struct
{
	int nShiderTemp100;       //µ²Æ¬ÎÂ¶ÈµÄ100±¶£ºnShiderTemp100£»
	int nLensTemp100;         //Ç»ÌåÎÂ¶ÈµÄ100±¶£ºnLensTemp100£»
	int nAmbientTemp100;      //»·¾³ÎÂ¶ÈµÄ100±¶£ºnAmbientTemp100£»
	int nOpenMachineTimeLong; //ÔËÐÐÊ±¼äµÄ100±¶£ºnOpenMachineTimeLong£»
	int nShiderTemp100_zero;  //µ÷ÁãÊ±¿Ìµ²Æ¬ÎÂ¶ÈµÄ100±¶£ºnShiderTemp100						//_zero£»
	int nLensTemp100_zero;    //Ç»ÌåÎÂ¶ÈµÄ100±¶£ºnLensTemp100_zero£»
	int nBarrierGray;         //µ²Æ¬»Ò¶ÈµÄ100±¶£ºnBarrierGray
	int nDetectorTemp100;     //Ì½²âÆ÷ÎÂ¶ÈµÄ100±¶£ºnDetectorTemp100£»
	char reserve[32];           //32¸ö×Ö½ÚµÄÔ¤Áô¿Õ¼ä
}NET_DEV_TEMPREALTIMEPARA;

//ÉèÖÃ²âÎÂµã½á¹¹
typedef struct 
{
	int point_id;			//²âÎÂµã±àºÅ[0,9]£»
	int p_xcoord;			//µãx×ø±ê£»
	int p_ycoord;			//µãy×ø±ê;
	int p_radiance100;		//µã·øÉäÂÊ100±¶[1,100];
	int refer_flag;			//ÉèÎª²Î¿¼[0,1]
	short temp100;
	char reserve[14];         //16¸ö×Ö½ÚµÄÔ¤Áô¿Õ¼ä
}NET_DEV_POINTPARA;

//ÉèÖÃ²âÎÂÇøÓò½á¹¹
typedef struct 
{
	int rect_id;			//ÇøÓò±àºÅ[0,4];
	int temptype;			//ÇøÓò²âÎÂÀàÐÍ[0,2]:ÆäÖÐ0=×î¸ßÎÂ£¬1=×îµÍÎÂ£¬2=Æ½¾ùÎÂ;
	int origin_xcoord;		//ÇøÓòx;ÇøÓòÆðµã£¨×óÉÏ½Çµã£©µÄx×ø±ê
	int origin_ycoord;		//ÇøÓòy;ÇøÓòÆðµã£¨×óÉÏ½Çµã£©µÄy×ø±ê
	int rect_wide;			//ÇøÓòw;ÇøÓòµÄ¿í¶È¡£
	int rect_high;			//ÇøÓòh;ÇøÓòµÄ¸ß¶È¡£
	int r_radiance100;		//·øÉäÂÊ100±¶[1,100];
	int refer_flag;			//ÉèÎª²Î¿¼[0,1];
	short mintemp100;
	short maxtemp100;
	short avgtemp100;
	short MaxX;
	short MaxY;
	short MinX;
	short MinY;
	char reserve[2];          //16¸ö×Ö½ÚµÄÔ¤Áô¿Õ¼ä
}NET_DEV_RECTPARA;

//ÉèÖÃ²âÎÂÏß½á¹¹
typedef struct 
{
	int line_id;			//Ïß±àºÅ[0£¬1]:0ÎªË®Æ½Ïß 1Îª´¹Ö±Ïß;
	int l_xcoord;			//×ø±êx;
	int l_ycoord;			//×ø±êy;
	int l_radiance100;		//·øÉäÂÊ100±¶[1,100]
	short temp100;
	char reserve[14];         //16¸ö×Ö½ÚµÄÔ¤Áô¿Õ¼ä
}NET_DEV_LINEPARA;

//Ã¿Ò»×Ö½Ú±íÊ¾²âÎÂ¶ÔÏóµÄ×´Ì¬£¬1±íÊ¾Ê¹ÄÜ£¬0±íÊ¾²»Ê¹ÄÜ¡£µÚ0Î»ÖÁµÚ9Î»±íÊ¾µã²àÎÂ0-9£¬
//µÚ10Î»±íÊ¾Ë®Æ½ÏßµÚ11Î»±íÊ¾´¹Ö±Ïß£¨»¥³âÉèÖÃ£¬²»ÄÜÍ¬Ê±Îª1£©£¬
//µÚ12Î»ÖÁµÚ16Î»±íÊ¾ÇøÓò²âÎÂ0-4£¬ÆäËûÎ»Ô¤Áô¡£
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
	char reserve[15];       //16¸ö×Ö½ÚµÄÔ¤Áô¿Õ¼ä
}NET_DEV_TEMPOBJECT;


//È«Í¼ÎÂ¶ÈÐÅÏ¢
typedef struct 
{
	short nMinTemp100;
	short nMaxTemp100;
	short nAvgTemp100;
	short nMinx;		//×îµÍÎÂ×ø±êx
	short nMiny;		//×îµÍÎÂ×ø±êy
	short nMaxx;		//×î¸ßÎÂ×ø±êx
	short nMaxy;		//×î¸ßÎÂ×ø±êy
	char reserve[10];

}NET_DEV_RECTTEMP;

//ËÄ·ÖÍ¼×´Ì¬
typedef struct 
{
	char active;
	char reserve[15];

}NET_DEV_FOURSET;


//²âÎÂ¶ÔÏóÉèÎª²Î¿¼
typedef struct 
{
	int enable;				//Ê¹ÄÜÎ»[0,1]: 0=¹Ø±Õ²Î¿¼ÎÂ¶È 1=ÉèÖÃ²Î¿¼ÎÂ¶È
	int ReferTemp100;		//²Î¿¼ÎÂ¶ÈµÄ100±¶£»
}NET_DEV_TEMPREFERENCE;

//ÉèÖÃÎÂ¶ÈµµÎ»
typedef struct 
{
	int TempRangeID;		//²âÎÂµµÎ»[0,2]£»
	int LensId;				//¾µÍ·[0,1]
	char reserve[8];			//8¸ö×Ö½ÚµÄÔ¤Áô¿Õ¼ä
}NET_DEV_TEMPRANGR;

//ÉèÖÃµÈÎÂÉ«
typedef struct
{
	int IsoTemp100;				//µÈÎÂÎÂ¶È100±¶[-27315,120000]£»
	int IsoTempHigh100;			//µÈÎÂ¸ß¶È100±¶[10,120000]£»
	int IsoTempColorId;			//µÈÎÂÉ«µÄID[0,10]µÈÎÂÉ«µÄÊýÁ¿ºÍRGBÖµÔÚÒÇÆ÷ÐÅÏ¢¸ø¡£
	char reserve[8];				//8¸ö×Ö½ÚµÄÔ¤Áô¿Õ¼ä
}NET_DEV_ISOTHERMTEMP;

//ÉèÖÃ±¨¾¯ÎÂ¶È(ÎÞ±¨¾¯µÈ¼¶)
typedef struct 
{
	int AlarmSwitch;		//±¨¾¯¿ª¹Ø[0,2]£º0¹Ø±Õ±¨¾¯£¬1×î¸ßÎÂ±¨¾¯£¬2×îµÍÎÂ±¨¾¯;
	int AlarmTemp100;		//±¨¾¯ÎÂ¶È100±¶£º[-27315,120000]£»
	int AlarmColorId;		//±¨¾¯É«[0-10]: 0=¹Ø±Õ 1=ºìÉ« 2=ÂÌÉ« 3=À¶É« 4=ºÚÉ« 5=°×É« 6=»ÒÉ« 7= »ÆÉ« 8=¾Æºì 9=×ÏÉ« 10=³ÈÉ«
	char reserve[8];			//8¸ö×Ö½ÚµÄÔ¤Áô¿Õ¼ä
}NET_DEV_ALARMTEMP;

//ÉèÖÃ±¨¾¯ÎÂ¶È(ÓÐ±¨¾¯µÈ¼¶)
typedef struct 
{
	int AlarmSwitch;		//±¨¾¯¿ª¹Ø[0,2]£º0¹Ø±Õ±¨¾¯£¬1×î¸ßÎÂ±¨¾¯£¬2×îµÍÎÂ±¨¾¯;
	int AlarmLevel;			//±¨¾¯µÈ¼¶
	int AlarmColorId;		//±¨¾¯É«[0-10]: 0=¹Ø±Õ 1=ºìÉ« 2=ÂÌÉ« 3=À¶É« 4=ºÚÉ« 5=°×É« 6=»ÒÉ« 7= »ÆÉ« 8=¾Æºì 9=×ÏÉ« 10=³ÈÉ«
	char reserve[8];			//8¸ö×Ö½ÚµÄÔ¤Áô¿Õ¼ä
}NET_DEV_ALARMTEMPWITHALARMLV;

//Ä£Ê½ÉèÖÃ
typedef struct 
{
	int ManualType;			//ÀàÐÍ[0, 1]£º0 = ×Ô¶¯£»1 = ÊÖ¶¯£º
	int TempHighest;		//  Èç¹ûÊÖ¶¯Ä£Ê½: ÊÖ¶¯ÉèÖÃ×î¸ßÎÂ100±¶    
	int TempLowest;			// ÊÖ¶¯ÉèÖÃ×îµÍÎÂ100±¶ ×Ô¶¯Ä£Ê½¶¼ÎªÉèÎª0
	char reserve[16];			//16¸ö×Ö½ÚµÄÔ¤Áô¿Õ¼ä
}NET_DEV_MANUALPARA;

//Òþ²Ø²Ëµ¥½á¹¹
typedef struct
{
	int ShowGrayEnable;		//[0,1] 0=²»ÏÔÊ¾»Ò¶È 1=ÏÔÊ¾»Ò¶È£»
	int CollectEnable;		//[0, 1] 0=²»²É¼¯¶¨±êÊý¾Ý  1=²É¼¯¶¨±êÊý¾Ý
	int CorrectTemp100;		//ÄÚ²¿ÐÞÕýÎÂ¶ÈµÄ100±¶
	int CorrectFact100;		//ÄÚ²¿ÐÞÕýÏµÊýµÄ100±¶
	char reserve[32];			//32¸ö×Ö½ÚµÄÔ¤Áô¿Õ¼ä
}NET_DEV_HIDEMENUPARA;

//Éè±¸ÐÅÏ¢
typedef struct
{
	char DeviceType[32];		//ÄÚ²¿ÐÍºÅ "TI615"
	char OEMDeviceType[32];		//ÌùÅÆÐÍºÅ "IRI-615"
	char DataProtocal[32];		//Êý¾ÝÐ­Òé "Ethernet1000M"
	char SerialNumber[20];		//ÐòÁÐºÅ "9008801013"
	char ProduceTime[16];		//Éú²úÈÕÆÚ "2015-01-01" 
	char SoftwareVersion[16];	//Èí¼þ°æ±¾ "Ver1.1"
	char HardwareVersion[16];	//Ó²¼þ°æ±¾ "HW169"
	int  TempCalcType;			//²âÎÂËã·¨±àºÅ 21
	int  ImageCalcType;			//Í¼ÏñËã·¨±àºÅ 5
	int  DetectorWidth;			//Ì½²âÆ÷ÏñËØ¿í	640
	int  DetectorHeight;		//Ì½²âÆ÷¸ß		480
	int  BitsPerPixel;			//ÏñËØÎ»Êý 	14
	int  BytesPerPixel;			//ÏñËØ×Ö½ÚÊý	2
	int  ImageWidth;			//Í¼Ïñ¿í	640
	int  ImageHeight;			//Í¼Ïñ¸ß	480 49

	//É«±ê
	char ColorRulerName[64];	// "/home/xia/dm6446/ColorRuler.dat"
	int  ColurRulerNum;			// É«±êÊýÄ¿ 12
	int  ColorNumPerRuler;		// É«±êÑÕÉ«Êý 512
	int  BytesPerColor;			// Ã¿¸öÑÕÉ«×Ö½ÚÊý 4
	char ColorRulerType[8];		// "YUV444" 21

	// ÓïÑÔ
	int LangNum;				// 10
	char LangArray[32];			// ÓïÑÔIDÊý×é 0Ó¢Óï 1ÖÐÎÄ¼òÌå 2ÖÐÎÄ·±Ìå 3 4 5 6 7 8 9 ×î¶à32ÖÖÓïÑÔ	
	int PointNum;				// µãÊýÁ¿ 10
	int LineNum;				// ÏßÊýÁ¿ 2
	int RectNum;				// ÇøÓòÊýÁ¿ 10 8

	int TempRangeNum;				// µ±Ç°Éè±¸µµÎ»Êý 3
	int TempRangeMaxTemp100[5];		// µµÎ»×î´óÖµ100±¶£¬Ô¤Áô5¸öµµÎ»
	int TempRangeMinTemp100[5];		// µµÎ»×îÐ¡Öµ100±¶£¬Ô¤Áô5¸öµµÎ»
	int LensNum;					// µ±Ç°Éè±¸¾µÍ·Êý 2
	int LensDiameter[5];			// ¾µÍ·Ö±¾¶£¬ÒÔºÁÃ×Îªµ¥Î»£¬Ô¤Áô5¸ö¾µÍ·
	char Reserved[116];
} NET_DEV_DEVICEINFO;					// 512 bytes

//ÏµÍ³ÐÅÏ¢
typedef struct
{
	// temppara
	int Emissivity100;			// ·øÉäÂÊ
	int Repair100; 				// ÓÃ»§ÐÞÕýÏµÊý100±¶
	int RepairTemp100;			// ÓÃ»§ÐÞÕýÎÂ¶È100±¶
	int Distance100;			// ¾àÀë100±¶
	int Humidity100;			// Êª¶È100±¶
	int ObjectStatus;			// µãÏßÇøÓòÑ¡ÖÐ±êÖ¾Î»

	// analysis
	int TempRangeID;			// µµÎ»
	int OtherLensID;			// ¾µÍ·
	int TempClass;				// µµÎ» + 2
	int TempUnit;				// ²âÎÂµ¥Î»
	int RefeTempType;			// ²Î¿¼ÎÂ¶ÈÀàÐÍ 0 1 2 ..
	int RefeTemp100;			// ²Î¿¼ÎÂ¶È100±¶

	// image
	int AlarmColorID;			// ±¨¾¯É«
	int AlarmSwitch;			// ±¨¾¯ÎÂ¶È¿ª¹Ø 0¹Ø±Õ 1×î¸ßÎÂ 2×îµÍÎÂ
	int AlarmTemp100;			// ±¨¾¯ÎÂ¶È100±¶
	int IsothermColorID;		// µÈÎÂÉ«
	int IsothermTemp100;		// µÈÎÂÎÂ¶È100±¶
	int IsothermHight100;		// µÈÎÂ¸ß¶È100±¶
	int ColorRulerID;			// É«±êID
	int ReverseImage;			// Í¼Ïñ·´Ïà
	int ManualType;				// 1ÊÖ¶¯ 0×Ô¶¯
	int TempHigh100;			// µ±Ç°×î¸ßÎÂ100±¶
	int TempLow100;				// µ±Ç°×îµÍÎÂ100±¶

	// set
	int LanguageID;				// ÓïÑÔID
	int AutoZeroTime;			//×Ô¶¯µ÷ÁãÊ±¼ä
	int AutoCapTime;			//×Ô¶¯×¥ÅÄÊ±¼ä
	int BlackBodyTemp100;		//ºÚÌåÎÂ¶È
	int ZoomRate;				//Í¼Ïñ·Å´ó±¶Êý
	int MonitorMode;			//ÊÓÆµÖÆÊ½
	char TimeString[16];		// 20150122105333 
	int ShowGrayEnable;			// »Ò¶ÈÏÔÊ¾Ê¹ÄÜ
	int CollectEnable;			// ²É¼¯Ê¹ÄÜ
	int InterCorrectTemp100;	// ÄÚ²¿ÐÞÎÂ
	int InterCorrectFact100;	// ÄÚ²¿ÐÞÏµ
	int AlarmLevel;				//µ±Ç°±¨¾¯µÈ¼¶
	int AlarmLevelNum;			//±¨¾¯µÈ¼¶¸öÊý9
	int Transmissivity100;		//Í¸ÉäÂÊ
	int Eviment100;				//»·¾³ÎÂ¶È
	char Reserve[108];			// ±£Áô
} NET_DEV_SYSCONFIG;					// 256 bytes

//(²»¶ÔÍâ¿ª·Å)²âÎÂ²ÎÊý½á¹¹
typedef struct 
{
	int AllK10;				// ×ÜÏµÊý
	int ZeroParaA100;		// ÐÞA
	int ZeroParaB1000;		// ÐÞB
	int InterTemp100;		// ÄÚ²¿ÐÞÎÂ100±¶
	char Reserve[16];
} NET_DEV_TEMPCALIBPARA;			

//Éè±¸IpÐÅÏ¢
typedef struct	//Éè±¸ÐÅÏ¢½á¹¹
{

	int Id;
	char Ip[16];
	char netmask[16];
	char gateway[16];
	char reserve[64];

}NET_DEV_DEVIPINFO; 


//Éè±¸Ê±¼ä
typedef struct	
{

	int nYear;
	int nMonth;
	int nDay;
	int nHour;
	int nMinute;

}NET_DEV_DEVTIME; 

//»Ò¶ÈÊý¾ÝÁ÷£¬Êý¾Ý°üÍ·½á¹¹
typedef struct
{
	int nTag;					// FILEATTR_TAG;
	int nDesLen;				// ½á¹¹Ìå´óÐ¡

	char btFileVersion[8];		// ÎÄ¼þ°æ±¾ºÅ
	char btCreateDate[20];		// ±£´æÈÕÆÚ
	char btCreateTime[20];		// ±£´æÊ±·Ö
	int nRawFrameSize;			// Ã¿Ö¡Ô­Ê¼Êý¾Ý´óÐ¡
	unsigned int nFileLength;	//Õû¸öÎÄ¼þ³¤¶È
	int nFPS;					// Ö¡ÂÊ
	int nTotalFrames;			// ×ÜÖ¡Êý

	// product
	int nDectorImageBits;		// Ì½²âÆ÷Î»Êý
	int nDectorImageH;			// Ì½²âÆ÷´¹Ö±·Ö±æÂÊ
	int nDectorImageW;			// Ì½²âÆ÷Ë®Æ½·Ö±æÂÊ
	char btSoftVer[20];			// 1.0.1
	char btProduct[20];			// TI615x

	// color
	int nColorRuleID;			// É«±êID
	int nAlarmType;				// ±¨¾¯ÀàÐÍ
	int bAlarm;					// ÊÇ·ñ±¨¾¯	
	int nAlarmTemp100;			// ±¨¾¯ÎÂ¶È
	int nColorAlarmID;			// ±¨¾¯É«ID
	int nIsothermalLow100;		// µÈÎÂÉ«ÏÂÏÞ
	int nIsothermalHigh100;		// µÈÎÂÉ«ÉÏÏÞ
	int nIsothermalID;			// µÈÎÂÉ«ID

	// measure temp	
	int nTempParaSize;			// ²âÎÂ²ÎÊý´óÐ¡
	int nCalcType;				// ²âÎÂËã·¨ÀàÐÍ
	char nReserved[160];			// ±£Áô
} NET_DEV_RAWFILEHEAD;


//osdÊ¹ÄÜÓ¦ÓÃ
struct NET_DEV_OSDSETTING
{
	char  color_ruler;			//0: ²»ÏÔÊ¾1:ÏÔÊ¾
	char  max_temp_obj;			//0: ²»ÏÔÊ¾1:ÏÔÊ¾
	char  user_setting_obj;		//0: ²»ÏÔÊ¾1:ÏÔÊ¾
	char  time;					//0: ²»ÏÔÊ¾1:ÏÔÊ¾
	char  min_temp_obj;
	char  avg_temp_obj;
	char  reserve[14];
};


/**************************»Øµ÷º¯Êý*****************************************************************/
//»ñÈ¡ÊµÊ±Êý¾Ý
typedef void (*GetRealStreamCallBack)(int dwDataType, char* pDataBuffer, int dwDataSize,int *dwUser, short shPort);
/**************************»Øµ÷º¯Êý*****************************************************************/
typedef void(*fExceptionCallBack)(int dwType,long lHandle,void *pUser);
/**************************»Øµ÷º¯Êý*****************************************************************/
typedef void (*fAlarmCallBack)(short sHandle,int dwAlarmType,char *pBuffer,int dwBuferSize,int *dwUser);

//-----------------------½Ó¿Úº¯Êý¶¨Òå-----------------------------
#ifdef __cplusplus
extern "C" {
#endif

	//»ñÈ¡ÍøÂç¿â°æ±¾ºÅ
	void  NetDev_GetVersion(unsigned char* version);

	//³õÊ¼»¯ÍøÂç¿â
	 int NetDev_Init();
	//·´³õÊ¼»¯ÍøÂç¿â
	 int NetDev_Cleanup();

	//Á¬½ÓÉè±¸  Ê§°Ü£º·µ»Ø-1£» 
	//Á¬½Ó·½Ê½²Î¿¼Ã¶¾ÙCONNECTTYPE¡£
	short NetDev_Connect(char *szIP, int dwConnectType);
	//¶Ï¿ªÁ¬½Ó
	int NetDev_DisConnect(short shHandle);

	//ÉèÖÃÍøÂçÒì³£ÐÅÏ¢£¬Òì³£ÀàÐÍ¾ßÌå¶¨Òå¼ûEXPECTIONÃ¶¾Ù
	int NetDev_SetExceptionCallBack(fExceptionCallBack cbExceptionCallBack,void* pUser);
	////»ñÈ¡»ÒÍ¼Êý¾Ý
	//int NetDev_GetImage(short shHandle,BYTE **pBuffer,DWORD *dwBufSize, LPVOID lpOutRealTimePara = NULL);
	
	//¶ÔÉè±¸µÄ¶¯×÷
	int  NetDev_DoAction(short shHandle,int dwActionType);
	//ÉèÖÃÉè±¸
	int  NetDev_SetConfig(short shHandle, int dwCmdType, void* lpInBuffer, int dwInBufSize, void* lpOutBuffer , int dwOutBufferSize);
	//»ñÈ¡Éè±¸ÐÅÏ¢
	int  NetDev_GetConfig(short shHandle, int dwCmdType, void* lpOutBuffer, int dwOutBufferSize, int *lpBytesReturned);
	//»ñÈ¡´íÎóÂë
	int NetDev_GetLastError();
	//»ñÈ¡´íÎóÐÅÏ¢
	char* NetDev_GetErrorInfo(int dwErrorID);

	//float NetDev_GetTempFromGray(short shHandle, short nGray, float fEmissivity, _MeasureTempPara *pTempPara,int iType);

	//ÊÕË÷ÔÚÏßÉè±¸¸öÊý
	int NetDev_SearchDev(int *dwDevNum);
	//»ñÈ¡Éè±¸ÐÅÏ¢
	int NetDev_GetDevIpInfo(int dwDevIndex, NET_DEV_DEVIPINFO* pDevInfo);
	//ÐÞ¸ÄÉè±¸IPÐÅÏ¢
	int NetDev_SetDevIpInfo(NET_DEV_DEVIPINFO* pDevInfo);

	//×¥»ÒÍ¼
	int NetDev_CaptureGrayImage(short shHandle, char* szInBuf, int dwInBufSize, int *dwRealSize);				
	//×¥jpg
	int NetDev_CaptureJPEGPicture(short shHandle,char *szInBuf,int dwInBufSize, int *dwRealSize);
	//×¥bmp
	int NetDev_CaptureBMPPicture(short shHandle,char *szInBuf,int dwInBufSize,int *dwRealSize);

	//¿ªÊ¼»ñÈ¡Êý¾Ý
	//ÀàÐÍ¶¨Òå¼ûREALSTREAMTYPE
	int NetDev_StartRealStream(short shHandle, int dwStreamType);       	 
	//½áÊø»ñÈ¡Êý¾Ý
	int NetDev_StopRealStream(short shHandle);  
	//ÉèÖÃÊµÊ±Êý¾Ý»Øµ÷
	int NetDev_SetRealStreamCallBack(short shHandle, GetRealStreamCallBack funRealStream, int *dwUser);
	//Í¨ÖªÉè±¸£¬²åÈë¹Ø¼üÖ¡
	int NetDev_MakeKeyFrame(short shHandle );   

	//¿ªÆô±¨¾¯Í¨µÀ
	int NetDev_OpenAlarmChan(short shHandle);
	//½áÊø±¨¾¯Í¨µÀ
	int NetDev_CloseAlarmChan(short shHandle);
	//ÉèÖÃ±¨¾¯»Øµôº¯Êý
	int NetDev_SetAlarmCallback(short shHandle, fAlarmCallBack funAlarm, int *dwUser);

#ifdef __cplusplus
}
#endif

#endif

