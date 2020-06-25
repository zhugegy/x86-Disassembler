#include "stdafx.h"
#include "Interface.h"

bool SetEnvironment()
{
	system("color 07");
	system("title ·´»ã±àÒýÇæ");
	CONSOLE_CURSOR_INFO CurInfo;
	CurInfo.bVisible = false;
	CurInfo.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&CurInfo);
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiRawMap;
	SMALL_RECT srRawMap = { 0, 0, 120, 120 };
	GetConsoleScreenBufferInfo(hStdOut, &csbiRawMap);
	csbiRawMap.dwSize.X = 120;
	csbiRawMap.dwSize.Y = 120;
	SetConsoleScreenBufferSize(hStdOut, csbiRawMap.dwSize);
	SetConsoleWindowInfo(hStdOut, true, &srRawMap);
	return true;
}
