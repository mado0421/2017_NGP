#include "Room.h"
#include "h.h"



Room::Room()
{
}


Room::~Room()
{
}

void Room::initialize()
{
	/*for Test*/
}

void Room::playerArrive(const SOCKET &client_sock)
{
	/*for Test*/
	m_teamList[0].m_socket = client_sock;

}

bool Room::checkMsg(char* msg)
{
	if (msg[0] == NULL) return false;
	switch (msg[0])
	{
	case 0x00:
		printf("0x00\n");
		break;
	case 0x01:
		printf("0x01\n");
		break;
	case 0x02:
		printf("0x02\n");
		break;
	default:
		return false;
	}
	return true;
}

void Room::sendMsg(int player, int type)
{
	char msg[MSGSIZE];
	switch (type)
	{
	case 0:
		msg[0] = 0x00;
		break;
	case 1:
		msg[0] = 0x01;
		break;
	case 2:
		msg[0] = 0x02;
		break;
	default:
		break;
	}
	send(m_teamList[player].m_socket, msg, MSGSIZE, 0);
	return;
}
