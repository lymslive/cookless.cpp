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
#define H1(s) _TFreportSection<char,10>(s, 1, '*')
#define H2(s) _TFreportSection<char,10>(s, 2, '-')

// 打印一个对象，要求类型定义了 disp 方法
template <typename T>
void _TFdisp(const char* pVar, T &var)
{
	printf("[DISP] %s = ", pVar);
	var.disp(stdout);
	printf("\n");
}
#define DISP(s) _TFdisp(#s, (s))

#define _TAST_TRUE(b) ((b) ? "true" : "false")
#define _TAST_PASS(p) ((p) ? "[PASS]" : "[FAIL]")
#define println(format, ...) printf(format, ##__VA_ARGS__); printf("\n")

// 串接成 g_f 函数，定义为测试函数
#define TAST(g, f) void g ##_ ## f()
#define DO_TAST(g, f) printf("\n[TAST] %s()\n", #g "_" #f); g ## _ ## f()

// 全局统计变量
struct _STast
{
	int see_count;
	int pass_count;
	int fail_count;

	_STast () : see_count(0), pass_count(0), fail_count(0) {}
};
_STast _g_tast;

// 测试宏，打印表达式及其值，可选输入预期值判断是否通过
#define SEE(exp, ...) _TFseeout(#exp, (exp), ##__VA_ARGS__)
template <typename ResultT>
void _TFseeout(const char* pExp, ResultT vExp)
{
	std::cout << std::boolalpha; // 将 bool 输出 true/false
	std::cout << "[SEE] " << pExp << " [OUT] " << vExp;
	std::cout << std::endl << std::noboolalpha;
	_g_tast.see_count++;
}

template <typename ResultT>
void _TFseeout(const char* pExp, ResultT vExp, ResultT vExpect)
{
	bool bPass = vExp == vExpect;
	std::cout << std::boolalpha;
	std::cout << "[SEE] " << pExp << " [OUT] " << vExp << " " << _TAST_PASS(bPass);
	if (!bPass) {
		std::cout << " [WANT] " << vExpect;
	}
	std::cout << std::endl << std::noboolalpha;
	_g_tast.see_count++;
	if (bPass) {
		_g_tast.pass_count++;
	}
	else {
		_g_tast.fail_count++;
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
	std::cout << "Tast See : " << _g_tast.see_count << std::endl;
	std::cout << "Tast Pass: " << _g_tast.pass_count << std::endl;
	std::cout << "Tast Fail: " << _g_tast.fail_count << std::endl;
	return _g_tast.fail_count;
}

#endif /* end of include guard: UTAST_HPP__ */
