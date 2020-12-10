#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"parser.h"
#pragma warning(disable:4996)
extern void Parser( char *SrcFilePtr);
int main(int argc,  char *argv[])
{
	char*p;
	p = (char*)malloc(20 * sizeof(char));
	memset(p, 0, sizeof(char) * 20);
	strcpy(p, "test_parser.txt");
	Parser(p);
	free(p);
	system("pause");
}