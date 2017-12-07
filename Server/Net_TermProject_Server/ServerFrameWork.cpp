#include "stdafx.h"
#include "ServerFrameWork.h"

//정의
Room ServerFrameWork::room[MAXROOMCOUNT];

HANDLE ServerFrameWork::hCommunicated[MAXROOMCOUNT][MAX_PLAYER];
HANDLE ServerFrameWork::hSendPacket[MAXROOMCOUNT][MAX_PLAYER];

HANDLE ServerFrameWork::hGameThread[MAXROOMCOUNT];
HANDLE ServerFrameWork::hNextThreadCall;
std::queue<int> ServerFrameWork::m_ThreadQueue;

#define TEST_THREAD_COUNT 1
#define TEST_CNT_COUNT	10000
int cnt = 0;

ServerFrameWork::ServerFrameWork()
{
	ZeroMemory(room, sizeof(Room)*MAXROOMCOUNT);

	HANDLE hTmp;
	hTmp = CreateThread(NULL, 0, ThreadOrder, 0, 0, NULL);
	CloseHandle(hTmp);

	hNextThreadCall = CreateEvent(NULL, FALSE, FALSE, NULL);
}

ServerFrameWork::~ServerFrameWork()
{
}




void ServerFrameWork::SetSocket(int RoomNumber, int PlayerId,SOCKET socket)
{
	room[RoomNumber].m_teamList[PlayerId].m_socket = socket;
	//	TimeOut 옵션 필요할 것 같음
}

void ServerFrameWork::InitRoom(int roomIndex)
{
	//	Make Event
	room[roomIndex].m_roomState = Play;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		hCommunicated[roomIndex][i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		hSendPacket[roomIndex][i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}

	hGameThread[roomIndex] = CreateEvent(NULL, FALSE, FALSE, NULL);

	Room_Player p[MAX_PLAYER];
	HANDLE hTmp;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		p[i].roomNum = roomIndex;
		p[i].playerNum = i;
		hTmp = CreateThread(NULL, 0, CommunicationPlayer, (LPVOID)&p[i], 0, NULL);
		CloseHandle(hTmp);
	}
	hTmp = CreateThread(NULL, 0, GameThread, (LPVOID)roomIndex, 0, NULL);
	CloseHandle(hTmp);
	
}

void ServerFrameWork::GameStart(int roomIndex)
{
	for (int i = 0; i < MAX_PLAYER; ++i) {
		SetEvent(hSendPacket[roomIndex][i]);
		//NOTUSESLEEP
		//Sleep(1000);
	}
	m_ThreadQueue.push(roomIndex);
	SetEvent(hNextThreadCall);
	room[roomIndex].timeInit();
}

void ServerFrameWork::CloseRoom(int roomIndex)
{
	for (int i = 0; i < MAX_PLAYER; ++i) {
		CloseHandle(hCommunicated[roomIndex][i]);
		CloseHandle(hSendPacket[roomIndex][i]);
	}
	CloseHandle(hGameThread[roomIndex]);
}

DWORD ServerFrameWork::ThreadOrder(LPVOID arg)
{
	int index;
	DWORD retval;
	while (true) {
		retval = WaitForSingleObject(hNextThreadCall, INFINITE);
		if (retval != WAIT_OBJECT_0)break;

		if (!m_ThreadQueue.empty()) 
		{
			index = m_ThreadQueue.front();
			m_ThreadQueue.pop();

			if (room[index].m_roomState == Play) 
			{
				m_ThreadQueue.push(index);
				SetEvent(hGameThread[index]);
			}
			else 
			{
				SetEvent(hNextThreadCall);
			}
		}
	}
	return 0;
}

DWORD ServerFrameWork::GameThread(LPVOID arg)
{
	int roomIndex = (int)arg;
	DWORD retEvent;
	while (true)
	{
		//	wait queue
		WaitForSingleObject(hGameThread[roomIndex], INFINITE);
		//for test
		//if (cnt++ >= TEST_CNT_COUNT)break;
		//::printf("\n cnt = %d\n", cnt);

		//	wait Communication
		retEvent = WaitForMultipleObjects(MAX_PLAYER, hCommunicated[roomIndex], TRUE, INFINITE);
		if (retEvent != WAIT_OBJECT_0)break;

#ifdef FIXFREQUENCY
		//	Fix FrameRate with THREADFREQ
		FixFrame(roomIndex);
#endif
		// Calc;
		//::printf("[%d] Calculate\n", roomIndex);
		Calculate(roomIndex);
		//::printf("[%d] Calc Success\n", roomIndex);

		// Set packet
		S2CPacket packet;
		::ZeroMemory(&packet, sizeof(S2CPacket));
		packet.SetPacket(roomIndex, room[roomIndex]);

		//	Send
		SendPacketToClient(&packet, roomIndex);
		//::printf("SendPacketToClient %d \n", roomIndex);

		//	For CommunicationPlayer Thread, notice ready to Communicate
		for (int i = 0; i < MAX_PLAYER; ++i)
			SetEvent(hSendPacket[roomIndex][i]);

		//	Set Next Thread's
		SetEvent(hNextThreadCall);
	}
	return 0;
}

DWORD ServerFrameWork::CommunicationPlayer(LPVOID arg)
{
	Room_Player* index = (Room_Player*)arg;
	int index_p = index->playerNum, index_r = index->roomNum;
	int retval;
	DWORD retEvent;
	while (true) {
	
		retEvent = WaitForSingleObject(hSendPacket[index_r][index_p], INFINITE);
		if (retEvent != WAIT_OBJECT_0)break;

		retval = ReceivePacketFromClient(index_r, index_p);

		if (retval != SOCKET_ERROR);
			//::printf("CommunicationPlayer %d %d retval=%d\n", index_r, index_p, retval);
		else;
			//::printf("CommunicationPlayer %d %d SOCKET_ERROR\n", index_r, index_p);

		SetEvent(hCommunicated[index_r][index_p]);
		
		//if (cnt >= TEST_CNT_COUNT)break;
	}
	return 0;
}

int ServerFrameWork::ReceivePacketFromClient(int roomNum, int PlayerID)
{
	//InfoTeam *team = &room[roomNum].m_teamList[PlayerID];
	C2SPacket packet;
	ZeroMemory(&packet, sizeof(C2SPacket));
	int retval;
	retval = recvn(TeamList(roomNum, PlayerID).m_socket, (char*)&packet, sizeof(C2SPacket), 0);
	if (retval == SOCKET_ERROR)
		return SOCKET_ERROR;
	//printf("ReceivePacketFromClient room:%d, PlayerId:%d, retval = %d\n", roomNum, PlayerID, retval);
	memcpy(&TeamList(roomNum, PlayerID).m_player, &packet.player, sizeof(InfoPlayer));
	memcpy(&TeamList(roomNum, PlayerID).m_bullets, &packet.Bullets, sizeof(InfoBullet)*MAX_BULLET);

	return retval;
}

void ServerFrameWork::SendPacketToClient(S2CPacket * packet, int roomNum)
{
	SOCKET client_sock[MAX_PLAYER];

	for (int i = 0; i < MAX_PLAYER; ++i){
		client_sock[i] = TeamList(roomNum,i).m_socket;
	}

	//	ConnectCheck? , Need Thread?
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		packet->SendTime = chrono::system_clock::now();
		packet->Message = 0;
		send(client_sock[i], (char*)packet, sizeof(S2CPacket), 0);
	}
}

void ServerFrameWork::Calculate(int roomNum)
{
	InfoPlayer* player;
	InfoBullet* bullet;

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		player = &TeamList(roomNum, i).m_player;
		//	Bullet Check;
		if (IsPlayerDead(player->m_hp))continue;

		for (int j = 0; j < MAX_PLAYER; ++j)
		{
			//	동일 플레이어 건너띔
			if (i == j)continue;

			for (int k = 0; k < MAX_BULLET; ++k)
			{
				bullet = &TeamList(roomNum, j).m_bullets[k];
				if (!IsExistBullet(bullet->m_pos.x))continue;

				if (player->m_pos.x - PLAYERSIZE > bullet->m_pos.x + BULLETSIZE)continue;
				if (player->m_pos.x + PLAYERSIZE < bullet->m_pos.x - BULLETSIZE)continue;
				if (player->m_pos.y - PLAYERSIZE > bullet->m_pos.y + BULLETSIZE)continue;
				if (player->m_pos.y + PLAYERSIZE < bullet->m_pos.y - BULLETSIZE)continue;

				//	Collide Test ok
				player->m_hp -= BULLETDAMAGE;
				DestroyBullet(bullet);
			}
		}
	}
}

inline void ServerFrameWork::FixFrame(int roomIndex)
{
	while (true) {
		room[roomIndex].Tick();
		if (THREADFREQ > room[roomIndex].m_ElapsedTime)
			Sleep(1);
		else {
			room[roomIndex].m_ElapsedTime = 0;
			break;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////
/// 로비 ///
///////////////////////////////////////////////////////////////////////////////////

bool ServerFrameWork::arrivePlayer(SOCKET& socket, int roomID)
{
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		if (room[roomID].m_teamList[i].m_socket == NULL)
		{
			room[roomID].m_teamList[i].m_socket = socket;
			return true;
		}
	}
	return false;
}

int ServerFrameWork::findVocantRoom(SOCKET& socket)
{
	for (int i = 0; i < MAXROOMCOUNT; ++i)
	{
		/*room[i]에 모든 플레이어가 있는게 아니면*/
		if (!room[i].checkAllPlayerInRoom())
		{
			/*그 룸에 지금 들어온 애를 넣고*/
			if (!room[i].playerArrive(socket)) return -2;

			/*만약 지금 애 들어가서 그 룸이 4명이 되었으면*/
			if (room[i].checkAllPlayerInRoom())
			{
				/*게임 스타트 시킴*/
				room[i].gameStart();
				room[i].m_roomState = Play;
			}
			return i;
		}
	}
	return -1;
}


