#pragma once
#include "h.h"

#define MAX_ROOM 20
#define MAX_BULLET 18
#define MAX_PLAYER 4
#define MAX_ITEM 3

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

class Room
{
private:
	///////////////////////////////////////////////
	// 클라이언트
	// Room을 특정하는 고유 키
	unsigned short m_roomID;

	// 각 팀별로 정보를 저장하는 배열
	InfoTeam m_teamList[MAX_PLAYER];

	// 아이템 정보를 저장하는 배열
	InfoItem m_itemList[MAX_ITEM];

	// Room의 상태를 저장하는 변수(false: Lobby, true: Play)
	bool m_roomState;

	// Room에 접속해있는 현재 인원
	int m_nPlayerInRoom;

	///////////////////////////////////////////////
	// 서버
	
public:
	Room();
	~Room();
public:
	///////////////////////////////////////////////
	void initialize();
	void playerArrive(const SOCKET &client_sock);

	bool checkMsg(char* msg);
	void sendMsg(int player, int type);

	///////////////////////////////////////////////
	// LobbyState
	///////////////////////////////////////////////
	// 클라이언트
	bool accessLobby();
	void leaveLobby();
	void checkMsg();

	///////////////////////////////////////////////
	// 서버
	bool isPlayerReady(int idx);
	void readyToStartPlay();
	void playerArrive();

	///////////////////////////////////////////////
	// PlayState
	///////////////////////////////////////////////
	// 클라이언트
	/*게임 결과를 수신받음*/
	void recvResult();

	///////////////////////////////////////////////
	// 서버
	void createItem();

	// 현재 게임상황을 보고 게임이 끝났는지 확인함
	/*플레이어가 한 명만 남거나, 동시에 죽거나, 시간이 끝나면 종료
	플레이어가 한 명만 남을 경우: 그 플레이어가 승리. 나머지는 죽은 순서대로 4위, 3위, 2위
	동시에 플레이어들이 죽을 경우: 해당 플레이어들은 draw, 나머지는 죽은 순서대로 4위, 3위.. (4명이 동시에 죽을 경우, 모두 draw)
	시간이 끝났을 경우: 위와 같음*/
	void checkPresentCondition();

	/*만약 게임에 라운드 별로 제한시간이 있다면? 만약 2분 동안 게임을 한다면?
	처음 3초간 준비상태, 1분간 통상상태, 30초간 회복불가상태, 마지막 30초간 최종전상태라면?
	준비상태: 모든 플레이어가 동시에 시작하기 위한 준비상태.
	통상상태: 일반적인 게임모드. 벽이 있고 회복 아이템이 나온다.
	회복불가상태: 노란 테마의 게임모드. 회복 아이템이 나오지 않는다.
	최종전상태: 붉은 테마의 게임모드. 회복 아이템이 나오지 않으며, 테두리가 아닌 벽들이 사라지고 모든 공격이 강공격으로 생성된다.*/
	void changeGameState();

};

