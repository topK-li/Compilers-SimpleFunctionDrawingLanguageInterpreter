/*
#include "scanner.h"

int main()
{
	Token token;
	FILE *fp;
	InitScanner("test_scanner.txt");
	printf("记号类型    字符串     常数值     函数指针\n");
	printf("------------------------------------------\n");
	while (1)
	{
		token = GetToken();//获得记号
		if (token.type != NONTOKEN)//打印记号内容
			printf("%4d,%12s,%12f,%12x\n", token.type, token.lexeme, token.value, token.FuncPtr);
		else break;
	}
	printf("------------------------------------------\n");
	CloseScanner();
}
*/