// 네트워크게임프로그래밍_게임.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Framework.h"
#include <chrono>
#include <Windows.h>
#define MAX_ROOM 20
#define MAX_BULLET 18
#define MAX_PLAYER 4
#define MAX_ITEM 3



//class ServerFramework {
//private:
//	int m_nCurAccessedPlayers;
//
//public:
//	ServerFramework();
//	~ServerFramework();
//
//public:
//	void arrivePlayer();
//	void leavePlayer();
//
//	void createNewRoom();
//	void deleteRoom(int roomID);
//	void checkRoomsState();
//};

InfoPlayer p[4];
C2SPacket c2spacket;
S2CPacket s2cpacket;


namespace
{
	Framework framework;
}

GLvoid initialize(int argc, char **argv);
GLvoid renderScene(GLvoid);
GLvoid reshape(int w, int h);
GLvoid idle(GLvoid);
GLvoid mouseInput(int button, int state, int x, int y);
GLvoid keyDown(unsigned char key, int x, int y);
GLvoid keyUp(unsigned char key, int x, int y);
GLvoid specialKeyDown(int key, int x, int y);
GLvoid specialKeyUp(int key, int x, int y);

std::chrono::system_clock::time_point start;
std::chrono::duration<double> sec;

DWORD WINAPI communicateThreadFunc(LPVOID arg)
{
	int retval;
	start = std::chrono::system_clock::now();
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == INVALID_SOCKET)
		err_quit("socket()");

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("connect");
	int msg = 0;
	
	//send: 저 접속했어요 메시지, 필요 없을 수 있음
	while (msg != ENDGAME)
	{
		sec = std::chrono::system_clock::now() - start;
		if(sec.count()>1/60)
		{
			memcpy(&(c2spacket.player), p, sizeof(InfoPlayer) * 4);
			send(sock, (char*)&c2spacket, sizeof(C2SPacket), 0);
			//retval = recvn(sock, (char*)&msg, MSGSIZE, 0);
			switch (msg)
			{
			case DATA:
				retval = recvn(sock, (char*)&s2cpacket, sizeof(S2CPacket), 0);
				memcpy(p, &(s2cpacket.iPlayer), sizeof(InfoPlayer) * 4);
				//printf("%f, %f", p[1].m_pos.x, p[1].m_pos.y);
				//memcpy(p, data, sizeof(InfoPlayer) * 4);
				framework.updatePlayerInfo(p);
				break;
			case STARTGAME:
				//pFramework->ChangeScene(PlayScene);
				break;
			case ENDGAME:
				//pFramework->ChangeScene(TitleScene);
				break;
			}
			/*memcpy(&(c2spacket.player), p, sizeof(InfoPlayer) * 4);
			send(sock, (char*)&c2spacket, sizeof(C2SPacket), 0);*/
			start = std::chrono::system_clock::now();
		}
	}
	closesocket(sock);
	WSACleanup();
	return 0;
}

DWORD WINAPI gameThreadFunc(LPVOID arg)
{
	startData s = *((startData*)arg);
	framework.initialize(s.argc, s.argv);
	framework.run();
	return 0;
}

int main(int argc, char **argv)
{
	startData s;
	s.argc = argc;
	s.argv = argv;
	HANDLE hThread[2];
	hThread[1] = CreateThread(NULL, 0, gameThreadFunc, (LPVOID)&s, 0, NULL);
	hThread[0] = CreateThread(NULL, 0, communicateThreadFunc, NULL, 0, NULL);
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
	return 0;

	//pFramework = new Framework();
	//pFramework->initialize();
}

GLvoid initialize(int argc, char **argv)
{
	framework.initialize(argc, argv);
}

GLvoid renderScene(GLvoid)
{
	framework.renderScene();
}

GLvoid reshape(int w, int h)
{
	framework.reshape(w, h);
}

GLvoid idle(GLvoid)
{
	framework.idle();
}

GLvoid mouseInput(int button, int state, int x, int y)
{
	framework.mouseInput(button, state, x, y);
}

GLvoid keyDown(unsigned char key, int x, int y)
{
	framework.keyDown(key, x, y);
}

GLvoid keyUp(unsigned char key, int x, int y)
{
	framework.keyUp(key, x, y);
}

GLvoid specialKeyDown(int key, int x, int y)
{
	framework.specialKeyDown(key, x, y);
}

GLvoid specialKeyUp(int key, int x, int y)
{
	framework.specialKeyUp(key, x, y);
}
