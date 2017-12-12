#pragma once


class ServerFrameWork
{
	enum { end_of_game = -1, ok = 1, };
	enum { data = 0, end_data = 1 };

	static Room room[MAXROOMCOUNT];
	static std::vector<Tile> m_map;
	static int itemIndex[MAX_ITEM];

	static HANDLE hCommunicated[MAXROOMCOUNT][MAX_PLAYER];
	static HANDLE hSendPacket[MAXROOMCOUNT][MAX_PLAYER];

	static HANDLE hGameThread[MAXROOMCOUNT];
	static HANDLE hThreadScheduler;
	static bool bOrder[MAXROOMCOUNT];

	static std::vector<int> m_order;
	static std::queue<int> m_delQueue;
	static std::queue<int> m_insQueue;

	static std::queue<BuffInfo> m_buffQueue;

public:
	ServerFrameWork();
	~ServerFrameWork();

public:
	bool arrivePlayer(SOCKET& socket, int roomID);

	//빈 자리가 있는 룸을 찾아서 그 룸의 번호를 반환하는 함수
	int findVocantRoom(SOCKET& socket);   
	bool isGameReady(int i) {
		return room[i].m_roomState;
	}

	public:
		void SetSocket(int roomIndex, int PlayerId, SOCKET socket);
		void InitRoom(int roomIndex);
		void GameStart(int roomIndex);
	
public:
	//	Using in Thread Function
	static DWORD WINAPI GameThread(LPVOID arg);
	static DWORD WINAPI CommunicationPlayer(LPVOID arg);
	static DWORD WINAPI ThreadScheduler(LPVOID arg);
	static chrono::system_clock::time_point t_order_start;
	static chrono::system_clock::time_point t_order_end;

	static int ReceivePacketFromClient(int roomNum,int PlayerID);
	static void SendPacketToClient(S2CPacket* packet,int roomNum);
	static int Calculate(int roomNum);

	static inline void FixFrame(int roomNum);

	static void GameEnd(int roomIndex);
	static void CloseRoom(int roomIndex);

};

