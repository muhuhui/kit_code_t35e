#ifndef _IPTINT_H_
#define _IPTINT_H_

#ifdef __cplusplus
extern "C" {

#endif
	typedef unsigned int uint;
	uint ipTint(const char *ipstr);
	void ipTstr(uint ipint, char * ipStr);

#ifdef __cplusplus
}
#endif

#endif