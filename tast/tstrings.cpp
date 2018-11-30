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

#define DISP(s) disp(#s, s)
#define TAST(s) report_section(s)

int main(int argc, char *argv[])
{
	utd::CStr objStr1("Hellow word!");
	utd::CString objString1("Hellow word!!");
	utd::CStrbuf objStrbuf1("Hellow word!!!");

	DISP(objStr1);
	DISP(objString1);
	DISP(objStrbuf1);
	return 0;
}
