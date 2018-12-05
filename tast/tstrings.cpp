// #include "tstrings/tstrings.hpp"
#include "telib/tstrbuf.hpp"
#include <cstdio>

void disp(const char* msg, const utd::CStr& s) 
{
	printf("%s: [0x%x][%d]->%s\n", msg, s.c_str(), s.length(), s.c_str());
}

void disp(const char* msg, const utd::CStrbuf& s) 
{
	printf("%s: [0x%x][%d/%d]->%s\n", msg, s.c_str(), s.length(), s.capacity(), s.c_str());
}

void report_section_h1(const char* msg)
{
	printf("/* *********** %s *********** */\n", msg);
}

void report_section_h2(const char* msg)
{
	printf("/* ----------- %s ----------- */\n", msg);
}

#define DISP(s) disp(#s, (s))
#define DISPL(s) displ(#s, &s)
#define TASTH1(s) report_section_h1(s)
#define TASTH2(s) report_section_h2(s)

int main(int argc, char *argv[])
{
	TASTH1("基础构造对象");

	TASTH2("从字面字符串构造对象");
	const char* pLit1 = "Hello Word!";
	utd::CStr objStr1(pLit1);
	utd::CString objString1(pLit1);
	utd::CStrbuf objStrbuf1(pLit1);
	DISP(objStr1);
	DISP(objString1);
	DISP(objStrbuf1);

	TASTH2("从低层字符串类构造对象");
	utd::CString objString2(objStr1);
	utd::CStrbuf objStrbuf2(objStr1);
	utd::CStrbuf objStrbuf3(objString1);
	DISP(objString2);
	DISP(objStrbuf2);
	DISP(objStrbuf3);

	TASTH2("默认空构造对象");
	utd::CStr objStr0;
	utd::CString objString0;
	utd::CStrbuf objStrbuf0;
	DISP(objStr0);
	DISP(objString0);
	DISP(objStrbuf0);

	TASTH2("拷贝构造函数()");
	utd::CStr objStr11(objStr1);
	utd::CString objString11(objString1);
	utd::CStrbuf objStrbuf11(objStrbuf1);
	DISP(objStr11);
	DISP(objString11);
	DISP(objStrbuf11);

	TASTH2("拷贝构造函数=");
	utd::CStr objStr12 = objStr1;
	utd::CString objString12 = objString1;
	utd::CStrbuf objStrbuf12 = objStrbuf1;
	DISP(objStr12);
	DISP(objString12);
	DISP(objStrbuf12);

	objStrbuf1 << objStr1;
	DISP(objStr1);
	DISP(objStrbuf1);
	printf("objStrbuf1 capacity: %d\n", objStrbuf1.capacity());

	TASTH1("索引运算符支持");
	printf("objStrbuf1[0]: %c; objStrbuf1[-1]: %c\n", objStrbuf1[0], objStrbuf1[-1]);
	objStrbuf1[0] = 'B';
	objStrbuf1[-1] = 'E';
	printf("objStrbuf1[0]: %c; objStrbuf1[-1]: %c\n", objStrbuf1[0], objStrbuf1[-1]);
	DISP(objStrbuf1);

	for (auto it = objString1.begin(); it != objString1.end(); ++it)
	{
		*it = *it + 1;
	}
	DISP(objString1);

	TASTH1("修改运算符");

	TASTH2("加法运算符支持");
	objString1 = "abcxyz";
	DISP(objString1);
	objString1 = objString1 + objString1 + "!!!";
	DISP(objString1);
	objStrbuf1 += objStr1;
	DISP(objStrbuf1);

	return 0;
}
