// tstrbuf.hpp 测试

TAST(CStrbufTast, Usage)
{
	H1("CStrbuf 类用法，可完全沿用 CString ");

	H2("构造几个 CStr 基类对象");
	utd::CStr objStr1 = "abcdefg";
	DISP(objStr1);
	const char* pLit = "abcdefg";
	utd::CStr objStr2(pLit);
	DISP(objStr2);

	NOTE("CStr 对象共享指针");
	SEE(objStr1.c_str() == objStr2.c_str(), true);
	SEE(objStr1 == objStr2, true);

	H2("构造几个 CStrbuf 基类对象");
	MARK("CStrbuf 对象不共享指针");
	utd::CStrbuf objStrbuf1 = pLit;
	utd::CStrbuf objStrbuf2 = objStr1;
	DISP(objStrbuf1);
	DISP(objStrbuf2);
	SEE(objStrbuf1.c_str() == objStr2.c_str(), false);
	SEE(objStrbuf1 == objStr2, true);
	SEE(objStrbuf1.c_str() == objStrbuf2.c_str(), false);
	SEE(objStrbuf1 == objStrbuf2, true);
	NOTE("隐式转 const char* 指针，可直接流输出 <<");
	SEE(objStrbuf1, objStrbuf2);

	MARK("CStrbuf 对象赋值不共享指针");
	objStrbuf2 = objStrbuf1;
	SEE(objStrbuf1.c_str() == objStrbuf2.c_str(), false);
	SEE(objStrbuf1 == objStrbuf2, true);
	utd::CStrbuf objStrbuf3(objStrbuf1);
	SEE(objStrbuf1.c_str() == objStrbuf3.c_str(), false);
	SEE(objStrbuf1 == objStrbuf3, true);

	H2("CStrbuf 对象修改：索引运算符");
	NOTE("保存旧指针，也可用 CStr 对象保存");
	const char* pOld3 = objStrbuf3.c_str();
	utd::CStr objStr3(objStrbuf3);
	SEE(pOld3 == objStr3.c_str());

	NOTE("使用 begin() end() 指针迭代器修改");
	for (char* p = objStrbuf3.begin(); p != objStrbuf3.end(); ++p)
	{
		*p = toupper(*p);
	}
	DISP(objStrbuf3);
	SEE(objStrbuf1 == objStrbuf3, false);
	SEE(objStrbuf1.c_str() == objStrbuf3.c_str(), false);
	NOTE("应该不改变字符串空间，只改变字符串内容");
	SEE(objStrbuf3.c_str() == objStr3.c_str(), true);

	NOTE("在排序上大写字母比小写字母反而小");
	SEE(objStrbuf1 < objStrbuf3, false);
	SEE(objStrbuf3 < objStrbuf1, true);
	utd::CStrbuf objStrbuf4 = objStrbuf3;

	NOTE("使用索引修改");
	for (int i = 0; i < objStrbuf3.length(); ++i)
	{
		objStrbuf3[i] = tolower(objStrbuf3[i]);
	}
	DISP(objStrbuf3);
	SEE(objStrbuf1 == objStrbuf3, true);
	SEE(objStrbuf3.c_str() == pOld3, true);

	NOTE("使用负索引修改");
	DISP(objStrbuf4);
	int iend = -1;
	for (int n = objStrbuf4.length(); n-- > 0; --iend)
	{
		objStrbuf4[iend] = tolower(objStrbuf4[iend]);
	}
	DISP(objStrbuf4);
	SEE(objStrbuf1 == objStrbuf4, true);
	MARK("-1 至最负的索引？ (-1-length, -1]");
	SEE(objStrbuf4.length());
	SEE(iend, -1-objStrbuf4.length());

	H2("CStrbuf 对象修改：加减运算符");

	NOTE("+ 加法不影响操作数，生成另一个字符串");
	utd::CStrbuf objStrbuf = objStrbuf3 + "xyz";
	DISP(objStrbuf);
	SEE(objStrbuf1 == objStrbuf3, true);
	SEE(objStrbuf3.c_str() == pOld3, true);
	SEE(objStrbuf == objStrbuf3, false);
	SEE(objStrbuf >= objStrbuf3, true);

	NOTE("- 减法去除尾字符，与 + 逆运算");
	const char* pOld = objStrbuf.c_str();
	objStrbuf -= "xyz";
	DISP(objStrbuf);
	SEE(objStrbuf == objStrbuf3, true);
	SEE(objStrbuf.c_str() == pOld, true);

	NOTE("-= 保持空间，- 不会保持空间");
	objStrbuf = objStrbuf - 'g';
	DISP(objStrbuf);
	SEE(objStrbuf.c_str() == pOld, false);
	SEE(objStrbuf3.length() - objStrbuf.length(), 1);

	NOTE("与 CString 不同， += 也不一定会重新申请空间，因为有一定容量");
	pOld = objStrbuf.c_str();
	objStrbuf += "g";
	DISP(objStrbuf);
	SEE(objStrbuf.c_str() == pOld);
	SEE(objStrbuf == objStrbuf3, true);

	NOTE("add_suffix() 比 + 运算可额外指定附加的分隔串");
	utd::CStrbuf sufStrbuf = objStrbuf3 + "." + "xyz";
	objStrbuf.add_suffix("xyz", ".");
	DISP(objStrbuf);
	SEE(objStrbuf == sufStrbuf, true);

	NOTE("sub_suffix() 也是逆运算");
	objStrbuf.sub_suffix("xyz", ".");
	DISP(objStrbuf);
	SEE(objStrbuf == objStrbuf3, true);

	H2("CStrbuf 对象修改：乘除运算符");
	objStrbuf = objStrbuf3 * 3;
	DISP(objStrbuf);
	SEE(objStrbuf.length() == objStrbuf3.length() * 3, true);
	SEE(objStrbuf / 3 == objStrbuf3, true);

	NOTE("截取一段子串构造 CStrbuf");
	const char* pFirst = objStrbuf.c_str() + 3;
	const char* pLast = objStrbuf.c_str() + 10;
	utd::CStrbuf subStrbuf(pFirst, pLast);
	DISP(subStrbuf);
	SEE(subStrbuf.length(), 7);

	NOTE("构造定长等字符 CStrbuf");
	utd::CStrbuf repStrbuf(20, 'x');
	DISP(repStrbuf);
	SEE(repStrbuf.length(), 20);
	SEE(repStrbuf[0], 'x');
	SEE(repStrbuf[-1], 'x');

	NOTE("除法就按比例截取前面部分子串");
	repStrbuf /= 2;
	DISP(repStrbuf);
	SEE(repStrbuf.length(), 10);
}

TAST(CStrbufTast, Capacity)
{
	H1("测试 CStrbuf 容量相关功能");

	NOTE("构造一定容量的字符串缓冲");
	utd::CStrbuf objStrbuf(8);
	DISP(objStrbuf);
	SEE(objStrbuf.length(), 0);
	SEE(objStrbuf.capacity(), 8);
	SEE(objStrbuf.c_str() != NULL, true);

	NOTE("附加字符串在容量范围内不改变空间");
	const char* pOld = objStrbuf.c_str();
	objStrbuf += "abcdefg";
	DISP(objStrbuf);
	SEE(objStrbuf.c_str() == pOld, true);

	NOTE("用父类保存基类可行？");
	utd::CString objString = objStrbuf;
	DISP(objString);

	NOTE("使用 << 运算符附加字符串，越容应该另申请空间");
	objStrbuf << objString;
	DISP(objStrbuf);
	SEE(objStrbuf.c_str() == pOld, false);

	NOTE("与数字+=相加扩容，但容量有圆整");
	MARK("须得 size_t 类型的数字表容量，裸数字 int 与字符有歧义");
	size_t nOld = objStrbuf.capacity();
	pOld = objStrbuf.c_str();
	objStrbuf += size_t(7);
	DISP(objStrbuf);
	SEE(objStrbuf.capacity() >= nOld + 7, true);
	SEE(objStrbuf.c_str() == pOld, false);

	NOTE("直接申请保留空间");
	pOld = objStrbuf.c_str();
	objStrbuf.reserve(50);
	DISP(objStrbuf);
	SEE(objStrbuf.capacity() >= 50, true);
	SEE(objStrbuf.c_str() == pOld, false);

	NOTE("与数字-=相减扩容");
	nOld = objStrbuf.capacity();
	pOld = objStrbuf.c_str();
	objStrbuf -= size_t(6);
	DISP(objStrbuf);
	SEE(objStrbuf.capacity() >= nOld - 6, true);
	SEE(objStrbuf.c_str() == pOld, false);

	NOTE("缩减到适合字符串");
	nOld = objStrbuf.capacity();
	pOld = objStrbuf.c_str();
	objStrbuf.shrink();
	DISP(objStrbuf);
	SEE(objStrbuf.capacity() == objStrbuf.length(), true);
	SEE(objStrbuf.c_str() == pOld, false);
}
