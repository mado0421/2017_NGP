// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// Net_TermProject_Server.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0)
	{
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}
// TODO: 필요한 추가 헤더는
// 이 파일이 아닌 STDAFX.H에서 참조합니다.

bool Room::playerArrive(SOCKET& socket)
{
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		if (m_teamList[i].m_socket == NULL)
		{
			m_teamList[i].m_socket = socket;
			int retval;
			char msg[MSGSIZE];
			retval = recvn(m_teamList[i].m_socket, msg, MSGSIZE, 0);
			if (retval == SOCKET_ERROR) return false;
			if (msg[0] == msg::OK)
			{
				msg[0] = msg::TEAMNO;
				retval = send(socket, msg, MSGSIZE, 0);
				if (retval == SOCKET_ERROR) return false;
				msg[0] = i;
				retval = send(socket, msg, MSGSIZE, 0);
				if (retval == SOCKET_ERROR) return false;
				return true;
			}
			return false;
		}
	}
	return false;
}
