// Net_TermProject_Server.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include"ServerFrameWork.h"

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int ats = 0;
//Room room[MAXROOMCOUNT];

// arg is Room Id
//DWORD WINAPI GameThread(LPVOID arg)
//{
//	// MainGame
//
//	// Room[arg].Info
//
//	// Calc
//
//	// Send to Player 4
//	return 0;
//}

ServerFrameWork g_server;

int main()
{
	// test Code
	/*for (int i = 0; i < MAXROOMCOUNT; ++i)
	{
		room[i].m_roomID = i;
	}*/

	g_server.ThreadTest();

	//	main
	while (false)
	{
		//g_server.test();
		
		//need to build
	}
}
