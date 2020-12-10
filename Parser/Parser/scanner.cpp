#include "scanner.h"
#define TOKEN_LEN 100//记号最大长度
#pragma warning(disable: 4996)

unsigned int LineNo;//源文件行号
static FILE *InFile;//输入文件流
static char TokenBuffer[TOKEN_LEN];//记号字符缓冲

//初始化词法分析器
extern int InitScanner(const char *FileName)
{
	LineNo = 1;//行号
	InFile = fopen(FileName, "r");//只读形式打开文件
	if (InFile != NULL) return 1;//打开成功
	else return 0;//打开失败
}

//关闭词法分析器
extern void CloseScanner(void)
{
	if (InFile != NULL)
		fclose(InFile);//关闭文件
}

//从输入源程序（流）读字符
static char GetChar(void)
{
	char Char = getc(InFile);//逐个字符读
	return toupper(Char);//全部转换为大写，然后return
}

//将预读字符退回源程序（流）
static void BackChar(char Char)
{
	if (Char != EOF)
		ungetc(Char, InFile);
}

//加入字符到记号缓冲区
static void AddCharTokenString(char Char)
{
	int TokenLength = strlen(TokenBuffer);
	if (TokenLength + 1 >= sizeof(TokenBuffer)) return;//数组越界
	TokenBuffer[TokenLength] = Char;//写入字符到缓冲区
	TokenBuffer[TokenLength + 1] = '\0';//加入字符串结束标志“\0”
}

//请空记号缓冲区
static void EmptyTokenString()
{
	memset(TokenBuffer, 0, TOKEN_LEN);
}

//判断记号是否在记号表中
static Token JudgeKeyToken(const char *IDString)
{
	int loop;
	//sizeof(TokenTab)L:表示这个数组一共占了多少字节数;sizeof(TokenTab[0])：表示一个元素所占的字节数,两者相除，表述数组中一共有多少个元素
	for (loop = 0; loop < sizeof(TokenTab) / sizeof(TokenTab[0]); loop++)
	{	//遍历TokenTab表
		if (strcmp(TokenTab[loop].lexeme, IDString) == 0)
			return TokenTab[loop];//判断在字符表中就返回该记号
	}
	Token errortoken;
	memset(&errortoken, 0, sizeof(Token));//先将errortoken置空
	errortoken.type = ERRTOKEN;//然后填入出错记号
	return errortoken;//返回出错记号
}

//获得一个记号
extern Token GetToken(void)
{
	Token token;
	char Char;
	memset(&token, 0, sizeof(Token));//token置空
	EmptyTokenString();//清空记号缓冲区
	token.lexeme = TokenBuffer;
	for (;;)//此循环用来过滤掉源程序中的空格、TAB、回车等分隔符，文件结束返回空记号
	{
		Char = GetChar();//从源程序读字符
		if (Char == EOF)//读出错
		{
			token.type = NONTOKEN;
			return token;
		}
		if (Char == '\n')
			LineNo++;//行号+1
		if (!isspace(Char))//遇到空格该记号肯定已经完成，退出循环
			break;
	}
	AddCharTokenString(Char);//如果不是上面的那些分隔符，就先加入缓冲区
	if (isalpha(Char))//如果是英文字母，一定是函数、关键字、PI、E等
	{
		for (;;)
		{
			Char = GetChar();
			if (isalnum(Char))//如果是字母或数字
				AddCharTokenString(Char);//加入缓冲区
			else break;
		}
		BackChar(Char);//退回缓冲区
		token = JudgeKeyToken(TokenBuffer);//判断是否在记号表中
		token.lexeme = TokenBuffer;
		return token;
	}
	else if (isdigit(Char))//如果是数字，一定是常量
	{
		for (;;)
		{
			Char = GetChar();
			if (isdigit(Char))
				AddCharTokenString(Char);
			else break;
		}
		if (Char == '.')
		{
			AddCharTokenString(Char);
			for (;;)
			{
				Char = GetChar();
				if (isdigit(Char))
					AddCharTokenString(Char);
				else break;
			}
		}
		BackChar(Char);
		token.type = CONST_ID;
		token.value = atof(TokenBuffer);
		return token;
	}
	else//如果是其他符号
	{
		switch (Char)
		{
		case ';': token.type = SEMICO; break;
		case '(': token.type = L_BRACKET; break;
		case ')': token.type = R_BRACKET; break;
		case ',': token.type = COMMA; break;
		case '+': token.type = PLUS; break;
		case '-':
			Char = GetChar();
			if (Char == '-')
			{
				while (Char != '\n' && Char != EOF) Char = GetChar();
				BackChar(Char);
				return GetToken();
			}
			else
			{
				BackChar(Char);
				token.type = MINUS;
				break;
			}
		case '/':
			Char = GetChar();
			if (Char == '/')
			{
				while (Char != '\n' && Char != EOF) Char = GetChar();
				BackChar(Char);
				return GetToken();
			}
			else
			{
				BackChar(Char);
				token.type = DIV;
				break;
			}
		case '*':
			Char = GetChar();
			if (Char == '*')
			{
				token.type = POWER;
				break;
			}
			else
			{
				BackChar(Char);
				token.type = MUL;
				break;
			}
		default:token.type = ERRTOKEN; break;
		}
	}
	return token;
}