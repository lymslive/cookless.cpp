#include "DigitEncode.h"
#include "stdio.h"
#include "memory.h"
#include <string.h>
#include <ctype.h>

// Section: MD5
SPACE_BEGIN

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

static void MD5Transform(UINT4 [4], unsigned char [64]);
static void Encode(unsigned char *, UINT4 *, unsigned int);
static void Decode(UINT4 *, unsigned char *, unsigned int);
static void MD5_memcpy(POINTER, POINTER, unsigned int);
static void MD5_memset(POINTER, int, unsigned int);

static unsigned char PADDING[64] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* F, G, H and I are basic MD5 functions.
*/
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
*/
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
   Rotation is separate from addition to prevent recomputation.
   */
#define FF(a, b, c, d, x, s, ac) { \
	(a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define GG(a, b, c, d, x, s, ac) { \
	(a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define HH(a, b, c, d, x, s, ac) { \
	(a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define II(a, b, c, d, x, s, ac) { \
	(a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}

/* MD5 basic transformation. Transforms state based on block.
*/
static void MD5Transform(UINT4 state[], unsigned char block[])
{
	UINT4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

	Decode (x, block, 64);

	/* Round 1 */
	FF ( a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
	FF ( d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
	FF ( c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
	FF ( b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
	FF ( a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
	FF ( d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
	FF ( c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
	FF ( b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
	FF ( a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
	FF ( d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
	FF ( c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF ( b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF ( a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF ( d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF ( c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF ( b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

	/* Round 2 */
	GG ( a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
	GG ( d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
	GG ( c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG ( b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
	GG ( a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
	GG ( d, a, b, c, x[10], S22, 0x2441453); /* 22 */
	GG ( c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG ( b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
	GG ( a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
	GG ( d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG ( c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
	GG ( b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
	GG ( a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG ( d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
	GG ( c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
	GG ( b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

	/* Round 3 */
	HH ( a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
	HH ( d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
	HH ( c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH ( b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH ( a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
	HH ( d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
	HH ( c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
	HH ( b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH ( a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH ( d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
	HH ( c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
	HH ( b, c, d, a, x[ 6], S34, 0x4881d05); /* 44 */
	HH ( a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
	HH ( d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH ( c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH ( b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

	/* Round 4 */
	II ( a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
	II ( d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
	II ( c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II ( b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
	II ( a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II ( d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
	II ( c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II ( b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
	II ( a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
	II ( d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II ( c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
	II ( b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II ( a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
	II ( d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II ( c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
	II ( b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;

	/* Zeroize sensitive information.
	*/
	MD5_memset ((POINTER)x, 0, sizeof (x));
}

/* Encodes input (UINT4) into output (unsigned char). Assumes len is
   a multiple of 4.
   */
static void Encode(unsigned char *output, UINT4 *input, unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[j] = (unsigned char)(input[i] & 0xff);
		output[j+1] = (unsigned char)((input[i] >> 8) & 0xff);
		output[j+2] = (unsigned char)((input[i] >> 16) & 0xff);
		output[j+3] = (unsigned char)((input[i] >> 24) & 0xff);
	}
}

/* Decodes input (unsigned char) into output (UINT4). Assumes len is
   a multiple of 4.
   */
static void Decode(UINT4 *output, unsigned char *input, unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((UINT4)input[j]) | (((UINT4)input[j+1]) << 8) |
			(((UINT4)input[j+2]) << 16) | (((UINT4)input[j+3]) << 24);
}

/* Note: Replace "for loop" with standard memcpy if possible.
*/
static void MD5_memcpy(POINTER output, POINTER input, unsigned int len)
{
	unsigned int i;

	for (i = 0; i < len; i++)
		output[i] = input[i];
}

/* Note: Replace "for loop" with standard memset if possible.
*/
static void MD5_memset (POINTER output, int value, unsigned int len)
{
	unsigned int i;

	for (i = 0; i < len; i++)
		((char *)output)[i] = (char)value;
}

/* MD5 initialization. Begins an MD5 operation, writing a new context.
*/
void MD5Init(MD5_CTX *context)
{
	context->count[0] = context->count[1] = 0;

	/* Load magic initialization constants.
	*/
	context->state[0] = 0x67452301;
	context->state[1] = 0xefcdab89;
	context->state[2] = 0x98badcfe;
	context->state[3] = 0x10325476;
}

/* MD5 block update operation. Continues an MD5 message-digest operation,
   processing another message block, and updating the context.
*/
void MD5Update(MD5_CTX *context,unsigned char *input, unsigned int inputLen)
{
	unsigned int i, index, partLen;

	/* Compute number of bytes mod 64 */
	index = (unsigned int)((context->count[0] >> 3) & 0x3F);

	/* Update number of bits */
	if ((context->count[0] += ((UINT4)inputLen << 3)) < ((UINT4)inputLen << 3))
		context->count[1]++;
	context->count[1] += ((UINT4)inputLen >> 29);

	partLen = 64 - index;

	/* Transform as many times as possible.
	*/
	if (inputLen >= partLen) {
		MD5_memcpy ((POINTER)&context->buffer[index], (POINTER)input, partLen);
		MD5Transform (context->state, context->buffer);

		for (i = partLen; i + 63 < inputLen; i += 64)
			MD5Transform (context->state, &input[i]);

		index = 0;
	}
	else
		i = 0;

	/* Buffer remaining input */
	MD5_memcpy((POINTER)&context->buffer[index], (POINTER)&input[i], inputLen-i);
}

/* MD5 finalization. Ends an MD5 message-digest operation, writing the
   the message digest and zeroizing the context.
   */
void MD5Final(unsigned char digest[],MD5_CTX *context)
{
	unsigned char bits[8];
	unsigned int index, padLen;

	/* Save number of bits */
	Encode (bits, context->count, 8);

	/* Pad out to 56 mod 64.
	*/
	index = (unsigned int)((context->count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	MD5Update (context, PADDING, padLen);

	/* Append length (before padding) */
	MD5Update (context, bits, 8);

	/* Store state in digest */
	Encode (digest, context->state, 16);

	/* Zeroize sensitive information.
	*/
	MD5_memset ((POINTER)context, 0, sizeof (*context));
}

SPACE_END

// Section: BCD
SPACE_BEGIN
int BCD_Encode(const unsigned char *source, int sourceLength, char *dest, int maxDestLength, int *resultLength)
{
	int i, pos, g, l;
	int result = 0;
	const static char *ctable = "0123456789ABCDEF";

	if (resultLength == NULL || maxDestLength <= 0
			|| sourceLength <= 0 || source == NULL || dest == NULL) {
		return -1;
	}
	dest[0] = 0;
	*resultLength = 0;
	pos = 0;
	i = 0;

	while (i<sourceLength) {
		g = source[i] / 16;
		l = source[i] % 16;
		if (pos<maxDestLength-2) {
			dest[pos] = ctable[g];
			dest[pos+1] = ctable[l];
			pos += 2;
		} else {
			result = -2;
			break;
		}
		i++;
	}
	dest[pos] = 0;
	*resultLength = pos;
	return result;
}

int BCD_Encode_LowChar(const unsigned char *source, int sourceLength, char *dest, int maxDestLength, int *resultLength)
{
	int result = BCD_Encode(source, sourceLength, dest, maxDestLength, resultLength);
	if (result != 0)
	{
		return result;
	}

	for (int i = 0; i < *resultLength; ++i)
	{
		dest[i] = tolower(dest[i]);
	}

	return result;
}

int BCD_Decode(const char *source, int sourceLength, unsigned char *dest, int maxDestLength, int *resultLength)
{
	int i, pos;
	int g, l;
	int result = 0;

	if (resultLength == NULL || maxDestLength <= 0
			|| sourceLength <= 0 || source == NULL || dest == NULL) {
		return -1;
	}
	*resultLength = 0;
	pos = 0;
	i = 0;

	while (i < sourceLength && source[i]) {
		char c1 = source[i];
		char c2 = source[i+1];

		g = l = 0;
		if (c1 >= 'A' && c1 <= 'F') {
			g = 10 + (c1 - 'A');
		} else if (c1 >= 'a' && c1 <= 'f') {
			g = 10 + (c1 - 'a');
		} else if (c1 >= '0' && c1 <= '9') {
			g = c1 - '0';
		} else{
			result = -3;
			break;
		}

		if (c2 >= 'A' && c2 <= 'F') {
			l = 10 + (c2 - 'A');
		} else if (c2 >= 'a' && c2 <= 'f') {
			l = 10 + (c2 - 'a');
		} else if (c2 >= '0' && c2 <= '9') {
			l = c2 - '0';
		} else {
			result = -4;
			break;
		}

		if (pos<maxDestLength) {
			dest[pos] = (unsigned char)(g*16+l);
			pos ++;
		} else {
			result = -5;
			break;
		}
		i += 2;
	}
	*resultLength = pos;
	return result;
}
SPACE_END

// Section: Base64
SPACE_BEGIN
#define XX 127
/*
 * Table for decoding hexadecimal in quoted-printable
 */
static const unsigned char index_hex[256] = {
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	0, 1, 2, 3,  4, 5, 6, 7,  8, 9,XX,XX, XX,XX,XX,XX,
	XX,10,11,12, 13,14,15,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,10,11,12, 13,14,15,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX
};
#define HEXCHAR(c)  (index_hex[(unsigned char)(c)])

/*
 * Table for decoding base64
 */
static const unsigned char index_64[256] = {
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,62, XX,XX,XX,63,
	52,53,54,55, 56,57,58,59, 60,61,XX,XX, XX,XX,XX,XX,
	XX, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14,
	15,16,17,18, 19,20,21,22, 23,24,25,XX, XX,XX,XX,XX,
	XX,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40,
	41,42,43,44, 45,46,47,48, 49,50,51,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX
};
#define CHAR64(c)  (index_64[(unsigned char)(c)])

static const char base64_char[64] = {
	'A','B','C','D',  'E','F','G','H',
	'I','J','K','L',  'M','N','O','P',
	'Q','R','S','T',  'U','V','W','X',
	'Y','Z','a','b',  'c','d','e','f',
	'g','h','i','j',  'k','l','m','n',
	'o','p','q','r',  's','t','u','v',
	'w','x','y','z',  '0','1','2','3',
	'4','5','6','7',  '8','9','+','/'
};

static void Base64EncodeGroup(const unsigned char *input, int grpLen, char* output)
{
	if (grpLen == 3) {
		output[0] = base64_char[ input[0]>>2 ];
		output[1] = base64_char[ ((input[0]&0x03)<<4) + (input[1]>>4) ];
		output[2] = base64_char[ ((input[1]&0x0f)<<2) + (input[2]>>6) ];
		output[3] = base64_char[ input[2]&0x3f ];
	} else if (grpLen == 2) {
		output[0] = base64_char[ input[0]>>2 ];
		output[1] = base64_char[ ((input[0]&0x03)<<4) + (input[1]>>4) ];
		output[2] = base64_char[ ((input[1]&0x0f)<<2) ];
		output[3] = '=';
	} else if (grpLen == 1)	{
		output[0] = base64_char[ input[0]>>2 ];
		output[1] = base64_char[ ((input[0]&0x03)<<4) ];
		output[2] = '=';
		output[3] = '=';
	}
}

static void Base64DecodeGroup(const char input[4], unsigned char output[3], int* pWrite)
{	
	if (input[2] == '=' && input[3] == '=') {
		//this means there 1 byte in the last group.
		output[0] = CHAR64(input[0])<<2;
		output[0] |= CHAR64(input[1])>>4;
		*pWrite = 1;
	} else if (input[3] == '=') {
		//this means there 2 byte in the last group.
		output[0] = CHAR64(input[0])<<2;
		output[0] |= CHAR64(input[1])>>4;
		output[1] = CHAR64(input[1])<<4;
		output[1] |= CHAR64(input[2])>>2;
		*pWrite = 2;
	} else {
		//this means there 3 byte in the last group.
		output[0] = CHAR64(input[0])<<2;
		output[0] |= CHAR64(input[1])>>4;
		output[1] = CHAR64(input[1])<<4;
		output[1] |= CHAR64(input[2])>>2;
		output[2] = CHAR64(input[2])<<6;
		output[2] |= CHAR64(input[3]);
		*pWrite = 3;
	}
}

bool Base64_Encode(const unsigned char* input, int input_len, char* output, int output_size, int* result_length)
{
	int encode_length;
	int grpCount, i;

	if (!output) //output buffer is invalid.
		return false;

	encode_length = ((input_len+2)/3)*4;
	if (output_size < encode_length+1)
		return false;
	*result_length = encode_length;
	grpCount = (input_len+2)/3;
	for (i = 0; i < grpCount; i++) {
		if (i == grpCount-1)
			Base64EncodeGroup(input+i*3, input_len-i*3, output+i*4);
		else
			Base64EncodeGroup(input+i*3, 3, output+i*4);
	}
	output[encode_length] = 0;
	return true;
}


bool Base64_Decode(const char* input, int input_len, unsigned char* output, int output_size, int* result_length)
{
	int i,count;
	int bad_data;
	int written;

	if (!output)   //output buffer is invalid.
		return false;

	if ((input_len%4) != 0)  //the data to be decoding must be 4-bytes align.
		return false;

	*result_length = count = 0;
	while (count < input_len) {
		//Output buffer is insufficient.
		if (*result_length >= output_size)
			return false;

		//Check the input data at first.
		bad_data = 0;
		//for the last group '=' is permitted.
		if (input[count+2] == '=' && input[count+3] == '=') { //this group contain only 1 byte.
			if (CHAR64(input[count]) == XX || CHAR64(input[count+1]) == XX)
				bad_data = 1;
		} else if (input[count+3] == '=') {//this group contain 2 bytes.
			if (CHAR64(input[count]) == XX 
					|| CHAR64(input[count+1]) == XX 
					|| CHAR64(input[count+2]) == XX)
				bad_data = 1;
		} else {
			//this group contain 3 bytes.
			for (i = 0; i < 4 && !bad_data; i++) {
				if (CHAR64(input[count+i]) == XX || input[count+i] == '=')
					bad_data = 1;
			}
		}
		if (bad_data)
			return false;

		//Source data is correct, decode this group.
		Base64DecodeGroup(input+count, (unsigned char*)output + *result_length, &written);
		*result_length += written;
		count += 4;
	}
	return true;
}
SPACE_END

// Section: Util
SPACE_BEGIN
std::string md5sum(const std::string& data)
{
	unsigned char md5value[16];
	MD5_CTX md5ctx;
	MD5Init(&md5ctx);
	MD5Update(&md5ctx, (unsigned char*)data.c_str(), data.length());
	MD5Final(md5value, &md5ctx);

	char md5hex[33];
	int iLen = 0;
	BCD_Encode(md5value, 16, md5hex, 33, &iLen);

	return std::string(md5hex, iLen);
}
SPACE_END
