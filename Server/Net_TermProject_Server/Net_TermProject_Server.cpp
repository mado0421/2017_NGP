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
#define SERVERPORT 9000
int main()
{
	int retval;
	int cnt = 4;
	//	윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	//	socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)err_quit("socket()");

	//	bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)err_quit("bind()");

	//	listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)err_quit("listen()");

	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	HANDLE hThread;
	int addrlen;
	Room_Player p;
	p.roomNum = 0;
	HANDLE hHandle[5];
	int i = 0;
	while (true) {
		g_server.InitRoom(p.roomNum);
		while (true) {
			//	accept()
			if (cnt == 0)break;
			addrlen = sizeof(clientaddr);
			client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
			if (client_sock == INVALID_SOCKET)
			{
				err_display("accept()");
				break;
			}
			p.playerNum = 4 - cnt--;
			printf("Player Number=%d\n", p.playerNum);
			//	접속한 클라이언트 정보 출력
			printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
				inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
			g_server.SetSocket(p.roomNum, p.playerNum, client_sock);

			hHandle[i++] = CreateThread(NULL, 0, g_server.CommunicationPlayer, (LPVOID)&p, 0, NULL);

		}
		hHandle[i++] = CreateThread(NULL, 0, g_server.GameThread, (LPVOID)p.roomNum, 0, NULL);
		g_server.GameStart(p.roomNum);
		break;
	}
	WaitForMultipleObjects(5, hHandle, true, INFINITE);
	
	printf("exit\n");

}
