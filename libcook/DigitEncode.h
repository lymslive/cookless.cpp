#ifndef DIGITENCODE_H__
#define DIGITENCODE_H__
#include "namespace.h"
#include <string>

// Section: MD5
SPACE_BEGIN

/* POINTER defines a generic pointer type */
typedef unsigned char * POINTER;

/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef unsigned int UINT4;

/* MD5 context. */
typedef struct {
	UINT4 state[4];						/* state (ABCD) */
	UINT4 count[2];						/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];			/* input buffer */
} MD5_CTX;

void MD5Init(MD5_CTX *);
void MD5Update(MD5_CTX *, unsigned char *, unsigned int);
void MD5Final(unsigned char [16], MD5_CTX *);
SPACE_END

// Section: BCD
SPACE_BEGIN

//dest will end by '\0', maxDestLength include the '\0'
int BCD_Encode(const unsigned char *source, int sourceLength, char *dest, int maxDestLength, int *resultLength);

//dest will end by '\0', maxDestLength include the '\0'
int BCD_Encode_LowChar(const unsigned char *source, int sourceLength, char *dest, int maxDestLength, int *resultLength);

//dest will not end by '\0', because the dest is binary
int BCD_Decode(const char *source, int sourceLength, unsigned char *dest, int maxDestLength, int *resultLength);
SPACE_END

// Section: Base64
SPACE_BEGIN
/*-------------------------------------------------------------
  功能：将一块数据编码成BASE64
  参数：input ---待编码的数据
  input_len --- 待编码的数据块长度。
  output --- 用来存放结果的缓冲区
  output_siz ---缓冲区output的长度
  result_length --- 编码后的数据块长度。
  注：编码后的数据长度为：((input_len+2)/3)*4  , output_size >= ((input_len+2)/3)*4+1.
  ---------------------------------------------------------------*/
bool Base64_Encode(const unsigned char* input, int input_len, char* output, int output_size, int* result_length);

/*-------------------------------------------------------------
  功能：将一块BASE64编码的数据解码
  参数：input ---待解码的数据
  格式：
  input_len --- 待解码的数据块长度。
  output --- 用来存放解码后数据的缓冲区
  output_size ---缓冲区output的长度
  result_length --- 解码后的数据块长度。
  返回值：0 成功
  -1失败
  注：由于每个Group中可能含有1至3字节。 所以解码后的数据长度不一定，
  如果每个Group中都有3个字节数据，则解码后的数据长度为：
  (input_len*3)/4.
  ---------------------------------------------------------------*/
bool Base64_Decode(const char* input, int input_len, unsigned char* output, int output_size, int* result_length);
SPACE_END

// Section: Util
SPACE_BEGIN
std::string md5sum(const std::string& data);
SPACE_END

// Section: 
SPACE_BEGIN
SPACE_END

#endif /* end of include guard: DIGITENCODE_H__ */
