#pragma once

struct argument {
	int number = 0;
	int member = 0;
};



class ServerFrameWork
{
	static Room room[MAXROOMCOUNT];
	static HANDLE hGameThread[MAXROOMCOUNT];
	static bool isCommunicationed[MAXROOMCOUNT][MAX_PLAYER];
	HANDLE hTread[2];
	argument th[2];
public:
	ServerFrameWork();
	~ServerFrameWork();

public:	// Àç¿í
	void arrivePlayer();
	void leavePlayer();

	void createNewRoom();
	void deleteRoom(int roomID);
	void checkRoomsState();

public:
	static DWORD WINAPI GameThread(LPVOID arg);
	static DWORD WINAPI CommunicationPlayer(LPVOID arg);
	static void ReceivePacketFromClient(int roomNum,int PlayerID);
	static void SendPacketToClient(S2CPacket* packet,int roomNum);
public:
	// test
	void test();
	void testInit();
	static DWORD WINAPI GameThreadtest(LPVOID arg);
	void ThreadTest()
	{
		th[0].number = 0;
		th[0].member = 0;
		th[1].number = 1;
		th[1].member = 0;
		hTread[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)this->GameThreadtest, &th[0], 0, NULL);
		hTread[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)this->GameThreadtest, &th[1], 0, NULL);
		WaitForMultipleObjects(2, hTread, TRUE, INFINITE);	
	}

	void test_ReceivePacketFromClient(int roomNum, int PlayerID);
	void test_Commun(Room_Player);

};

