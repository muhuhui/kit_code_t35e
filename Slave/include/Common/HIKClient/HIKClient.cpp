#include "HIKClient.h"
#include <stdlib.h>
#include "ThirdParty/jsoncpp/json.h"

HIKClient* HIKClient::m_pInstance = NULL;

int xmlInterpret(char* pData, char* Event)
{
    char* buf = (char*)strstr(pData, Event);
    if (buf != NULL)
    {
		char state[ENENTSIZE];
        char c;
        int index = strlen(Event) + 1;
        for (int i = 0; i<strlen(buf); i++)
        {
            c = buf[index];
            if (c != '<')
            {
                state[i] = c;
            }
            else
            {
                state[i] = '\0';
                break;
            }
            index++;
        }
        return atoi(state);
    }
    else{
        printf("No matching phrase.\n");
        return -1000;
    }   
}

HIKClient* HIKClient::getInstance()
{
    if (m_pInstance == NULL) {
        if (m_pInstance == NULL) {
            m_pInstance = new HIKClient();
        }
    }
    return m_pInstance;
}

void HIKClient::init(const char *ip, const char *port, const char *username, const char *password)
{
    snprintf(m_ip, sizeof(m_ip), "%s", ip);
    snprintf(m_port, sizeof(m_port), "%s", port);
    snprintf(m_username, sizeof(m_username), "%s", username);
    snprintf(m_password, sizeof(m_password), "%s", password);
    m_mintemp = -50.0;
    m_maxtemp = 300.0;
    printf("HIKClient::%s(), ip: %s, port: %s, username: %s, password: %s\n", __FUNCTION__, m_ip, m_port, m_username, m_password);
}

int HIKClient::pixelParamConfig()
{
    char url[128] = "/ISAPI/Thermal/channels/2/thermometry/pixelToPixelParam";
    char URL[256] = "http://";
    strcat(URL, m_ip);
    strcat(URL, ":");
    strcat(URL, m_port);
    strcat(URL, url);

    UserInfo info = {};
    info.username = m_username;
    info.password = m_password;

    char *DataResponse = new char[BUFFERSIZE];
    memset(DataResponse, 0, BUFFERSIZE);
    char inboundData[1024] = "\
    <?xml version=\"1.0\" encoding=\"UTF-8\"?>\
    <PixelToPixelParamCap version=\"2.0\" xmlns=\"http://www.hikvision.com/ver20/XMLSchema\">\
        <id>2</id>\
        <maxFrameRate opt=\"400,200,100,50,25,12,6\">400</maxFrameRate>\
        <reflectiveEnable opt=\"true,false\">false</reflectiveEnable>\
        <reflectiveTemperature min=\"-100.00\" max=\"1000.00\">20.00</reflectiveTemperature>\
        <emissivity min=\"0.01\" max=\"1.00\">0.96</emissivity>\
        <distance min=\"0\" max=\"50\">30</distance>\
        <refreshInterval min=\"1\" max=\"500\">50</refreshInterval>\
        <distanceUnit opt=\"meter,feet,centimeter\">meter</distanceUnit>\
        <temperatureDataLength opt=\"2,4\">4</temperatureDataLength>\
        <JpegPictureWithAppendData>\
            <jpegPicEnabled opt=\"true,false\">false</jpegPicEnabled>\
        </JpegPictureWithAppendData>\
    </PixelToPixelParamCap>\
    ";

    CURLcode res = httpPut(URL, inboundData, DataResponse, &info);
    if (res == 0) {
        printf("%s\n", DataResponse);
    }
    else {
        printf("curl_easy_perform failed:%s\n", curl_easy_strerror((CURLcode)res));
    }

    memset(DataResponse, 0, BUFFERSIZE);
    res = httpGet(URL, DataResponse, &info);
    if (res == 0) {
        printf("%s\n", DataResponse);
    }
    else {
        printf("curl_easy_perform failed:%s\n", curl_easy_strerror((CURLcode)res));
    }

    delete[]DataResponse;
    DataResponse = NULL;

    return res;
}

int HIKClient::getTempData(char* pData, int size)
{
    char url[128] = "/ISAPI/Thermal/channels/2/thermometry/jpegPicWithAppendData?format=json";
    char URL[256] = "http://";
    strcat(URL, m_ip);
    strcat(URL, ":");
    strcat(URL, m_port);
    strcat(URL, url);

    UserInfo info = {};
    info.username = m_username;
    info.password = m_password;

    if (pData == NULL) {
        return -1;
    }
    FILE *file;
    if ((file = fopen("./.temp_buffer", "w+")) == NULL) {
        printf("HIKClient::%s, open buffer failed!\n", __FUNCTION__);
        return -1;
    }
    fclose(file);
    if ((file = fopen("./.temp_buffer", "a+")) == NULL) {
        printf("HIKClient::%s, open buffer failed!\n", __FUNCTION__);
        return -1;
    }
    CURLcode res = httpGetBin(URL, file, &info);
    fclose(file);
    int readLen = 0;
    if (res == 0) {
        if ((file = fopen("./.temp_buffer", "r")) == NULL) {
            printf("HIKClient::%s, open buffer failed!\n", __FUNCTION__);
            return -1;
        }
        readLen = fread(pData, 1, size, file);
        printf("HIKClient::%s, size: %d, buffer readLen: %d!\n", __FUNCTION__, size, readLen);
        if (readLen <= 0) {
            printf("HIKClient::%s, read buffer failed!\n", __FUNCTION__);
            fclose(file);
            return -1;
        }
        fclose(file);
        for (int i = 0; i < 400; i++) {
           printf("%c", (unsigned char)pData[i]);
        }
        printf("\n");
    }
    else {
        printf("curl_easy_perform failed:%s\n", curl_easy_strerror((CURLcode)res));
    }

    return readLen;
}

int HIKClient::getTempFile(const char* pFilePath)
{
    int picWidth = 0, picHeight = 0, dataLen = 0;
    const int BUF_LEN = 1024*2048;
    char buffer[BUF_LEN] = "";
    memset(buffer, 0, BUF_LEN);
    int readLen = 0;
    if ((readLen = getTempData(buffer, BUF_LEN)) < 0) {
        return -1;
    }
    if (pFilePath == NULL) {
        return -1;
    }
	char *boundary = (char*)"--boundary\r\nContent-Type: ";
    char *pIndex = buffer;
	if ((pIndex = strstr(pIndex, boundary)) == NULL) {
        printf("HIKClient::%s, no boundary found!\n", __FUNCTION__);
        return -1;
	}
    char *ContentT = (char*)"Content-Length: ";
	if ((pIndex = strstr(pIndex, ContentT)) == NULL) {
        printf("HIKClient::%s, no content length found!\n", __FUNCTION__);
        return -1;
	}
    pIndex += strlen(ContentT);
    char chLen[16] = {0};
    for (int i = 0; i < 16 && *pIndex != '\r'; i++, pIndex++) {
        chLen[i] = *pIndex;
    }
    int nChLen = atoi(chLen);
    printf("HIKClient::%s, json content length: %d\n", __FUNCTION__, nChLen);
    if (nChLen <= 0) {
        printf("HIKClient::%s, invalid content length!\n", __FUNCTION__);
        return -1;
    }

	if ((pIndex = strstr(pIndex, "{")) == NULL) {
        printf("HIKClient::%s, no json content found!\n", __FUNCTION__);
        return -1;
	}
    char thJson[1024];
    memset(thJson, 0, sizeof(thJson));
    snprintf(thJson, nChLen+1, "%s", pIndex);

    Json::Reader reader;
    Json::Value val;
    std::string sJson = thJson;
    if (reader.parse(sJson, val, false)) {
        if (val["JpegPictureWithAppendData"].isNull() ||
            val["JpegPictureWithAppendData"]["jpegPicWidth"].isNull() ||
            val["JpegPictureWithAppendData"]["jpegPicHeight"].isNull()) {
            printf("%s(), appenddata parse failed!\n", __FUNCTION__);
            return -1;
        }
        picWidth = val["JpegPictureWithAppendData"]["jpegPicWidth"].asInt();
        picHeight = val["JpegPictureWithAppendData"]["jpegPicHeight"].asInt();
        printf("%s(), parse picwidth: %d, picheight: %d\n", __FUNCTION__, picWidth, picHeight);
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
    }

    char *ContentD = (char*)"octet-stream\r\nContent-Length: ";
	if ((pIndex = strstr(pIndex, ContentD)) == NULL) {
        printf("HIKClient::%s, no data length found!\n", __FUNCTION__);
        return -1;
	}
    pIndex += strlen(ContentD);
    char thLen[16] = {0};
    for (int i = 0; i < 16 && *pIndex != '\r'; i++, pIndex++) {
        thLen[i] = *pIndex;
    }
    dataLen = atoi(thLen);
    printf("HIKClient::%s, json data length: %d\n", __FUNCTION__, dataLen);

    pIndex += 4;
	char *boundaryEnd = (char*)"--boundary--\r\n";
    char *pIndexEnd = buffer + readLen - strlen(boundaryEnd);
	if ((pIndexEnd = strstr(pIndexEnd, boundaryEnd)) == NULL) {
        printf("HIKClient::%s, no end boundary found!\n", __FUNCTION__);
        return -1;
	}
    pIndexEnd -= 2;
    printf("HIKClient::%s, json data get length: %d\n", __FUNCTION__, (int)(pIndexEnd - pIndex));

    printf("HIKClient::%s, data begin - 50:\n", __FUNCTION__);
    for (int i = 0; i < 100; i++) {
       printf("%x ", (unsigned char)pIndex[i]);
    }
    printf("\n");
    printf("HIKClient::%s, data end - 50:\n", __FUNCTION__);
    for (int i = 0; i < 100; i++) {
       printf("%x ", (unsigned char)pIndexEnd[i-100]);
    }
    printf("\n");

    if (dataLen != (int)(pIndexEnd - pIndex)) {
        printf("HIKClient::%s, check length failed!\n", __FUNCTION__);
        return -1;
    }

    FILE *file;
    if ((file = fopen(pFilePath, "w+")) == NULL) {
        printf("HIKClient::%s, open %s failed!\n", __FUNCTION__, pFilePath);
        fclose(file);
    }

    fwrite(&picWidth, 4, 1, file);
    fwrite(&picHeight, 4, 1, file);
    fwrite(&dataLen, 4, 1, file);
    fwrite(pIndex, 1, dataLen, file);

    fclose(file);
    return 0;
}

int HIKClient::config(const char *ip, const char *port, const char *username, const char *password, const char *url, int method)
{
	int res = -1;

	char URL[256] = "http://";
	strcat(URL, ip);
	strcat(URL, ":");
	strcat(URL, port);
	strcat(URL, url);

	UserInfo info = {};
	info.username = (char*)username;
	info.password = (char*)password;

	char *DataResponse = new char[BUFFERSIZE];
	memset(DataResponse, 0, BUFFERSIZE);

	switch (method)
	{
	case GET:
		res = httpGet(URL, DataResponse, &info);
		break;
	case PUT:
	{

		FILE *file;

		if ((file = fopen("./CHttpClient/inboundData/put.txt", "r")) == NULL)
		{
			printf("打开配置文件失败\n");
			fclose(file);
			break;
		}
		fseek(file, 0, SEEK_END);
		int len = ftell(file);

		rewind(file);

		char *inboundData = new char[len + 1];
		memset(inboundData, 0, len + 1);

		fread(inboundData, len, 1, file);
		inboundData[len] = '\0';
		fclose(file);

		res = httpPut(URL, inboundData, DataResponse, &info);

		delete[]inboundData;
		inboundData = NULL;
		break;
	}
	case POST:
	{
		FILE *file;

		if ((file = fopen("./CHttpClient/inboundData/post.txt", "r")) == NULL)
		{
			printf("打开配置文件失败\n");
			fclose(file);
			break;
		}


		fseek(file, 0, SEEK_END);
		int len = ftell(file);

		rewind(file);

		char *inboundData = new char[len + 1];
		memset(inboundData, 0, len + 1);


		fread(inboundData, len, 1, file);

		inboundData[len] = '\0';
		fclose(file);

		res = httpPost(URL, inboundData, DataResponse, &info);

		delete[]inboundData;
		inboundData = NULL;
		break;
	}
	case DELETE:
		res = httpDelete(URL, DataResponse, &info);
		break;
	default:
		break;
	}

	if (res == 0)
	{
        // printf("%s\n", DataResponse);
        for (int i = 0; i < 200; i++) {
           printf("%c", DataResponse[i]);
        }
        printf("\n");
        for (int i = 0; i < 200; i++) {
           printf("%x ", (unsigned char)DataResponse[i]);
        }
        printf("\n");
	}
	else if (res > 0)
	{
		printf("curl_easy_perform failed:%s\n", curl_easy_strerror((CURLcode)res));
	}

	delete[]DataResponse;
	DataResponse = NULL;
    return res;
}

int HIKClient::getStatusData(char* pData, int size)
{
    char url[128] = "/ISAPI/PTZCtrl/channels/1/status";
    char URL[256] = "http://";
    strcat(URL, m_ip);
    strcat(URL, ":");
    strcat(URL, m_port);
    strcat(URL, url);

    UserInfo info = {};
    info.username = m_username;
    info.password = m_password;

    if (pData == NULL) {
        return -1;
    }
    FILE *file;
    if ((file = fopen("./.status_buffer", "w+")) == NULL) {
        printf("HIKClient::%s, open status_buffer failed!\n", __FUNCTION__);
        return -1;
    }
    fclose(file);
    if ((file = fopen("./.status_buffer", "a+")) == NULL) {
        printf("HIKClient::%s, open status_buffer failed!\n", __FUNCTION__);
        return -1;
    }
    CURLcode res = httpGetBin(URL, file, &info);
    fclose(file);
    int readLen = 0;
    if (res == 0) {
        if ((file = fopen("./.status_buffer", "r")) == NULL) {
            printf("HIKClient::%s, open status_buffer failed!\n", __FUNCTION__);
            return -1;
        }
        readLen = fread(pData, 1, size, file);
        printf("HIKClient::%s, size: %d, status_buffer readLen: %d!\n", __FUNCTION__, size, readLen);
        if (readLen <= 0) {
            printf("HIKClient::%s, read status_buffer failed!\n", __FUNCTION__);
            fclose(file);
            return -1;
        }
        fclose(file);
/*         for (int i = 0; i < 400; i++) {
           printf("%c", (unsigned char)pData[i]);
        }
        printf("\n"); */
    }
    else {
        printf("curl_easy_perform failed:%s\n", curl_easy_strerror((CURLcode)res));
    }

    return readLen;
}

int HIKClient::getStatus(int* pData)
{
    int nRet = 0;
    const int BUF_LEN = 1024;
    char buffer[BUF_LEN] = "";
    memset(buffer, 0, BUF_LEN);
    int readLen = 0;
    if ((readLen = getStatusData(buffer, BUF_LEN)) < 0) {
        return -1;
    }
    if (pData == NULL) {
        return -1;
    } 

    char *axi_X = (char*)"azimuth";
    char *axi_Y = (char*)"elevation";
    char *zoom = (char*)"absoluteZoom";
    pData[0] = xmlInterpret(buffer, axi_X);
    pData[1] = xmlInterpret(buffer, axi_Y);
    pData[2] = xmlInterpret(buffer, zoom);

    return 0;
}

int HIKClient::getMinMaxTempData(char* pData, int size)
{
    char url[128] = "/ISAPI/Thermal/channels/2/thermometry/1/rulesTemperatureInfo?format=json";
    char URL[256] = "http://";
    strcat(URL, m_ip);
    strcat(URL, ":");
    strcat(URL, m_port);
    strcat(URL, url);

    UserInfo info = {};
    info.username = m_username;
    info.password = m_password;

    if (pData == NULL) {
        return -1;
    }
    FILE *file;
    if ((file = fopen("./.minmax_buffer", "w+")) == NULL) {
        printf("HIKClient::%s, open minmax_buffer failed!\n", __FUNCTION__);
        return -1;
    }
    fclose(file);
    if ((file = fopen("./.minmax_buffer", "a+")) == NULL) {
        printf("HIKClient::%s, open minmax_buffer failed!\n", __FUNCTION__);
        return -1;
    }
    CURLcode res = httpGetBin(URL, file, &info);
    fclose(file);
    int readLen = 0;
    if (res == 0) {
        if ((file = fopen("./.minmax_buffer", "r")) == NULL) {
            printf("HIKClient::%s, open minmax_buffer failed!\n", __FUNCTION__);
            return -1;
        }
        readLen = fread(pData, 1, size, file);
        printf("HIKClient::%s, size: %d, minmax_buffer readLen: %d!\n", __FUNCTION__, size, readLen);
        if (readLen <= 0) {
            printf("HIKClient::%s, read minmax_buffer failed!\n", __FUNCTION__);
            fclose(file);
            return -1;
        }
        fclose(file);
        for (int i = 0; i < 400; i++) {
           printf("%c", (unsigned char)pData[i]);
        }
        printf("\n");
    }
    else {
        printf("curl_easy_perform failed:%s\n", curl_easy_strerror((CURLcode)res));
    }

    return readLen;
}

int HIKClient::getMinMaxTemp()
{
    const int BUF_LEN = 1024;
    char buffer[BUF_LEN] = "";
    memset(buffer, 0, BUF_LEN);
    int readLen = 0;
    if ((readLen = getMinMaxTempData(buffer, BUF_LEN)) < 0) {
        return -1;
    }

    Json::Reader reader;
    Json::Value val;
    std::string sJson = buffer;
    if (reader.parse(sJson, val, false)) {
        if (val["ThermometryRulesTemperatureInfoList"].isNull() ||
            val["ThermometryRulesTemperatureInfoList"]["ThermometryRulesTemperatureInfo"].isNull() ||
            val["ThermometryRulesTemperatureInfoList"]["ThermometryRulesTemperatureInfo"][0]["maxTemperature"].isNull() ||
            val["ThermometryRulesTemperatureInfoList"]["ThermometryRulesTemperatureInfo"][0]["minTemperature"].isNull()){
            printf("%s(), appenddata parse failed!\n", __FUNCTION__);
            return -1;
        }
        m_mintemp = val["ThermometryRulesTemperatureInfoList"]["ThermometryRulesTemperatureInfo"][0]["minTemperature"].asFloat();
        m_maxtemp = val["ThermometryRulesTemperatureInfoList"]["ThermometryRulesTemperatureInfo"][0]["maxTemperature"].asFloat();
        printf("%s(), parse picwidth: %f, picheight: %f\n", __FUNCTION__, m_mintemp, m_maxtemp);
    }
    else {
        printf("%s(), parse failed!\n", __FUNCTION__);
        return -1;
    }
    return 0;
}

float HIKClient::getMinTemp()
{
    return m_mintemp;
}

float HIKClient::getMaxTemp()
{
    return m_maxtemp;
}

