// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include "Dependencies\glew.h"
#include "Dependencies\freeglut.h"

#include <vector>

#include <WinSock2.h>
#include <Windows.h>
#include <timeapi.h>
#include <chrono>
#pragma comment(lib, "ws2_32")

#define PLAYER_0 0
#define PLAYER_1 1
#define PLAYER_2 2
#define PLAYER_3 3

#define WWIDTH (21 * 32)
#define WHEIGHT (21 * 32)

#define TILESIZE 16.0f
#define SERVERIP "127.0.0.1"
#define BUFSIZE 256
#define SERVERPORT 9000
#define MSGSIZE 1
#define MSGSIZE2 4
#define MAX_BULLET 18
#define MAX_PLAYER 4

enum msg {
	TEAMNO = 0,
	ISREADY,
	STARTPLAY,
	LEAVE,
	TEST,

	OK,
};

enum waitState {
	none=0,
	wait,
	error,
	success
};

struct NetworkData
{
	SOCKADDR_IN serveraddr;
	WSADATA		wsa;
	SOCKET		sock;
	int			m_myTeamNo;
	int			state = waitState::none;
};

struct Color
{
	float r, g, b, a;
	Color()
		: r(0.0f)
		, g(0.0f)
		, b(0.0f)
		, a(0.0f)
	{}
	Color(float r, float g, float b, float a)
		: r(r)
		, g(g)
		, b(b)
		, a(a)
	{}
	~Color() {}
};

struct Vector2D
{
	float x, y;
	Vector2D() { x = 0.0f; y = 0.0f; }
	Vector2D(float x, float y)
		:x(x), y(y)
	{}
	~Vector2D() {}
};

enum message {
	DATA, STARTGAME, ENDGAME
};

namespace Vector
{
	inline float length(Vector2D val) { return sqrt(val.x*val.x + val.y*val.y); }
	inline Vector2D normalize(Vector2D val) { float len = Vector::length(val); if(len != 0) return Vector2D(val.x / len, val.y / len); else return Vector2D(0, 0); }
	inline Vector2D add(Vector2D a, Vector2D b) { return Vector2D(b.x + a.x, b.y + a.y); }
	inline Vector2D sub(Vector2D from, Vector2D to) { return Vector2D(to.x - from.x, to.y - from.y); }
}

struct InfoPlayer {
	Vector2D m_pos;
	int m_hp;
	int m_state;
};

struct InfoBullet {
	Vector2D m_pos;
	int m_type;
};

struct InfoItem {
	Vector2D m_pos;
	int m_type;
};

struct InfoTeam {
	SOCKET m_socket;
	InfoPlayer m_player;
	InfoBullet m_bullets[MAX_BULLET];
};

//소켓 함수 오류 출력 후 종료
inline void err_quit(char *msg)
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

//소켓 함수 오류 출력
inline void err_display(char *msg)
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

// 사용자 정의 데이터 수신 함수
inline int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
struct startData {
	int argc;
	char **argv;
};

struct S2CPacket {	// Server to Client Packet 구조체 실제 데이터를 서버에서 보낼
	DWORD	Message;	//	HIWORD 메시지 타입
						//	0번 Data, 1번 게임시작, 2번 게임종료…
	InfoPlayer iPlayer[MAX_PLAYER];
	InfoBullet iBullet[MAX_PLAYER][MAX_BULLET];
	std::chrono::system_clock::time_point SendTime;

};


struct C2SPacket {

	InfoPlayer player;
	InfoBullet Bullets[MAX_BULLET];
};