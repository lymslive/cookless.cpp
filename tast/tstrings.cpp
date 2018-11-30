#include "tstrings/tstrings.hpp"
#include <cstdio>

void disp(const char* msg, const utd::CStr& s) 
{
	printf("%s: [0x%x][%d]->%s\n", msg, s.c_str(), s.length(), s.c_str());
}

void disp(const char* msg, const utd::CStrbuf& s) 
{
	printf("%s: [0x%x][%d/%d]->%s\n", msg, s.c_str(), s.length(), s.capacity(), s.c_str());
}

void report_section(const char* msg)
{
	printf("/* *********** %s *********** */\n", msg);
}

#define DISP(s) disp(#s, (s))
#define DISPL(s) displ(#s, &s)
#define TAST(s) report_section(s)

int main(int argc, char *argv[])
{
	TAST("基础构造对象");
	utd::CStr objStr1("Hellow word!");
	utd::CString objString1("Hellow word!!");
	utd::CStrbuf objStrbuf1("Hellow word!!!");

	DISP(objStr1);
	DISP(objString1);
	DISP(objStrbuf1);

	objStrbuf1 << objStr1;
	DISP(objStr1);
	DISP(objStrbuf1);
	printf("objStrbuf1 capacity: %d\n", objStrbuf1.capacity());

	TAST("索引运算符支持");
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

	TAST("加法运算符支持");
	objString1 = "abcxyz";
	DISP(objString1);
	objString1 = objString1 + objString1 + "!!!";
	DISP(objString1);
	objStrbuf1 += objStr1;
	DISP(objStrbuf1);

	return 0;
}
