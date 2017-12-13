#include "stdafx.h"
#include "ServerFrameWork.h"
//정의
Room ServerFrameWork::room[MAXROOMCOUNT];

//std::vector<Tile> ServerFrameWork::m_map;
//int ServerFrameWork::itemIndex[MAX_ITEM];

Vector2D ServerFrameWork::itemPos[MAX_ITEM];

HANDLE ServerFrameWork::hCommunicated[MAXROOMCOUNT][MAX_PLAYER];
HANDLE ServerFrameWork::hSendPacket[MAXROOMCOUNT][MAX_PLAYER];

HANDLE ServerFrameWork::hGameThread[MAXROOMCOUNT];
HANDLE ServerFrameWork::hThreadScheduler;

std::vector<int> ServerFrameWork::m_order;
std::queue<int> ServerFrameWork::m_delQueue;
std::queue<int> ServerFrameWork::m_insQueue;

chrono::system_clock::time_point ServerFrameWork::t_order_start;
chrono::system_clock::time_point ServerFrameWork::t_order_end;

ServerFrameWork::ServerFrameWork()
{
	ZeroMemory(room, sizeof(Room)*MAXROOMCOUNT);
	m_order.reserve(MAXROOMCOUNT);
	hThreadScheduler = CreateThread(NULL, 0, ThreadScheduler, 0, 0, NULL);

	printf("ServerFrameWork() complete\n");
	srand(time(NULL));
}

ServerFrameWork::~ServerFrameWork()
{
	for (int i = 0; i < MAXROOMCOUNT; ++i)
	{
		for (int j = 0; j < MAX_PLAYER; ++j)
		{
			CloseHandle(hCommunicated[i][j]);
			CloseHandle(hSendPacket[i][j]);
			closesocket(TeamList(i, j).m_socket);
		}
		CloseHandle(hGameThread[i]);
	}

	TerminateThread(hThreadScheduler, 0);
	CloseHandle(hThreadScheduler);
}

void ServerFrameWork::InitFrameWork()
{
	int ret = false;
	std::vector<Tile> m_map;
	m_map.reserve(TILEMAXCNT);
	char path[] = "assets\\maps\\map_1.map";
	while (!ret)
	{
		ret = loadMapFile(path, m_map);
		if (ret)
			printf("loadMapFile success\n");
		else {
			printf("loadMapFile failed\n");
		}
	}
	for (int j = 0, i = 0; j < m_map.size() && i < MAX_ITEM; ++j)
	{
		if (m_map[j].m_type == ISpawn)
		{
			itemPos[i] = m_map[j].m_pos;
			//printf("itemPos[%d] %f %f\n", i, itemPos[i].x, itemPos[i].y);
			++i;
		}
	}
	printf("InitFramework() complete\n");
}

void ServerFrameWork::SetSocket(int RoomNumber, int PlayerId,SOCKET socket)
{
	room[RoomNumber].m_teamList[PlayerId].m_socket = socket;
}

void ServerFrameWork::InitRoom(int roomIndex)
{
	//	Make Event
	for (int i = 0; i < MAX_PLAYER; ++i) {
		hCommunicated[roomIndex][i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		hSendPacket[roomIndex][i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	hGameThread[roomIndex] = CreateEvent(NULL, FALSE, FALSE, NULL);

	Room_Player* p[MAX_PLAYER];
	HANDLE hTmp;
	for (int i = 0; i < MAX_PLAYER; ++i) 
	{
		p[i] = new Room_Player;
		p[i]->roomNum = roomIndex;
		p[i]->playerNum = i;
	#ifdef DEBUGMODE
		printf("room:%d / p[%d] num: %d\n",p[i]->roomNum,i, p[i]->playerNum);
	#endif
		hTmp = CreateThread(NULL, 0, CommunicationPlayer, (LPVOID)p[i], 0, NULL);
		CloseHandle(hTmp);
	}
	hTmp = CreateThread(NULL, 0, GameThread, (LPVOID)roomIndex, 0, NULL);
	CloseHandle(hTmp);	
	printf("InitRoom %d complete\n", roomIndex);
}

void ServerFrameWork::GameStart(int roomIndex)
{
	for (int i = 0; i < MAX_PLAYER; ++i){
		SetEvent(hSendPacket[roomIndex][i]);
	}
	m_insQueue.push(roomIndex);
	room[roomIndex].timeInit();
	printf("room: %d GameStart\n", roomIndex);
}

void ServerFrameWork::GameEnd(int roomIndex)
{
	// Set packet
	SOCKET client_sock[MAX_PLAYER];
	S2CPacket endPacket;

	endPacket.SetPacket(roomIndex, room[roomIndex]);
	endPacket.Message = end_data;

	for (int i = 0; i < MAX_PLAYER; ++i) {
		client_sock[i] = TeamList(roomIndex, i).m_socket;
	}

	int ret;
	for (int i = 0; i < MAX_PLAYER; ++i){
		ret=send(client_sock[i], (char*)&endPacket, sizeof(S2CPacket), 0);
		printf("endSend ret=%d\n", ret);
	}
	room[roomIndex].m_roomState = closing;
	for (int i = 0; i < MAX_PLAYER; ++i)
		SetEvent(hSendPacket[roomIndex][i]);
	CloseRoom(roomIndex);
	printf("room %d의 게임이 종료 CloseRoom\n", roomIndex);
}

void ServerFrameWork::CloseRoom(int roomIndex)
{
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		CloseHandle(hCommunicated[roomIndex][i]);
		CloseHandle(hSendPacket[roomIndex][i]);
		closesocket(TeamList(roomIndex, i).m_socket);
		TeamList(roomIndex, i).m_socket = NULL;
	}
	CloseHandle(hGameThread[roomIndex]);
	
	m_delQueue.push(roomIndex);
}

DWORD ServerFrameWork::ThreadScheduler(LPVOID arg)
{
	int index;
	DWORD retval;
	int ins, del;
	double elapsedTime;
	while (true) {
		t_order_start = chrono::system_clock::now();
		elapsedTime = 0;
		//	Order
		while (!m_insQueue.empty())
		{
			ins = m_insQueue.front();
			m_insQueue.pop();
			m_order.push_back(ins);
		}
		if (!m_order.empty()) 
		{
			for (const auto& p : m_order)
			{
				if (room[p].m_roomState == Play)
				{
					//printf("SetEvent%d\n", p);
					SetEvent(hGameThread[p]);
				}
			}
			while (!m_delQueue.empty())
			{
				del = m_delQueue.front();
				m_delQueue.pop();
				m_order.erase(find(m_order.begin(), m_order.end(), del));
				room[del].m_roomState = Lobby;
			}
		}
		
		//	Freq
		while (elapsedTime < ORDERFREQ)
		{
			t_order_end = chrono::system_clock::now();
			elapsedTime = chrono::duration_cast<chrono::milliseconds>
				(t_order_end - t_order_start).count()*0.001f;
			Sleep(1);
		}
	}
	return 0;
}

DWORD ServerFrameWork::GameThread(LPVOID arg)
{
	int roomIndex = (int)arg;
	DWORD retEvent;
	int retCalc;
	chrono::system_clock::time_point timeGen = chrono::system_clock::now();
	chrono::system_clock::time_point threadTime;
	while (true)
	{
		//	wait queue
		WaitForSingleObject(hGameThread[roomIndex], INFINITE);
		
		//	wait Communication
		retEvent = WaitForMultipleObjects(MAX_PLAYER, hCommunicated[roomIndex], TRUE, INFINITE);

		// Calc
		threadTime = chrono::system_clock::now();
		retCalc = Calculate(roomIndex, threadTime);
		if (retCalc == end_of_game){
			printf("room:%d end_of_game\n",roomIndex);
			break;
		}

		// ItemGen
		if (isItemCooltime(timeGen))
		{
			//printf("ItemGen\n");
			for (int i = 0; i < MAX_ITEM; ++i)
			{
				if (room[roomIndex].m_itemList[i].m_type == notExist)
				{
					room[roomIndex].m_itemList[i].m_type = rand() % 2 + 1;
					room[roomIndex].m_itemList[i].m_pos = itemPos[i];
					/*room[roomIndex].m_itemList[i].m_type = 2;
					printf("room:%d , type:%d (%f, %f)\n", roomIndex, room[roomIndex].m_itemList[i].m_type,
						room[roomIndex].m_itemList[i].m_pos.x, room[roomIndex].m_itemList[i].m_pos.y);*/
					timeGen = chrono::system_clock::now();
				}
			}
		}

		// Set packet
		S2CPacket sendpacket;
		::ZeroMemory(&sendpacket, sizeof(S2CPacket));
		sendpacket.Message = data;

		sendpacket.SetPacket(roomIndex, room[roomIndex]);

		//	Send
		SendPacketToClient(&sendpacket, roomIndex);

		//	For CommunicationPlayer Thread, notice ready to Communicate
		for (int i = 0; i < MAX_PLAYER; ++i)
			SetEvent(hSendPacket[roomIndex][i]);
	}
	GameEnd(roomIndex);
	return 0;
}

DWORD ServerFrameWork::CommunicationPlayer(LPVOID arg)
{
	Room_Player* index = (Room_Player*)arg;
	int playerID = index->playerNum, roomIndex = index->roomNum;
	int retval;
	DWORD retEvent;
	
	delete arg;
	while (true) 
	{
#ifdef DEBUGMODE
		printf("[%d].%d before hSendPacket\n", roomIndex,playerID);
#endif
		retEvent = WaitForSingleObject(hSendPacket[roomIndex][playerID], INFINITE);

#ifdef DEBUGMODE
		printf("[%d].%d after hSendPacket\n", roomIndex, playerID);
#endif
		if (retEvent != WAIT_OBJECT_0)
		{
			if (room[roomIndex].m_roomState == Play) {
				printf("room:%d , Player:%d hSendPacket의 retEvent가 WAIT_OBJECT가 아님\n", roomIndex, playerID);
				return 0;
			}
		}
		if (room[roomIndex].m_roomState != Play) {
			printf("room:%d 플레이 종료된 CommnunicationPlayer 스레드 종료\n", roomIndex);
			break;
		}
#ifdef DEBUGMODE
		printf("[%d].%d before ReceivePacketFromClient\n", roomIndex, playerID);
#endif
		retval = ReceivePacketFromClient(roomIndex, playerID);
#ifdef DEBUGMODE
		printf("[%d].%d after ReceivePacketFromClient\n", roomIndex, playerID);
#endif
		if (retval == SOCKET_ERROR)
		{
			//closesocket(TeamList(roomIndex, playerID).m_socket);
			//CloseHandle(hCommunicated[roomIndex][playerID]);
			TeamList(roomIndex, playerID).m_player.m_hp = 0;
			//if (room[roomIndex].m_roomState == Play)
			//printf("room:%d , Player:%d 가 비정상적인 방법으로 종료됨\n", roomIndex, playerID);
			//return 0;
		 }
#ifdef DEBUGMODE
		printf("[%d].%d before hCommunicated\n", roomIndex, playerID);
#endif
		SetEvent(hCommunicated[roomIndex][playerID]);
#ifdef DEBUGMODE
		printf("[%d].%d after hCommunicated\n", roomIndex, playerID);
#endif
	}
	return 0;
}

int ServerFrameWork::ReceivePacketFromClient(int roomNum, int PlayerID)
{
	C2SPacket recvpacket;
	ZeroMemory(&recvpacket, sizeof(C2SPacket));
	int retval;

	retval = recvn(TeamList(roomNum, PlayerID).m_socket,(char*)&recvpacket, sizeof(C2SPacket), 0);
	//printf("데이터 수신 %d번\n", PlayerID);
	if (retval == SOCKET_ERROR)
		return SOCKET_ERROR;

	memcpy(&TeamList(roomNum, PlayerID).m_player, &recvpacket.player, sizeof(InfoPlayer));
	memcpy(&TeamList(roomNum, PlayerID).m_bullets, &recvpacket.Bullets, sizeof(InfoBullet)*MAX_BULLET);

	return retval;
}

void ServerFrameWork::SendPacketToClient(S2CPacket * packet, int roomNum)
{
	SOCKET client_sock[MAX_PLAYER];

	for (int i = 0; i < MAX_PLAYER; ++i){
		client_sock[i] = TeamList(roomNum,i).m_socket;
	}

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		//packet->SendTime = chrono::system_clock::now();
		send(client_sock[i], (char*)packet, sizeof(S2CPacket), 0);
		//printf("데이터 송신 %d번\n", i);
	}
}

int ServerFrameWork::Calculate(int roomNum, chrono::system_clock::time_point time)
{
	InfoPlayer* player;
	InfoBullet* bullet;
	InfoItem* item;
	int dead = 0;

	//	BuffPop
	if (!room[roomNum].m_buffQueue.empty())
	{
		BuffInfo* front;
		front = &room[roomNum].m_buffQueue.front();
		while (front->endcheck(time))
		{
			TeamList(front->roomIndex, front->PlayerID).m_player.m_state = normal;
			//printf("room:%d pid:%d buff해제\n", front->roomIndex, front->PlayerID);
			room[roomNum].m_buffQueue.pop();
			if (!room[roomNum].m_buffQueue.empty())
				front = &room[roomNum].m_buffQueue.front();
			else 
				break;
		}
	}

	//	CollideCheck
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		player = &TeamList(roomNum, i).m_player;

		//	PlayerCheck
		if (IsPlayerDead(player->m_hp))
		{
			//	플레이어가 한명 남은경우 게임을 종료
			if (++dead >= MAX_PLAYER - 1)
				return end_of_game;

			// 죽은 플레이어는 계산하지 않고 다음 플레이어를 본다.
			continue;
		}

		//	Bullet Check;
		for (int j = 0; j < MAX_PLAYER; ++j)
		{
			//	동일 플레이어 건너띔
			if (i == j)continue;

			//	BulletCheck
			for (int k = 0; k < MAX_BULLET; ++k)
			{
				bullet = &TeamList(roomNum, j).m_bullets[k];
				if (!IsExistBullet(bullet->m_damage))continue;

				//	RectCollide
				if (!rectCollideCheck(player->m_pos, PLAYERSIZE, bullet->m_pos, BULLETSIZE))
					continue;
	
				//	Collide Test ok
				player->m_hp -= bullet->m_damage;
				if (IsPlayerDead(player->m_hp))dead++;
				DestroyBullet(bullet);
			}

			//	ItemCheck
			for (int t = 0; t < MAX_ITEM; ++t)
			{
				item = &room[roomNum].m_itemList[t];
				if (item->m_type != notExist)
				{
					if (!rectCollideCheck(player->m_pos, PLAYERSIZE, item->m_pos, BULLETSIZE))
						continue;
					switch (item->m_type)
					{
					case medikit:
						player->m_hp += 1;
						clamp<int>(player->m_hp, 0, PLAYERMAXHP);
						item->m_type = notExist;
						//printf("room:%d player:%i get medikit\n", roomNum, i);
						break;
					case reinforce:
						player->m_state = burst;
						item->m_type = notExist;
						room[roomNum].m_buffQueue.push(BuffInfo(roomNum, i, BUFFDURATION));
						//printf("room:%d player:%i get reinforce\n", roomNum, i);
						break;
					default:
						break;
					}
				}
			}
		}
	}

	//EOG
	if (dead >= MAX_PLAYER - 1)
		return end_of_game;
	return ok;
}

inline void ServerFrameWork::FixFrame(int roomIndex)
{
	while (true) {
		room[roomIndex].Tick();
		if (THREADFREQ > room[roomIndex].m_ElapsedTime)
			Sleep(0);
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
		if (room[i].m_roomState == Lobby) {
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
	}
	return -1;
}


