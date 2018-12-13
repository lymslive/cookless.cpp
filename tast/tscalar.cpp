#include "telib/tscalar.hpp"
#include "telib/toperator.hpp"
#include "utast.hpp"

TAST(CScalar, Ctor)
{
	H1("对象构造方法");
	const char* pLet14 = "abcdefghijklmn";
	const char* pLet26 = "abcdefghijklmnopqrstuvwxyz";
	const char* pHex = "0123456789abcdef";

	H2("从不同大小的字符串指针创建");
	utd::CScalar sc1 = pLet14;
	utd::CScalar sc2 = pHex;
	utd::CScalar sc3(pLet26);
	utd::CScalar sc4(pLet26, true);
	utd::CScalar sc5(pLet26, pLet26 + 26);
	DISP(sc1);
	DISP(sc2);
	DISP(sc3);
	DISP(sc4);
	DISP(sc5);

	NOTE("小于256字符串，头部长度容量各1字节，尾部1字节，总大小圆整至8字节");
	SEE(sc2.capacity() >= sc2.length(), true);
	SEE(sc2.capacity()+3 == sc2.block(), true);
	SEE(sc2.block() % 8 == 0, true);
	SEE(sc3.capacity() >= sc3.length(), true);
	SEE(sc3.capacity()+3 == sc3.block(), true);
	SEE(sc3.block() % 8 == 0, true);

	SEE(sc1 < sc2, true);
	SEE(sc2 < sc3, true);
	SEE(sc3 == sc4, true);
	SEE(sc3 == sc5, true);

#if 1
	H2("默认复制构造函数");
	utd::CScalar sc11(sc1);
	DISP(sc11);
	SEE(sc11.c_str() != sc1.c_str(), true);

	utd::CScalar sc12(sc2);
	DISP(sc12);
	SEE(sc12.c_str() != sc2.c_str(), true);

	utd::CScalar sc13(sc3);
	DISP(sc13);
	SEE(sc13.c_str() != sc3.c_str(), true);

	NOTE("只有 stable 字符串才只复制指针，其他复制对象都有另外的空间");
	utd::CScalar sc14(sc4);
	DISP(sc14);
	SEE(sc14.c_str() == sc4.c_str(), true);

	utd::CScalar sc15(sc5);
	DISP(sc15);
	SEE(sc15.c_str() != sc5.c_str(), true);
#endif

	H2("默认赋值运算符");
	NOTE("先构造个空对象");
	utd::CScalar sc;
	DISP(sc);

	SEE(sc.length(), 0);
	SEE(sc.empty(), true);
	const char* pOld = sc.c_str();

	NOTE("赋与短字符串，仍在栈空间");
	sc = sc1;
	DISP(sc);
	SEE(sc.c_str() != sc1.c_str(), true);
	SEE(sc.c_str() == pOld, true);

	NOTE("赋与长字符串，除 stable 字符串外，要分配另外空间");
	sc = sc2;
	DISP(sc);
	SEE(sc.c_str() != sc2.c_str(), true);
	SEE(sc.c_str() != pOld, true);

	sc = sc3;
	DISP(sc);
	SEE(sc.c_str() != sc3.c_str(), true);
	SEE(sc.c_str() != pOld, true);

	sc = sc4;
	DISP(sc);
	SEE(sc.c_str() == sc4.c_str(), true);
	SEE(sc.c_str() != pOld, true);

	sc = sc5;
	DISP(sc);
	SEE(sc.c_str() != sc5.c_str(), true);
	SEE(sc.c_str() != pOld, true);

}

TAST(CScalar, Number)
{
	H1("将 CScalar 当作数据类型使用");
	int i = 1;
	uint32_t u = 2;
	double d = 3.14;
	int64_t iL = 4;
	uint64_t uL = 5;

	H2("创建数字标量，须调用显式构造函数");
	// utd::CScalar sc1 = i;
	utd::CScalar sc1(i);
	utd::CScalar sc2(u);
	utd::CScalar sc3(d);
	utd::CScalar sc4(iL);
	utd::CScalar sc5(uL);

	DISP(sc1);
	DISP(sc2);
	DISP(sc3);
	DISP(sc4);
	DISP(sc5);

	H2("比较大小");
	SEE(sc1 < sc2, true);
	SEE(sc2 < sc3, true);
	SEE(sc3 < sc4, true);
	SEE(sc4 < sc5, true);

	SEE(sc1 <= sc2, true);
	SEE(sc1 >= sc2, false);
	SEE(sc1 > sc2, false);
	SEE(sc1 == sc2, false);
	SEE(sc1 != sc2, true);

	H2("显式转回数字参与运算");
	SEE(int(sc1) + int(sc2), 3);
	SEE(int(sc4) + int(sc5), 9);
	SEE(int(sc1) + int(sc2) + int(sc4) + int(sc5), 12);
	SEE(int(sc1) + int(sc2) == int(sc3), true);
	SEE(uint32_t(sc4) * uint32_t(sc5), 20);
	SEE(uint32_t(sc4) / uint32_t(sc2), 2);
	SEE(uint32_t(sc5) / uint32_t(sc2), 2);
	SEE(double(sc1) + double(sc2) < double(sc3), true);
	SEE(double(sc3) + double(sc4));

}

TAST(CScalar, String)
{
	H1("字符串操作");
	HEAD(1, "字符串操作");
	const char* pHex = "0123456789abcdef";
	const char* pLet = "abcdefg";

	HEAD(2, "字符串修改")
	{
		utd::CScalar sc(pHex, true);
		DISP(sc);

		NOTE("先申请一个静态字符串，未申请自己空间");
		SEE(sc.islocal(), false);
		SEE(sc.isstable(), true);
		SEE(sc.isalloc(), false);

		NOTE("对常量对象只读索引取值不会影响对象内存状态");
		const utd::CScalar& sconst = sc;
		const utd::CScalar* pconst = &sc;
		SEE(sconst[0] == '0', true);
		SEE(sconst.isstable(), true);
		SEE(sconst.isalloc(), false);
		SEE((*pconst)[0] == '0', true);
		SEE(pconst->isstable(), true);
		SEE(pconst->isalloc(), false);

		NOTE("对非常量索引取值就会重新申请内存");
		char ch = sc[0];
		DISP(sc);
		SEE(sc[0] == '0', true);
		SEE(sc.isstable(), false);
		SEE(sc.isalloc(), true);

		NOTE("当然索引赋值也会影响对象内存状态");
		{
			utd::CScalar sc(pHex, true);
			DISP(sc);
			sc[0] = 'F';
			sc[-1] = '0';
			DISP(sc);
			SEE(sc[0] != '0', true);
			SEE(sc.isstable(), false);
			SEE(sc.isalloc(), true);
		}
	}

	HEAD(2, "字符串增长 += 与逆缩减 -=")
	{
		utd::CScalar sc(pHex, true);
		DISP(sc);
		utd::CScalar dc(pHex, true);

		while (sc.length() < 256) {
			sc += pHex;
			DISP(sc);
			sc += dc;
			DISP(sc);
		}
		NOTE("超过 256 字长时，字符串头要用两个字节存储长度、容量");
		SEE(sc.block() % 8 == 0, true);
		SEE(sc.block() == sc.capacity() + 2 + 2 + 1, true);

		NOTE("减法作为删除后缀逆运算，但不会释放已申请的空间");
		const char* pOldStr = sc.c_str();
		size_t nOldLen = sc.length();
		size_t nOldCap = sc.capacity();
		size_t nOldBlock = sc.block();
		while (sc.length() > 0) {
			sc -= pHex;
			DISP(sc);
			sc -= dc;
			DISP(sc);
		}
		SEE(sc.c_str() == pOldStr, true);
		SEE(sc.length() != nOldLen, true);
		SEE(sc.capacity() == nOldCap, true);
		SEE(sc.block() == nOldBlock, true);

		NOTE("短字符串的加减");
		sc = "abcdefg";
		DISP(sc);
		SEE(sc.c_str() != pOldStr, true);
		pOldStr = sc.c_str();
		DISP(sc -= "g");
		SEE(sc.c_str() == pOldStr, true);
		DISP(sc += "g");
		SEE(sc.c_str() == pOldStr, true);
		DISP(sc += pLet);
		SEE(sc.c_str() == pOldStr, true);
		DISP(sc -= pLet);
		SEE(sc.c_str() == pOldStr, true);
		DISP(sc += sc);
		SEE(sc.c_str() == pOldStr, true);
	}
	
	HEAD(2, "字符串比较")
	{
		utd::CScalar sc1 = pLet;
		utd::CScalar sc2 = pHex;
		utd::CScalar sc3(pHex, true);
		DISP(sc1);
		DISP(sc2);
		DISP(sc3);

		NOTE("Scalar 对象只定义 < 与 == 比较，可比较 C 字符串指针或同类对象");
		SEE(sc1 < sc2, true);
		SEE(sc2 < sc1, false);
		SEE(sc1 < pHex, true);
		SEE(sc1 == pLet, true);
		SEE(sc2 == pHex, true);
		SEE(sc2 == sc3, true);

		NOTE("扩展比较符，要 include 通用模板 toperator.hpp");
		SEE(sc1 <= sc2, true);
		SEE(sc1 > sc2, false);
		SEE(sc1 >= sc2, false);
		SEE(sc2 != sc3, false);
		SEE(sc2 <= sc3, true);
		SEE(sc2 >= sc3, true);

		SEE(sc1 <= pHex, true);
		SEE(sc1 >= pHex, false);
		SEE(sc1 > pHex, false);
		SEE(sc1 == pHex, false);
		SEE(sc1 != pHex, true);
	}
}

TAST(CScalar, Operator)
{
	H1("标量的四则运算");
	const char* pHex = "0123456789abcdef";
	const char* pLet = "abcdefg";
	int iValue = -100;
	uint32_t uValue = 100;
	int64_t iLong = -100 * 100;
	uint64_t uLong = 100 * 100;
	double dValue = 3.14159;

	utd::CScalar str1 = pLet; DISP(str1);
	utd::CScalar str2 = pHex; DISP(str2);
	utd::CScalar si(iValue); DISP(si);
	utd::CScalar su(uValue); DISP(su);
	utd::CScalar siL(iLong); DISP(siL);
	utd::CScalar suL(uLong); DISP(suL);
	utd::CScalar sd(dValue); DISP(sd);

	NOTE("字符串加减");
	DISP(str1 + str2);
	DISP(str1 - str2);
	DISP(str2 - str1);

	NOTE("数字加减");
	DISP(si + 1);
	DISP(su + 1);
	DISP(si + su);
	DISP(si - su);
	DISP(su - si);

	SEE(int(su+1) == 101, true);

	DISP(siL + 100);
	DISP(suL + 100);
	DISP(siL + suL);
	DISP(siL - suL);
	DISP(suL - siL);

	NOTE("数字乘除");
	DISP(sd * 2);
	DISP(sd / 2);
	DISP(suL / su);
	DISP(siL / si);
	DISP(suL / si);
	DISP(siL / su);

	NOTE("字符串数字混加减无效果");
	DISP(str1 += si);
	DISP(str1 -= si);
	DISP(su += str2);
	DISP(su -= str2);
	SEE((str1 += si).c_str() == str1.c_str(), true);
	SEE((str2 += si).c_str() == str2.c_str(), true);
	SEE((str1 -= si).c_str() == str1.c_str(), true);
	SEE((str2 -= si).c_str() == str2.c_str(), true);
	SEE((su += str2) == su, true);
	SEE((su -= str2) == su, true);

	NOTE("也支持隐式赋值");
	utd::CScalar sc = 1;
	DISP(sc);
	sc = 2;
	DISP(sc);
	sc = 2.718;
	DISP(sc);
	sc = size_t(100);
	DISP(sc);
	sc = size_t(-1);
	DISP(sc);
}

TAST_MAIN()
{
	DO_TAST(CScalar, Ctor);
	DO_TAST(CScalar, Number);
	DO_TAST(CScalar, String);
	DO_TAST(CScalar, Operator);
	return SEE_RESULT;
}
