#ifndef COOK_CONVERT_CHAR_H
#define COOK_CONVERT_CHAR_H
/*
 * convert character set between utf8 and GB(chinise);
 */

#include "namespace.h"
#include <string>

SPACE_BEGIN

int UTF8toGB2312(const char *pInBuffer, int nInLen, char *pOutBuffer, int nOutLen);
int GB2312toUTF8(const char *pInBuffer, int nInLen, char *pOutBuffer, int nOutLen);

int UTF8toGBK(const char *pInBuffer, int nInLen, char *pOutBuffer, int nOutLen);
int GBKtoUTF8(const char *pInBuffer, int nInLen, char *pOutBuffer, int nOutLen);

int UTF8toGB18030(const char *pInBuffer, int nInLen, char *pOutBuffer, int nOutLen);
int GB18030toUTF8(const char *pInBuffer, int nInLen, char *pOutBuffer, int nOutLen);

std::string toGB(const std::string& utf);
std::string fromGB(const std::string& gb);

SPACE_END

#endif
