#pragma once

struct argument {
	int number = 0;
	int member = 0;
};

class ServerFrameWork
{
	static Room room[MAXROOMCOUNT];
	static HANDLE hCommunicated[MAXROOMCOUNT][MAX_PLAYER];
	static HANDLE hSendPacket[MAXROOMCOUNT][MAX_PLAYER];

	static HANDLE hGameThread[MAXROOMCOUNT];
	static HANDLE hNextThreadCall;

	static std::queue<int> m_ThreadQueue;

	//static HANDLE hroom[2];
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
		void SetSocket(int roomIndex, int PlayerId, SOCKET socket);
		void InitRoom(int roomIndex);
		void GameStart(int roomIndex);
		void CloseRoom(int roomIndex);
public:
	//	Using in Thread Function
	static DWORD WINAPI GameThread(LPVOID arg);
	static DWORD WINAPI CommunicationPlayer(LPVOID arg);
	static DWORD WINAPI ThreadOrder(LPVOID arg);

	static int ReceivePacketFromClient(int roomNum,int PlayerID);
	static void SendPacketToClient(S2CPacket* packet,int roomNum);
	static void Calculate(int roomNum);
	static inline void FixFrame(int roomNum);
};

