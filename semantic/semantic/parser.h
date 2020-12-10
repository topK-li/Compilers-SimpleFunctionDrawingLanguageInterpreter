#ifndef PARSER_H
#define PARSER_H

#include"scanner.h"
typedef double(*FuncPtr)(double);
struct ExprNode                                             //语法树节点类型
{
	enum Token_Type OpCode;                                 //PLUS,MINUS,MUL,DIV,POWER,FUNC,CONST_ID，T
	union
	{
		struct { ExprNode *Left, *Right; }CaseOperater;     //二元运算：只有左右孩子的内部节点
		struct { ExprNode *Child; FuncPtr MathFuncPtr; }CaseFunc;//函数调用：只有一个孩子的内部节点，还有一个指向对应函数名的指针 MathFuncPtr
		double CaseConst;                                   //常数：叶子节点  右值
		double *CaseParmPtr;                                //参数T   左值：存放T的值得地址
	}Content;
};

extern void Parser(char *SrcFilePtr);                       //语法分析器对外接口

#endif #pragma once
#pragma once
