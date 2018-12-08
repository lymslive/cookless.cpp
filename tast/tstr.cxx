// tstr.hpp 测试

TAST(TStrsTast, Ctor)
{
	H1("基础构造对象");

	H2("从字面字符串构造对象");
	const char* pLit1 = "Hello Word!";
	utd::CStr objStr1(pLit1);
	utd::CString objString1(pLit1);
	utd::CStrbuf objStrbuf1(pLit1);
	DISP(objStr1);
	DISP(objString1);
	DISP(objStrbuf1);

	H2("从低层字符串类构造对象");
	utd::CString objString2(objStr1);
	utd::CStrbuf objStrbuf2(objStr1);
	utd::CStrbuf objStrbuf3(objString1);
	DISP(objString2);
	DISP(objStrbuf2);
	DISP(objStrbuf3);

	H2("默认空构造对象");
	utd::CStr objStr0;
	utd::CString objString0;
	utd::CStrbuf objStrbuf0;
	DISP(objStr0);
	DISP(objString0);
	DISP(objStrbuf0);

	H2("拷贝构造函数()");
	utd::CStr objStr11(objStr1);
	utd::CString objString11(objString1);
	utd::CStrbuf objStrbuf11(objStrbuf1);
	DISP(objStr11);
	DISP(objString11);
	DISP(objStrbuf11);

	H2("拷贝构造函数=");
	utd::CStr objStr12 = objStr1;
	utd::CString objString12 = objString1;
	utd::CStrbuf objStrbuf12 = objStrbuf1;
	DISP(objStr12);
	DISP(objString12);
	DISP(objStrbuf12);
}

TAST(CStrTast, Usage)
{
	H1("CStr 类用法");
	utd::CStr str = "abcdefg";
	DISP(str);

	H2("索引与负索引");
	println("index str[0]=%c, [-1]=%c, [6]=%c", str[0], str[6], str[-1]);
	char charray[8];
	for (int i = 0; i < str.length(); ++i)
	{
		printf("%c", str[i]+7);
		charray[i] = str[i] + 7;
	}
	charray[7] = char(0);
	printf("\n"); // println() 宏不传参报错

	H2("c_str() -- c_end() 迭代器");
	for (const char* p = str.c_str(); p != str.c_end(); ++p)
	{
		printf("%c", toupper(*p));
	}
	printf("\n");

	H2("字符串比较");
	utd::CStr str2 = charray;
	DISP(str2);
	SEE(str < str2, true);
	SEE(str > str2, false);
	SEE(str <= str2, true);
	SEE(str >= str2, false);
	SEE(str == str2, false);
	SEE(str != str2, true);

	H2("与字面字符串比较");
	SEE(str == "abcdefg", true);
	SEE(str == "abcdef", false);
	SEE(str == "abcdefh", false);
	SEE(str > "abcdef", true);
	SEE(str < "abcdefh", true);

	H2("前后缀判断");
	SEE(str.has_prefix("ab"), true);
	SEE(str.has_prefix("AB"), false);
	SEE(str.has_suffix("g"), true);
	SEE(str.has_suffix("G"), false);
}

