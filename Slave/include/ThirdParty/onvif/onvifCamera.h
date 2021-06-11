#include <stdio.h>
#include <stdlib.h>
#ifndef __ONVIF_CAMERA_H__
#define __ONVIF_CAMERA_H__

#ifdef __cplusplus
extern "C" {
#endif
int ONVIF_DetectDeviceByIP(char *ipAddress,char *xAddrs);
int  cb_Snapshot_Infrared(char *deviceXAddr,char * fileName,char *userName, char * password);
int  cb_Snapshot_Visible(char *deviceXAddr,char * fileName,char *userName, char * password);
int  cb_PTZ_GotoPreset(char *deviceXAddr,int         presetIndex,char *userName, char * password);
int  cb_Connect(char *ipAddress,char *xAddrs,char *userName, char * password);
int  cb_SystemReboot(char *deviceXAddr,char *userName, char * password);


#ifdef __cplusplus
}
#endif

#endif
