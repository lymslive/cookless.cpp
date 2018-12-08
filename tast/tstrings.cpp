// #include "tstrings/tstrings.hpp"
#include "utast.hpp"
#include "telib/tstrbuf.hpp"
#include <cstdio>
#include <cctype>

// 直接引入独立编写的测试用例 *.cxx
#include "tstr.cxx"
#include "tstring.cxx"
#include "tstrbuf.cxx"

int predebug()
{
	H1("BUG 调试");
	utd::CStr objStr(NULL);
	DISP(objStr);
	return 0;
}

int main(int argc, char *argv[])
{
	// predebug();
	DO_TAST(TStrsTast, Ctor);
	DO_TAST(CStrTast, Usage);
	DO_TAST(CStringTast, Usage);
	DO_TAST(CStrbufTast, Usage);

	return SEE_RESULT;
}

// VMAKE: g++ -g -I .. tstrings.cpp
