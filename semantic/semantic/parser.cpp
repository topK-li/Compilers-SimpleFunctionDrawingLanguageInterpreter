#include"parser.h"
#include"semantic.h"
//#define Tree_trace(x) PrintSyntaxTree(x,1);
#pragma warning(disable: 4996)
#pragma warning(disable: 4703)
double Parameter = 0,
Origin_x = 0, Origin_y = 0,
Scale_x = 1, Scale_y = 1,
Rot_angle = 0;

static Token token;//定义一个记号

				   //辅助函数声明
static void FetchToken();//调用词法分析器的GetToken，把得到的当前记录保存起来。如果得到的记号是非法输入errtoken，则指出一个语法错误
static void MatchToken(enum Token_Type The_Token);//匹配当前记号
static void SyntaxError(int case_of);//处理语法错误的子程序。根据错误的性质打印相关信息并且终止程序运行。错误性质可以根据传参不同来区分：SyntaxError（1）词法错   SyntaxError（2）语法错
static void ErrMsg(unsigned LineNo, char *descrip, char *string);//打印错误信息
static void PrintSyntaxTree(struct ExprNode *root, int indent);//先序遍历打印语法树
															   //非终结符递归子程序声明 有2类
															   //第1类，语法分析，不构造语法树，因此语句的子程序均设计为过程->void类型的函数，非终结符的递归子程序声明
static void Program();//程序
static void Statement();//语句
static void OriginStatement();//Origin语句
static void RotStatement();//Rot语句
static void ScaleStatement();//Scale语句
static void ForStatement();//For语句
						   //第2类，语法分析+构造语法树，因此表达式均设计为返回值为指向语法树节点的指针的函数。
static struct ExprNode *Expression();//表达式、二元加减运算表达式
static struct ExprNode *Term();//乘除运算表达式
static struct ExprNode *Factor();//一元加减运算表达式
								 //把项和因子独立开处理解决了加减号与乘除号的优先级问题。在这几个过程的反复调用中，始终传递fsys变量的值，保证可以在出错的情况下跳过出错的符号，使分析过程得以进行下去。
static struct ExprNode *Component();//幂运算表达式
static struct ExprNode *Atom();//原子表达式

							   //对外接口声明
extern void Parser(char *SrcFilePtr);

//语法树构造函数声明
static struct ExprNode *MakeExprNode(enum Token_Type opcode, ...);

//通过词法分析器接口GetToken获得一个记号
static void FetchToken()
{
	token = GetToken();
	if (token.type == ERRTOKEN) SyntaxError(1); //如果得到的记号是非法输入errtoken，则指出一个语法错误
}

//匹配当前记号
static void MatchToken(enum Token_Type The_Token)
{
	if (token.type != The_Token)
	{
		SyntaxError(2);//若失败，指出语法错误
	}
	FetchToken();//若成功，获取下一个
}

//语法错误处理
static void SyntaxError(int case_of)
{
	switch (case_of)
	{
	case 1: ErrMsg(LineNo, (char*)"错误记号 ", token.lexeme);
		break;
	case 2: ErrMsg(LineNo, (char*)"不是预期记号", token.lexeme);
		break;
	}
}

//打印错误信息
void ErrMsg(unsigned LineNo, char *descrip, char *string)
{
	char msg[256];
	memset(msg, 0, 256);
	sprintf(msg, "Line No %5d:%s %s !", LineNo, descrip, string);

	MessageBoxA(NULL, msg, "error!", MB_OK);

	CloseScanner();
	exit(1);
}

//先序遍历打印语法树，根-左-右
void PrintSyntaxTree(struct ExprNode *root, int indent)
{
	int temp;
	for (temp = 1; temp <= indent; temp++) printf("\t");//缩进
	switch (root->OpCode)
	{
	case PLUS:		printf("%s\n", "+");								break;
	case MINUS:		printf("%s\n", "-");								break;
	case MUL:		printf("%s\n", "*");								break;
	case DIV:		printf("%s\n", "/");								break;
	case POWER:		printf("%s\n", "**");								break;
	case FUNC:		printf("%x\n", root->Content.CaseFunc.MathFuncPtr);	break;
	case CONST_ID:	printf("%f\n", root->Content.CaseConst);			break;
	case T:			printf("%s\n", "T");								break;
	default:		printf("Error Tree Node !\n");						exit(0);
	}
	if (root->OpCode == CONST_ID || root->OpCode == T) //叶子节点返回
		return;//常数和参数只有叶子节点 常数：右值；参数：左值地址
	if (root->OpCode == FUNC)//递归打印一个孩子节点
		PrintSyntaxTree(root->Content.CaseFunc.Child, indent + 1);//函数有孩子节点和指向函数名的指针
	else//递归打印两个孩子节点
	{//二元运算：左右孩子的内部节点
		PrintSyntaxTree(root->Content.CaseOperater.Left, indent + 1);
		PrintSyntaxTree(root->Content.CaseOperater.Right, indent + 1);
	}
}

//绘图语言解释器入口（与主程序的外部接口）
void Parser(char *SrcFilePtr)//语法分析器的入口
{
	if (!InitScanner(SrcFilePtr))//初始化词法分析器
	{
		printf("Open Source File Error !\n"); return;
	}
	FetchToken();//先获得一个记号
	Program();//然后进入Program递归子程序，递归下降分析
	CloseScanner();//关闭词法分析器
	return;
}

//程序Program的递归子程序
static void Program()
{
	while (token.type != NONTOKEN)//记号类型不为空
	{
		Statement();//程序有多个语句
		MatchToken(SEMICO);//直到匹配到分隔符
	}
}

//语句Statment的递归子程序
static void Statement()
{
	switch (token.type)
	{
		//罗列四种语句类型，分别分析
	case ORIGIN: OriginStatement(); break;
	case SCALE: ScaleStatement(); break;
	case ROT:  RotStatement(); break;
	case FOR: ForStatement(); break;
	default: SyntaxError(2); //否则报错
	}
}

//语句OriginStatement的递归子程序
static void OriginStatement(void)
{
	struct ExprNode *tmp;//语法树节点的类型
	MatchToken(ORIGIN);
	MatchToken(IS);
	MatchToken(L_BRACKET);

	tmp = Expression();  //Tree_trace(tmp);

	Origin_x = GetExprValue(tmp);//获取横坐标点平移距离
	DelExprTree(tmp);//删除一棵树

	MatchToken(COMMA);

	tmp = Expression();   //Tree_trace(tmp);

	Origin_y = GetExprValue(tmp);//获取纵坐标点平移距离
	DelExprTree(tmp);//删除一棵树

	MatchToken(R_BRACKET);
}

//语句ScaleStatement的递归子程序
static void ScaleStatement(void)
{
	struct ExprNode *tmp;
	MatchToken(SCALE);
	MatchToken(IS);
	MatchToken(L_BRACKET);

	tmp = Expression();  //Tree_trace(tmp);

	Scale_x = GetExprValue(tmp);//获取横坐标的比例因子
	DelExprTree(tmp);

	MatchToken(COMMA);

	tmp = Expression();    //Tree_trace(tmp);

	Scale_y = GetExprValue(tmp);//获取纵坐标的比例因子
	DelExprTree(tmp);

	MatchToken(R_BRACKET);
}

//语句RotStatement的递归子程序
static void RotStatement(void)
{
	struct ExprNode *tmp;
	MatchToken(ROT);
	MatchToken(IS);

	tmp = Expression();  //Tree_trace(tmp);
	
	Rot_angle = GetExprValue(tmp);//获得旋转角度
	DelExprTree(tmp);
}

//语句ForStatement的递归子程序
//对右部文法符号的展开->遇到终结符号直接匹配，遇到非终结符就调用相应子程序
//ForStatement中唯一的非终结符是Expression，他出现在5个不同位置，分别代表循环的起始、终止、步长、横坐标、纵坐标，需要5个树节点指针保存这5棵语法树
static void ForStatement(void)
{
	//eg:for T from 0 to 2*pi step pi/50 draw (t, -sin(t));
	double Start, End, Step;//绘图起点、终点、步长
	struct ExprNode *start_ptr, *end_ptr, *step_ptr, *x_ptr, *y_ptr;

	MatchToken(FOR);
	MatchToken(T);
	MatchToken(FROM);//eg:for T from

	start_ptr = Expression(); //Tree_trace(start_ptr);//获得参数起点表达式的语法树

	Start = GetExprValue(start_ptr);//计算参数起点表达式的值
	DelExprTree(start_ptr);//释放参数起点语法树所占空间

	MatchToken(TO);

	end_ptr = Expression(); //Tree_trace(end_ptr);//构造参数终点表达式语法树

	End = GetExprValue(end_ptr);//计算参数终点表达式的值
	DelExprTree(end_ptr);//释放参数终点语法树所占空间

	MatchToken(STEP);

	step_ptr = Expression(); //Tree_trace(step_ptr);//构造参数步长表达式语法树

	Step = GetExprValue(step_ptr);//计算参数步长表达式的值
	DelExprTree(step_ptr);//释放参数步长语法树所占空间

	MatchToken(DRAW);
	MatchToken(L_BRACKET);

	x_ptr = Expression(); //Tree_trace(x_ptr);

	MatchToken(COMMA);

	y_ptr = Expression();//Tree_trace(y_ptr);

	MatchToken(R_BRACKET);

	DrawLoop(Start, End, Step, x_ptr, y_ptr); //绘制图形
	DelExprTree(x_ptr);//释放横坐标语法树所占空间
	DelExprTree(y_ptr);//释放纵坐标语法树所占空间
}

//（二元加减运算）表达式Expression的递归子程序，与上边不太相同的是，表达式需要为其构造语法树
//把函数设计为语法树节点的指针，在函数内引进2个语法树节点的指针变量，分别作为Expression左右操作数（Term）的语法树节点指针
//表达式应该是由正负号或无符号开头、由若干个项以加减号连接而成。
static struct ExprNode *Expression()//展开右部，并且构造语法树
{
	struct ExprNode *left, *right;//左右子树节点指针
	Token_Type token_tmp;//当前记号

	left = Term();//分析左操作数得到其语法树，左操作数是一个乘除运算表达式
	while (token.type == PLUS || token.type == MINUS)
	{
		token_tmp = token.type;//当前记号是加/减
		MatchToken(token_tmp);//匹配记号
		right = Term();//分析右操作数得到其语法树，右操作数是一个乘除运算表达式
		left = MakeExprNode(token_tmp, left, right);//构造运算的语法树，结果为左子树
	}
	return left;//返回的是当前记号节点
}

//乘除运算表达式Term的递归子程序
//项是由若干个因子以乘除号连接而成
static struct ExprNode *Term()
{
	struct ExprNode *left, *right;
	Token_Type token_tmp;
	left = Factor();
	while (token.type == MUL || token.type == DIV)
	{
		token_tmp = token.type;
		MatchToken(token_tmp);
		right = Factor();
		left = MakeExprNode(token_tmp, left, right);
	}
	return left;
}

//一元加减运算Factor的递归子程序
//因子则可能是一个标识符或一个数字，或是一个以括号括起来的子表达式
static struct ExprNode *Factor()
{
	struct ExprNode *left, *right;
	if (token.type == PLUS)//匹配一元加运算
	{
		MatchToken(PLUS);
		right = Factor();
	}
	else if (token.type == MINUS)//表达式退化为仅有右操作数的表达式
	{
		MatchToken(MINUS);
		right = Factor();
		left = new ExprNode;
		left->OpCode = CONST_ID;
		left->Content.CaseConst = 0.0;
		right = MakeExprNode(MINUS, left, right);
	}
	else right = Component();//匹配非终结符Component
	return right;
}

//幂运算表达式Component的递归子程序
static struct ExprNode *Component()//右结合
{
	struct ExprNode *left, *right;
	left = Atom();
	if (token.type == POWER)
	{
		MatchToken(POWER);
		right = Component();//递归调用Component以实现POWER的右结合
		left = MakeExprNode(POWER, left, right);
	}
	return left;
}

//原子表达式Atom的递归子程序，包括分隔符 函数 常数 参数
static struct ExprNode *Atom()
{
	struct Token t = token;
	struct ExprNode *address, *tmp;
	switch (token.type)
	{
	case CONST_ID:
		MatchToken(CONST_ID);
		address = MakeExprNode(CONST_ID, t.value);
		break;
	case T:
		MatchToken(T);
		address = MakeExprNode(T);
		break;
	case FUNC:
		MatchToken(FUNC);
		MatchToken(L_BRACKET);
		tmp = Expression();  //Tree_trace(tmp);
		address = MakeExprNode(FUNC, t.FuncPtr, tmp);
		MatchToken(R_BRACKET);
		break;
	case L_BRACKET:
		MatchToken(L_BRACKET);
		address = Expression(); // Tree_trace(address);
		MatchToken(R_BRACKET);
		break;
	default:
		SyntaxError(2);
	}
	return address;
}

//生成语法树的一个节点，接收可变参数列表
static struct ExprNode * MakeExprNode(enum Token_Type opcode, ...)//注意这个函数是一个可变参数的函数
{
	struct ExprNode *ExprPtr = new(struct ExprNode);//为新节点开辟空间
	ExprPtr->OpCode = opcode;//向节点写入记号类别
	va_list ArgPtr;//指向可变函数的参数的指针
	va_start(ArgPtr, opcode);//初始化va_list变量，第一个参数也就是固定参数为opcode
	switch (opcode)//根据记号的类别构造不同的节点
	{
	case CONST_ID://常数节点
		ExprPtr->Content.CaseConst = (double)va_arg(ArgPtr, double);//返回可变参数，可变参数类型是常数
		break;
	case T://参数T节点
		ExprPtr->Content.CaseParmPtr = &Parameter;//返回可变参数，可变参数类型是参数T
		break;
	case FUNC://函数调用节点
		ExprPtr->Content.CaseFunc.MathFuncPtr = (FuncPtr)va_arg(ArgPtr, FuncPtr);//可变参数类型是对应函数的指针
		ExprPtr->Content.CaseFunc.Child = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);//可变参数类型是节点
		break;
	default://二元运算节点
		ExprPtr->Content.CaseOperater.Left = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);//可变参数类型是节点
		ExprPtr->Content.CaseOperater.Right = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);//同上
		break;
	}
	va_end(ArgPtr);//结束可变参数的获取

	return ExprPtr;
}