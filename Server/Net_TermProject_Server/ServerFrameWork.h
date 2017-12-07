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

public:	// 재욱
	bool arrivePlayer(SOCKET& socket, int roomID);

	int findVocantRoom(SOCKET& socket);	//빈 자리가 있는 룸을 찾아서 그 룸의 번호를 반환하는 함수

	void leavePlayer();

	void createNewRoom();
	void deleteRoom(int roomID);
	void checkRoomsState();

	public:
		void SetSocket(int RoomNumber, int PlayerId, SOCKET socket);
		void InitRoom(int RoomNumber);
		void GameStart(int RoomNumber);
		void CloseRoom(int RoomNumber);
public:
	//	Using in Thread Function
	static DWORD WINAPI GameThread(LPVOID arg);
	static DWORD WINAPI CommunicationPlayer(LPVOID arg);
	static DWORD WINAPI ThreadOrder(LPVOID arg);

	static int ReceivePacketFromClient(int roomNum,int PlayerID);
	static void SendPacketToClient(S2CPacket* packet,int roomNum);


};

