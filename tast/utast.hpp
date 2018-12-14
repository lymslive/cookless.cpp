#ifndef UTAST_HPP__
#define UTAST_HPP__
#include <iostream>
#include <cstdio>

// 打印一行报告标题 /* *********** <H1>%s</H1> *********** */
template <typename CharT, int SepLine>
void _TFreportSection(const char* msg, int iHead, CharT ch)
{
	CharT line[SepLine+1];
	int i = SepLine;
	line[i] = CharT(0);
	while (i-->0) line[i] = ch;
	printf("/* %s <H%d>%s</H%d> %s */\n", line, iHead, msg, iHead, line);
}
#define H1(s) _TFreportSection<char,10>(s, 1, '=');
#define H2(s) _TFreportSection<char,10>(s, 2, '-');
// HEAD 可以后面接一对 {} ，实现 if 控制运行，标题级数为 0 就运行这个块
#define HEAD(n, s) if(n>0) {_TFreportSection<char,10>(s, n, '~');} if(n>0)

// 打印一个对象，要求类型定义了 disp 方法
template <typename T>
void _TFdisp(const char* pVar, T &var)
{
	printf("[DISP] %s = ", pVar);
	var.disp(stdout);
	printf("\n");
}
#define DISP(s) _TFdisp(#s, (s))

#define _PASS_COLOR "\e[1;32m"
#define _FAIL_COLOR "\e[1;31m"
#define _NONE_COLOR "\e[0m"
#define _TAST_TRUE(b) ((b) ? "true" : "false")
#define _TAST_PASS(p) ((p) ? (_PASS_COLOR "[PASS]" _NONE_COLOR) : (_FAIL_COLOR "[FAIL]" _NONE_COLOR))

// 打印备注文本
#define println(format, ...) printf(format, ##__VA_ARGS__); printf("\n")
#define NOTE(format, ...) do { printf("[NOTE] "); printf(format, ##__VA_ARGS__); printf("\n"); } while(0);
#define MARK(format, ...) do { printf("[MARK] "); printf(format, ##__VA_ARGS__); printf(" (%s:%d)\n", __FILE__, __LINE__); } while(0);

// 串接成 g_f 函数，定义为测试函数
#define TAST(g, f) void g ##_ ## f()
#define DO_TAST(g, f) printf("\n[TAST] %s()\n", #g "_" #f); g ## _ ## f()
#define TAST_MAIN int main

// 全局统计变量，置于模板函数的静态变量中，便于包含头文件
template <class SizeT>
struct _STast
{
	SizeT see_count;
	SizeT pass_count;
	SizeT fail_count;

	_STast () : see_count(0), pass_count(0), fail_count(0) {}
};
template <class SizeT>
_STast<SizeT>& _TFgetTast()
{
	static _STast<SizeT> st;
	return st;
}
#define _G_TAST _TFgetTast<unsigned int>()

// 测试宏，打印表达式及其值，可选输入预期值判断是否通过
#define SEE(exp, ...) _TFseeout(__FILE__, __LINE__, #exp, (exp), ##__VA_ARGS__)
template <typename ResultT>
void _TFseeout(const char* pFile, int iLine, const char* pExp, ResultT vExp)
{
	std::cout << std::boolalpha; // 将 bool 输出 true/false
	std::cout << "[SEE] " << pExp << " [OUT] " << vExp;
	std::cout << std::endl << std::noboolalpha;
	_G_TAST.see_count++;
}

template <typename ResultT, typename ExpectT>
void _TFseeout(const char* pFile, int iLine, const char* pExp, ResultT vExp, ExpectT vExpect)
{
	bool bPass = vExp == vExpect;
	std::cout << std::boolalpha;
	std::cout << "[SEE] " << pExp << " [OUT] " << vExp << " " << _TAST_PASS(bPass);
	if (!bPass) {
		std::cout << " [WANT] " << vExpect;
		std::cout << " (" << pFile << ":" << iLine << ")";
	}
	std::cout << std::endl << std::noboolalpha;
	_G_TAST.see_count++;
	if (bPass) {
		_G_TAST.pass_count++;
	}
	else {
		_G_TAST.fail_count++;
	}
}

// 测试统计结果
// 若用在 return 之后返回未通过测试数，可用于 main() 的返回值
#define SEE_RESULT _TFseeResult(0)
template <typename T>
int _TFseeResult(T dummy)
{
	std::cout << std::endl;
	H1("测试结果");
	std::cout << "Tast See : " << _G_TAST.see_count << std::endl;
	std::cout << "Tast Pass: " << _G_TAST.pass_count << std::endl;
	std::cout << "Tast Fail: " << _G_TAST.fail_count << std::endl;
	return _G_TAST.fail_count;
}

#endif /* end of include guard: UTAST_HPP__ */
