#include "CharEncode.h"
#include <iconv.h>
#include <string.h>

static int code_convert(const char* pFromCharset, const char* pToCharset, const char* pInBuffer, size_t nInLen, char* pOutBuffer, size_t nOutLen)
{
	iconv_t cd;
	char** ppIn = const_cast<char**>(&pInBuffer);
	char** ppOut = &pOutBuffer;
	cd = iconv_open(pToCharset, pFromCharset);
	if (cd == (iconv_t)(-1)) {
		return -1;
	}
	memset(pOutBuffer, 0, nOutLen);
	int cap = nOutLen;
	int nResult = iconv(cd, ppIn, &nInLen, ppOut, &nOutLen);
	if (nResult>=0){
		nResult = cap - nOutLen;
	}
	iconv_close(cd);
	return nResult;
}

SPACE_BEGIN
int UTF8toGBK(const char *pInBuffer, int nInLen, char *pOutBuffer, int nOutLen)
{
	return code_convert("utf-8", "gbk", pInBuffer, nInLen, pOutBuffer, nOutLen);
}

int UTF8toGB18030(const char *pInBuffer, int nInLen, char *pOutBuffer, int nOutLen)
{
	return code_convert("utf-8", "gb18030", pInBuffer, nInLen, pOutBuffer, nOutLen);
}

int GB18030toUTF8(const char *pInBuffer, int nInLen, char *pOutBuffer, int nOutLen)
{
	return code_convert("gb18030","utf-8", pInBuffer, nInLen, pOutBuffer, nOutLen);
}

int UTF8toGB2312(const char *pInBuffer, int nInLen, char *pOutBuffer, int nOutLen)
{
	return code_convert("utf-8", "gb2312", pInBuffer, nInLen, pOutBuffer, nOutLen);
}

int GBKtoUTF8(const char *pInBuffer, int nInLen, char *pOutBuffer, int nOutLen)
{
	return code_convert("gbk", "utf-8", pInBuffer, nInLen, pOutBuffer, nOutLen);
}

int GB2312toUTF8(const char *pInBuffer, int nInLen, char *pOutBuffer, int nOutLen)
{
	return code_convert("gb2312", "utf-8", pInBuffer, nInLen, pOutBuffer, nOutLen);
}

std::string toGB(const std::string& utf)
{
	std::string gb;
	gb.resize(utf.size());
	int len = UTF8toGB18030(utf.c_str(), utf.size(), (char*)gb.c_str(), gb.size());
	gb.resize(len);
	return gb;
}

std::string fromGB(const std::string& gb)
{
	std::string utf;
	utf.resize(gb.size() * 2);
	int len = GB18030toUTF8(gb.c_str(), gb.size(), (char*)utf.c_str(), utf.size());
	utf.resize(len);
	return gb;
}

SPACE_END
