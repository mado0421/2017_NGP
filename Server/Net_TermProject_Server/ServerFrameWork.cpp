#include "stdafx.h"
#include "ServerFrameWork.h"


extern int ats;
//extern Room room[MAXROOMCOUNT];

//Á¤ÀÇ?
Room ServerFrameWork::room[MAXROOMCOUNT];

ServerFrameWork::ServerFrameWork()
{
	
}

ServerFrameWork::~ServerFrameWork()
{
}

void ServerFrameWork::test()
{
	int roomNumber = 0;
	HANDLE hHandle;

	hHandle=CreateThread(NULL, 0, GameThread, (LPVOID)roomNumber, 0, NULL);
	room[roomNumber].m_clock = chrono::system_clock::now();

	WaitForSingleObject(hHandle, INFINITE);
}

void ServerFrameWork::testInit()
{
	for (int i = 0; i < MAXROOMCOUNT; ++i)
	{
		room[i].m_roomID = i;
	}
}

DWORD ServerFrameWork::GameThreadtest(LPVOID arg)
{
	argument* ar = (argument*)arg;
	while (true) {
		//printf("thread %d  %d\n", ar->number, ar->member);
		ar->member++;
		if (ar->member >= 100)
			ExitThread(0);
		printf("room[%d] id = %d\n",ar->member, room[ar->member].m_roomID);
		Sleep(0);
	}
	return 0;
}

void ServerFrameWork::test_ReceivePacketFromClient(int roomNum, int PlayerID)
{
}

DWORD ServerFrameWork::GameThread(LPVOID arg)
{
	int RoomNumber = (int)arg;
	printf("GameThreadCall %d\n", RoomNumber);
	while (true) {

	//	Need Room TimeCheckout
		room[RoomNumber].Tick();
		if (room[RoomNumber].m_ElapsedTime < THREADFREQ)
			Sleep(0);
		else
			room[RoomNumber].m_ElapsedTime = 0;

	//	Communication Success?
	//	{
	//
	//	}
		InfoTeam team[MAX_PLAYER];
		for (int i = 0; i < MAX_PLAYER; ++i) {
			memcpy(&team[i], &room[RoomNumber].m_teamList[i], sizeof(InfoPlayer));
		}

		InfoPlayer iPlayer[MAX_PLAYER];
		InfoBullet iBullet[MAX_PLAYER][MAX_BULLET];
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			iPlayer[i] = team[i].m_player;
			memcpy(iBullet[i], team[i].m_bullets, sizeof(InfoBullet)*MAX_BULLET);
		}

		// Calc;



		S2CPacket* packet = new S2CPacket;
		// Set packet
		packet->SetPacket(RoomNumber, &room[RoomNumber]);

		SendPacketToClient(packet, RoomNumber);
		printf("SendPacketToClient %d \n",RoomNumber);

		delete packet;
	}
	//	Set Next Thread's Event
}

DWORD ServerFrameWork::CommunicationPlayer(LPVOID arg)
{
	Room_Player* index = (Room_Player*)arg;
	int index_p = index->playerNum, index_r = index->roomNum;

	while (true) {
		ReceivePacketFromClient(index_r, index_p);
		printf("CommunicationPlayer %d %d\n", index_r,index_p);
	}
	
	return 0;
}

void ServerFrameWork::ReceivePacketFromClient(int roomNum, int PlayerID)
{
	InfoTeam* team = &room[roomNum].m_teamList[PlayerID];
	SOCKET client_sock = team->m_socket;
	C2SPacket packet;
	recv(client_sock, (char*)&packet, sizeof(C2SPacket), 0);

	memcpy(&team[roomNum].m_player, &packet.player, sizeof(InfoPlayer));
	memcpy(&team[roomNum].m_bullets, &packet.Bullets, sizeof(InfoBullet)*MAX_BULLET);
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


