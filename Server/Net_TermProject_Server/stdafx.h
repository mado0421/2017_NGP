// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once
#pragma comment(lib,"ws2_32")

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include<WinSock2.h>
#include<stdlib.h>	
#include<stdio.h>
#include<math.h>
#include<chrono>
#include<vector>
#include<queue>
#include<algorithm>
#include<fstream>
using namespace std;

#define SERVERPORT 9000

#define MAX_BULLET 18
#define MAX_PLAYER 4
#define MAX_ITEM 3

#define DEFAULTHP 1
#define BULLETSPD 320.0f
#define BULLETSIZE 2.5f
#define BULLETDAMAGE 1
#define NOTEXIST	0

#define ITEMSIZE 7.5f
#define ITEMCOOLTIME	5.0f
#define TILESIZE	16
#define TILEMAXCNT	100

#define PLAYERSPD 125.0f
#define PLAYERSIZE 7.5f
#define PLAYERMAXHP	3
#define MAX_AMMO 6

#define MAXROOMCOUNT	100
#define THREADFREQ	0.016f
#define ORDERFREQ	0.016f

//#define DEBUGMODE

//#define FIXFREQUENCY

#define MSGSIZE 1
int recvn(SOCKET s, char *buf, int len, int flags);

template <class val>
inline void clamp(val& c, val min, val max)
{
	if (max < min) 
		swap(min, max);
	if (c > max)c = max;
	if (c < min)c = min;
}

enum msg {
	TEAMNO = 0,
	ISREADY,
	STARTPLAY,
	LEAVE,
	TEST,

	OK,
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

namespace Vector
{
	inline float length(Vector2D val) { return sqrt(val.x*val.x + val.y*val.y); }
	inline Vector2D normalize(Vector2D val) { float len = Vector::length(val); if (len != 0) return Vector2D(val.x / len, val.y / len); else return Vector2D(0, 0); }
	inline Vector2D add(Vector2D a, Vector2D b) { return Vector2D(b.x + a.x, b.y + a.y); }
	inline Vector2D sub(Vector2D from, Vector2D to) { return Vector2D(to.x - from.x, to.y - from.y); }
}

enum {
	dead,normal,burst
};

struct InfoPlayer {
	Vector2D m_pos;
	int m_hp;
	int m_state;
};

inline bool IsPlayerDead(int arg) {
	if (arg <= 0)
		return true;
	else 
		return false;
}

struct InfoBullet {
	Vector2D m_pos;
	int m_damage;
};

inline bool IsExistBullet(int damage)
{
	if (damage == NOTEXIST)
		return false;
	else 
		return true;
}

inline void DestroyBullet(InfoBullet* bullet) 
{
	bullet->m_damage = NOTEXIST;
}

#define BUFFDURATION	5.0f
enum {
	notExist = 0, medikit, reinforce
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

struct BuffInfo
{
	int roomIndex, PlayerID;
	double duration;
	chrono::system_clock::time_point occurTime;
	BuffInfo(){}
	BuffInfo(int ri, int pid, float d) :
		roomIndex(ri), PlayerID(pid), duration(d)
	{
		occurTime = chrono::system_clock::now();
	}
	BuffInfo(int ri,int pid,float d, 
		chrono::system_clock::time_point t):
		roomIndex(ri),PlayerID(pid),duration(d),occurTime(t) {}
	bool endcheck(chrono::system_clock::time_point& other)
	{
		double elapsed = chrono::duration_cast<chrono::milliseconds>
			(other - occurTime).count()*0.001f;
		if (elapsed >= duration)
			return true;
		return false;
	}
};

bool isItemCooltime(chrono::system_clock::time_point& time);

#define TeamList(RoomIndex,PlayerIndex) room[RoomIndex].m_teamList[PlayerIndex]

inline bool IsZero(float a) {
	if (abs(a) < FLT_EPSILON)
		return true;
	else 
		return false;
}

inline bool rectCollideCheck(Vector2D a, float aSize,
	Vector2D b, float bSize)
{
	if (a.x - aSize > b.x + bSize)return false;
	if (a.x + aSize < b.x - bSize)return false;
	if (a.y - aSize > b.y + bSize)return false;
	if (a.y + aSize < b.y - bSize)return false;
	return true;
}

enum tile
{
	Wall = 0,
	PSpawn,
	ISpawn,
};

struct Tile
{
	Vector2D m_pos;
	int m_type;
	int m_size;
	Tile() {}
	Tile(Vector2D v, int t):m_pos(v),m_type(t) {}
};

bool loadMapFile(char *fileDirectory, vector<Tile>& list);


enum {
	Lobby = false, Play = true, closing = 4
};

struct Room
{
	///////////////////////////////////////////////
	// 클라이언트
	// Room을 특정하는 고유 키
	unsigned int m_roomID;

	// 각 팀별로 정보를 저장하는 배열
	InfoTeam m_teamList[MAX_PLAYER];

	// 아이템 정보를 저장하는 배열
	InfoItem m_itemList[MAX_ITEM];


	// Room의 상태를 저장하는 변수(false: Lobby, true: Play)
	bool m_roomState;

	// Room에 접속해있는 현재 인원
	int m_numOfPlayerInRoom;

	std::queue<BuffInfo> m_buffQueue;

	// 준혁 - 시간체크용 tmp;
	double m_ElapsedTime = 0;
	chrono::system_clock::time_point m_clock;
	chrono::system_clock::time_point m_past;
	void Tick() { 
		m_clock = chrono::system_clock::now();
		m_ElapsedTime += chrono::duration_cast<chrono::milliseconds>(m_clock - m_past).count()*0.001f;
		m_past = m_clock;
		printf("Tick = %f\n", m_ElapsedTime);
		Sleep(1);
	};
	void timeInit() {
		m_past = chrono::system_clock::now();
		m_ElapsedTime = 0;
	}

	///////////////////////////////////////////////
	// 서버

	Room() {};
	~Room() {};

	void Room::initialize()
	{
		/*for Test*/
		for (int i = 0; i < MAX_PLAYER; ++i)
			m_teamList[i].m_socket = NULL;
	}

	bool checkAllPlayerInRoom()
	{
		char msg[MSGSIZE];
		int retval;
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			if (m_teamList[i].m_socket != NULL)
			{
				msg[0] = msg::ISREADY;
				retval = send(m_teamList[i].m_socket, msg, MSGSIZE, 0);
				if (retval == SOCKET_ERROR) return false;
				retval = recvn(m_teamList[i].m_socket, msg, MSGSIZE, 0);
				if (retval == SOCKET_ERROR) return false;
			}
			else return false;
		}
		return true;
	};

	bool gameStart()
	{
		char msg[MSGSIZE];
		int retval;
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			if (m_teamList[i].m_socket != NULL)
			{
				msg[0] = msg::STARTPLAY;
				retval = send(m_teamList[i].m_socket, msg, MSGSIZE, 0);
				if (retval == SOCKET_ERROR) return false;
				retval = recvn(m_teamList[i].m_socket, msg, MSGSIZE, 0);
				if (retval == SOCKET_ERROR) return false;
			}
			else return false;
		}
		return true;
	};
	///////////////////////////////////////////////
	// 클라이언트
	bool accessLobby();
	void leaveLobby();
	void checkMsg();

	///////////////////////////////////////////////
	// 서버
	bool isPlayerReady(int idx);
	void readyToStartPlay();

	bool playerArrive(SOCKET& socket);
};

// TMP using in 
//DWORD ServerFrameWork::CommunicationPlayer(LPVOID arg)'s argment
struct Room_Player
{
	int roomNum;
	int playerNum;
};

struct S2CPacket
{	// Server to Client Packet 구조체 실제 데이터를 서버에서 보낼
	DWORD	Message;	//	0번 Data, 1번 게임종료
	InfoPlayer iPlayer[MAX_PLAYER];
	InfoBullet iBullet[MAX_PLAYER][MAX_BULLET];
	InfoItem iItem[MAX_ITEM];
//	chrono::system_clock::time_point SendTime;
	
	void SetPacket(int roomNumber, Room& room)
	{
		InfoTeam iTeam[MAX_PLAYER];
		memcpy(&iTeam, &(room.m_teamList), sizeof(InfoTeam)*MAX_PLAYER);

		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			memcpy(&iPlayer[i], &iTeam[i].m_player, sizeof(InfoPlayer));
			memcpy(&iBullet[i], &iTeam[i].m_bullets, sizeof(InfoBullet)*MAX_BULLET);
			memcpy(&iItem[i], &room.m_itemList, sizeof(InfoItem)*MAX_ITEM);
		}
	};
};

struct C2SPacket 
{
	InfoPlayer player;
	InfoBullet Bullets[MAX_BULLET];
};

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.



