
#include <stdio.h> 
 
//注意，是双下划线，而不是单下划线  无需额外头文件
//__FILE__ 包含当前程序文件名的字符串 
//__LINE__  表示当前行号的整数 
//__DATE__ 包含当前日期的字符串 
//__STDC__  如果编译器遵循ANSI C标准，它就是个非零值 
//__TIME__ 包含当前时间的字符串 
//__FUNCTION__执行函数
int main()
{
	printf("%s\n",__FILE__);
	printf("%d\n",__LINE__);
	printf("%s\n",__DATE__);
	printf("%s\n",__TIME__);
	printf("%s\n",__FUNCTION__);
}