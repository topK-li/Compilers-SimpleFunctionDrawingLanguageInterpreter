#pragma warning(disable:4996)
#include "semantic.h"
#include <tchar.h> 
#define MAX_CHARS 200

extern HDC hDC;							// 窗口句柄，全局变量
char SrcFilePath[MAX_CHARS];		// 用于存放源程序文件路径
TCHAR Name[] = _T("函数绘图语言解释器");	// 窗口名

// 检查源程序文件是否合法函数声明
static bool CheckSrcFile(LPSTR);

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[] = _T("函数绘图语言解释器");

int WINAPI WinMain(HINSTANCE hThisInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpszArgument,
	int nFunsterStil)

{
	HWND hwnd;               /* This is the handle for our window */
	MSG messages;            /* Here messages to the application are saved */
	WNDCLASSEX wincl;        /* Data structure for the windowclass */
	int i;
	/*CTestDlg *pDlg;*/

	 /* The Window structure */
	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
	wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
	wincl.cbSize = sizeof(WNDCLASSEX);

	/* Use default icon and mouse-pointer */
	wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;                 /* No menu */
	wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
	wincl.cbWndExtra = 0;                      /* structure or the window instance */
	/* Use Windows's default color as the background of the window */
	wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

	/* Register the window class, and if it fails quit the program */
	if (!RegisterClassEx(&wincl))
		return 0;

	/* The class is registered, let's create the program*/
	hwnd = CreateWindowEx(
		0,                   /* Extended possibilites for variation */
		szClassName,         /* Classname */
		_T("函数绘图语言解释器"),       /* Title Text */
		WS_OVERLAPPEDWINDOW, /* default window */
		CW_USEDEFAULT,       /* Windows decides the position */
		CW_USEDEFAULT,       /* where the window ends up on the screen */
		740,                 /* The programs width */
		490,                 /* and height in pixels */
		HWND_DESKTOP,        /* The window is a child-window to desktop */
		NULL,                /* No menu */
		hThisInstance,       /* Program Instance handler */
		NULL                 /* No Window Creation data */
	);

	/* Make the window visible on the screen */
	ShowWindow(hwnd, nFunsterStil);
	hDC = GetDC(hwnd);
	/* pDlg=new CTestDlg();
	pDlg->Create(IDD_DIALOG1,this);
	 pDlg->ShowWindow(SW_SHOW);*/

	strcpy(SrcFilePath, "test2.txt");

	if (!CheckSrcFile(SrcFilePath)) return 1;

	// --------------------------------------------
	//		调用绘图语言解释器

	Parser(SrcFilePath);
	// --------------------------------------------

	/* Run the message loop. It will run until GetMessage() returns 0 */
	while (GetMessage(&messages, NULL, 0, 0))
	{
		/* Translate virtual-key messages into character messages */
		TranslateMessage(&messages);
		/* Send message to WindowProcedure */
		DispatchMessage(&messages);
	}

	/* The program return-value is 0 - The value that PostQuitMessage() gave */
	return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)                  /* handle the messages */
	{
	case WM_DESTROY:
		PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
		break;
	default:                      /* for messages that we don't deal with */
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}
// 检查源程序文件是否合法函数实现
bool CheckSrcFile(LPSTR lpszCmdParam)
{
	FILE * file = NULL;
	if (strlen(lpszCmdParam) == 0)
	{
		MessageBoxA(NULL, "未指定源程序文件 !", "错误", MB_OK);
		return false;
	}
	if ((file = fopen(lpszCmdParam, "r")) == NULL)
	{
		MessageBoxA(NULL, "打开源程序文件出错 !", "错误", MB_OK);
		MessageBoxA(NULL, lpszCmdParam, "文件名", MB_OK);
		return false;
	}
	else fclose(file);
	return true;
}
