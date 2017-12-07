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
			if (!room[i].playerArrive(socket, i)) return -2;

			/*만약 지금 애 들어가서 그 룸이 4명이 되었으면*/
			if (room[i].checkAllPlayerInRoom())
			{
				/*게임 스타트 시킴*/
				room[i].gameStart();
			}
			return i;
		}
	}
	return -1;
}

void ServerFrameWork::SetSocket(int RoomNumber, int PlayerId,SOCKET socket)
{
	room[RoomNumber].m_teamList[PlayerId].m_socket = socket;
	//	TimeOut 옵션 필요할 것 같음
}

void ServerFrameWork::InitRoom(int RoomNumber)
{
	//	Make Event
	room[RoomNumber].m_roomState = Play;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		hCommunicated[RoomNumber][i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		hSendPacket[RoomNumber][i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}

	hGameThread[RoomNumber] = CreateEvent(NULL, FALSE, FALSE, NULL);
}

void ServerFrameWork::GameStart(int RoomNumber)
{
	for (int i = 0; i < MAX_PLAYER; ++i) {
		SetEvent(hSendPacket[RoomNumber][i]);
		//NOTUSESLEEP
		Sleep(1000);
	}
	m_ThreadQueue.push(RoomNumber);
	SetEvent(hNextThreadCall);
	room[RoomNumber].timeInit();
}

void ServerFrameWork::CloseRoom(int RoomNumber)
{
	for (int i = 0; i < MAX_PLAYER; ++i) {
		CloseHandle(hCommunicated[RoomNumber][i]);
		CloseHandle(hSendPacket[RoomNumber][i]);
	}
	CloseHandle(hGameThread[RoomNumber]);
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
	int RoomNumber = (int)arg;
	DWORD retEvent;
	while (true)
	{
		WaitForSingleObject(hGameThread[RoomNumber], INFINITE);

		//
		if (cnt++ >= TEST_CNT_COUNT)break;

		::printf("\n cnt = %d\n", cnt);

		retEvent = WaitForMultipleObjects(MAX_PLAYER, hCommunicated[RoomNumber], TRUE, INFINITE);
		if (retEvent != WAIT_OBJECT_0)break;

		//	Room TimeCheckout
		while (true) {
			room[RoomNumber].Tick();
			if (THREADFREQ > room[RoomNumber].m_ElapsedTime)
				Sleep(1);
			else {
				room[RoomNumber].m_ElapsedTime = 0;
				break;
			}
		}

		//::printf("In GameThread room: %d\n", RoomNumber);
		//	Communication Success?
		//	{
		//
		//	}
		

		//	Set Data From Room -> Need Change
		InfoTeam team[MAX_PLAYER];
		InfoPlayer iPlayer[MAX_PLAYER];
		InfoBullet iBullet[MAX_PLAYER][MAX_BULLET];

		memcpy(&team, &room[RoomNumber].m_teamList, sizeof(InfoTeam)*MAX_PLAYER);
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			iPlayer[i] = team[i].m_player;
			memcpy(iBullet[i], team[i].m_bullets, sizeof(InfoBullet)*MAX_BULLET);
		}

		// Calc;
		//
		//
		//
		//

		// Set packet
		S2CPacket packet;
		::ZeroMemory(&packet, sizeof(S2CPacket));
		packet.SetPacket(RoomNumber, room[RoomNumber]);

		//	Send
		SendPacketToClient(&packet, RoomNumber);
		::printf("SendPacketToClient %d \n", RoomNumber);

		//	For CommunicationPlayer Thread, notice ready to Communicate
		for (int i = 0; i < MAX_PLAYER; ++i)
			SetEvent(hSendPacket[RoomNumber][i]);

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

		if (retval != SOCKET_ERROR)
			::printf("CommunicationPlayer %d %d retval=%d\n", index_r, index_p, retval);
		else
			::printf("CommunicationPlayer %d %d SOCKET_ERROR\n", index_r, index_p);

		SetEvent(hCommunicated[index_r][index_p]);
		
		if (cnt >= TEST_CNT_COUNT)break;
	}
	return 0;
}

int ServerFrameWork::ReceivePacketFromClient(int roomNum, int PlayerID)
{
	InfoTeam *team = &room[roomNum].m_teamList[PlayerID];
	C2SPacket packet;
	ZeroMemory(&packet, sizeof(C2SPacket));
	int retval;
	retval = recvn(team->m_socket, (char*)&packet, sizeof(C2SPacket), 0);
	if (retval == SOCKET_ERROR)
		return SOCKET_ERROR;
	printf("ReceivePacketFromClient room:%d, PlayerId:%d, retval = %d\n", roomNum, PlayerID, retval);
	memcpy(&team->m_player, &packet.player, sizeof(InfoPlayer));
	memcpy(&team->m_bullets, &packet.Bullets, sizeof(InfoBullet)*MAX_BULLET);

	return retval;
}

void ServerFrameWork::SendPacketToClient(S2CPacket * packet, int roomNum)
{
	InfoTeam teamList[MAX_PLAYER];
	memcpy(&teamList, room[roomNum].m_teamList, sizeof(InfoTeam)*MAX_PLAYER);

	SOCKET client_sock[MAX_PLAYER];
	for (int i = 0; i < MAX_PLAYER; ++i){
		client_sock[i] = teamList[i].m_socket;
	}

	//	ConnectCheck? , Need Thread?
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		packet->SendTime = chrono::system_clock::now();
		send(client_sock[i], (char*)packet, sizeof(S2CPacket), 0);
	}
}


