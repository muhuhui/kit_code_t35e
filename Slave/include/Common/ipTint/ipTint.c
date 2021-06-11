#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <math.h> 
#include "ipTint.h"  
#define LEN 16 
  
  
/** 
 * 字符串转整形 
 */
uint ipTint(const char *ipstr) 
{ 
  if (ipstr == NULL) return 0; 
  
  char *token; 
  uint i = 3, total = 0, cur; 
  
  token = strtok(ipstr, "."); 
  
  while (token != NULL) { 
    cur = atoi(token); 
    if (cur >= 0 && cur <= 255) { 
      total += cur * pow(256, i); 
    } 
    i --; 
    token = strtok(NULL, "."); 
  } 
  
  return total; 
} 
  
/** 
 * 逆置字符串 
 */
void swapStr(char *str, int begin, int end) 
{ 
  int i, j; 
  
  for (i = begin, j = end; i <= j; i ++, j --) { 
    if (str[i] != str[j]) { 
      str[i] = str[i] ^ str[j]; 
      str[j] = str[i] ^ str[j]; 
      str[i] = str[i] ^ str[j]; 
    } 
  } 
} 
  
/** 
 * 整形转ip字符串 
 */
void ipTstr(uint ipint,char * ipStr) 
{ 
 
	memset(ipStr, '\0', LEN);
	ipStr[0] = '.';
  char token[4]; 
  int bt, ed, len, cur; 
  
  while (ipint) { 
    cur = ipint % 256; 
    sprintf(token, "%d", cur); 
	strcat(ipStr, token);
    ipint /= 256; 
	if (ipint) strcat(ipStr, ".");
  } 
  
  len = strlen(ipStr);
  swapStr(ipStr, 0, len - 1);
  
  for (bt = ed = 0; ed < len;) { 
	  while (ed < len && ipStr[ed] != '.') {
      ed ++; 
    } 
	  swapStr(ipStr, bt, ed - 1);
    ed += 1; 
    bt = ed; 
  } 
  
  ipStr[len - 1] = '\0'; 
}