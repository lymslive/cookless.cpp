#include "utast.hpp"

TAST(Group, Name)
{
	NOTE("TAST 宏定义一个测试用例，建议在开始用 H1 输出一行说明标题：");
	H1("标题1");
	NOTE("H2 可输出二级标题，可酌情分隔部分测试逻辑，只定义了两级。");
	H2("标题2");

	NOTE("随后要在 main 函数中用 DO_TAST 调用所定义的测试用例。");
	NOTE("注释说明；随时可输出一行注释，解释测试用例用途。");
	MARK("附加源文件行号的注释说明");

	NOTE("用 SEE 宏查看一个表达式的值");
	SEE(1+2);
	NOTE("在 SEE 宏提供第二参数，可检查表达式的值是否符合预期");
	SEE(1+2, 3);
	SEE(1+2, -3);
	SEE(1+2 == 3, true);
	SEE(1+2 == -3, false);
	MARK("会自动统计 SEE 检查失败个数");
	NOTE("最后在 main 函数中 return SEE_RESULT 可查看统计结果！");
}

TAST_MAIN()
{
	DO_TAST(Group, Name);
	NOTE("TAST 与 DO_TAST 宏不是必须的，按常规函数定义与调用也可");
	NOTE("只不过是参考 gtest 的 TEST 宏风格，有必要时也方便迁移");
	return SEE_RESULT;
}
