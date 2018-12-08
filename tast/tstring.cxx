// tstring.hpp 测试

TAST(CStringTast, Usage)
{
	H1("CString 类用法");

	H2("构造几个 CStr 基类对象");
	utd::CStr objStr1 = "abcdefg";
	DISP(objStr1);
	const char* pLit = "abcdefg";
	utd::CStr objStr2(pLit);
	DISP(objStr2);

	NOTE("CStr 对象共享指针");
	SEE(objStr1.c_str() == objStr2.c_str(), true);
	SEE(objStr1 == objStr2, true);

	H2("构造几个 CString 基类对象");
	MARK("CString 对象不共享指针");
	utd::CString objString1 = pLit;
	utd::CString objString2 = objStr1;
	DISP(objString1);
	DISP(objString2);
	SEE(objString1.c_str() == objStr2.c_str(), false);
	SEE(objString1 == objStr2, true);
	SEE(objString1.c_str() == objString2.c_str(), false);
	SEE(objString1 == objString2, true);
	NOTE("隐式转 const char* 指针，可直接流输出 <<");
	SEE(objString1, objString2);

	MARK("CString 对象赋值不共享指针");
	objString2 = objString1;
	SEE(objString1.c_str() == objString2.c_str(), false);
	SEE(objString1 == objString2, true);
	utd::CString objString3(objString1);
	SEE(objString1.c_str() == objString3.c_str(), false);
	SEE(objString1 == objString3, true);

	H2("CString 对象修改：索引运算符");
	NOTE("保存旧指针，也可用 CStr 对象保存");
	const char* pOld3 = objString3.c_str();
	utd::CStr objStr3(objString3);
	SEE(pOld3 == objStr3.c_str());

	NOTE("使用 begin() end() 指针迭代器修改");
	for (char* p = objString3.begin(); p != objString3.end(); ++p)
	{
		*p = toupper(*p);
	}
	DISP(objString3);
	SEE(objString1 == objString3, false);
	SEE(objString1.c_str() == objString3.c_str(), false);
	NOTE("应该不改变字符串空间，只改变字符串内容");
	SEE(objString3.c_str() == objStr3.c_str(), true);

	NOTE("在排序上大写字母比小写字母反而小");
	SEE(objString1 < objString3, false);
	SEE(objString3 < objString1, true);
	utd::CString objString4 = objString3;

	NOTE("使用索引修改");
	for (int i = 0; i < objString3.length(); ++i)
	{
		objString3[i] = tolower(objString3[i]);
	}
	DISP(objString3);
	SEE(objString1 == objString3, true);
	SEE(objString3.c_str() == pOld3, true);

	NOTE("使用负索引修改");
	DISP(objString4);
	int iend = -1;
	for (int n = objString4.length(); n-- > 0; --iend)
	{
		objString4[iend] = tolower(objString4[iend]);
	}
	DISP(objString4);
	SEE(objString1 == objString4, true);
	MARK("-1 至最负的索引？");
	SEE(objString4.length());
	SEE(iend, -1-objString4.length());

	H2("CString 对象修改：加减运算符");

	NOTE("+ 加法不影响操作数，生成另一个字符串");
	utd::CString objString = objString3 + "xyz";
	DISP(objString);
	SEE(objString1 == objString3, true);
	SEE(objString3.c_str() == pOld3, true);
	SEE(objString == objString3, false);
	SEE(objString >= objString3, true);

	NOTE("- 减法去除尾字符，与 + 逆运算");
	const char* pOld = objString.c_str();
	objString -= "xyz";
	DISP(objString);
	SEE(objString == objString3, true);
	SEE(objString.c_str() == pOld, true);

	NOTE("-= 保持空间，- 不会保持空间");
	objString = objString - 'g';
	DISP(objString);
	SEE(objString.c_str() == pOld, false);
	SEE(objString3.length() - objString.length(), 1);

	NOTE("但是 += 会重新申请空间，要存储更多字符");
	pOld = objString.c_str();
	objString += "g";
	DISP(objString);
	SEE(objString.c_str() == pOld, false);
	SEE(objString == objString3, true);

	NOTE("add_suffix() 比 + 运算可额外指定附加的分隔串");
	utd::CString sufString = objString3 + "." + "xyz";
	objString.add_suffix("xyz", ".");
	DISP(objString);
	SEE(objString == sufString, true);

	NOTE("sub_suffix() 也是逆运算");
	objString.sub_suffix("xyz", ".");
	DISP(objString);
	SEE(objString == objString3, true);

	H2("CString 对象修改：乘除运算符");
	objString = objString3 * 3;
	DISP(objString);
	SEE(objString.length() == objString3.length() * 3, true);
	SEE(objString / 3 == objString3, true);

	NOTE("截取一段子串构造 CString");
	const char* pFirst = objString.c_str() + 3;
	const char* pLast = objString.c_str() + 10;
	utd::CString subString(pFirst, pLast);
	DISP(subString);
	SEE(subString.length(), 7);

	NOTE("构造定长等字符 CString");
	utd::CString repString(20, 'x');
	DISP(repString);
	SEE(repString.length(), 20);
	SEE(repString[0], 'x');
	SEE(repString[-1], 'x');

	NOTE("除法就按比例截取前面部分子串");
	repString /= 2;
	DISP(repString);
	SEE(repString.length(), 10);
}

TAST(CStringTast, ModifyAlgo)
{
	H1("一些修改字符串的操作算法");

	H2("逆序字符串");
	{
		utd::CString objString = "abcdefghijklmnopqrstuvwxyz";
		DISP(objString);
		SEE(objString.length(), 26);

		objString.reverse();
		DISP(objString);
		SEE(objString[0], 'z');

		NOTE("减半，13 奇数长度，持续减半");
		utd::CString midString = objString;
		while (midString.length() > 2)
		{
			midString /= 2;
			DISP(midString);
			midString.reverse();
			DISP(midString);
		}
	}

	H2("切分字符串 repart");
	{
		utd::CString objString = "abcdefg--hijklmn--opq,rst--uvw,xyz";
		DISP(objString);

		MARK("repart 是保留前几部分，从头一起的，不是中间一段");
		utd::CString cpyString = objString;
		SEE(cpyString.repart(1, "--"));
		DISP(cpyString);

		cpyString = objString;
		SEE(cpyString.repart(2, "--"));
		DISP(cpyString);

		cpyString = objString;
		SEE(cpyString.repart(3, "--"));
		DISP(cpyString);

		cpyString = objString;
		SEE(cpyString.repart(4, "--"));
		DISP(cpyString);

		cpyString = objString;
		SEE(cpyString.repart(5, "--"));
		DISP(cpyString);

		cpyString = objString;
		SEE(cpyString.repart(2, ","));
		DISP(cpyString);

		cpyString = objString;
		SEE(cpyString.repart(3, ","));
		DISP(cpyString);
	}
}
