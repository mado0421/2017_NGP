#include "stdafx.h"
#include "ServerFrameWork.h"


extern int ats;
//extern Room room[MAXROOMCOUNT];

//Á¤ÀÇ?
Room ServerFrameWork::room[MAXROOMCOUNT];

ServerFrameWork::ServerFrameWork()
{
	for (int i = 0; i < MAXROOMCOUNT; ++i)
	{
	room[i].m_roomID = i;
	}
}

ServerFrameWork::~ServerFrameWork()
{
}

void ServerFrameWork::test()
{
	printf("%d \n", room[ats].m_roomID);
	ats = (ats + 1) % MAXROOMCOUNT;
	//printf("%d\n", ats);
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
