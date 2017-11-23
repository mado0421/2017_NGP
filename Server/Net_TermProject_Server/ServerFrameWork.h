#pragma once

struct argument {
	int number = 0;
	int member = 0;
};
class ServerFrameWork
{
	static Room room[MAXROOMCOUNT];
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
	// test
	void test();

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

};

