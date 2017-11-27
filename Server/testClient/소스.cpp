#pragma comment(lib,"ws2_32")
#include"stdafx.h"

#define SERVERIP	"127.0.0.1"
#define	SERVERPORT	9000
#define BUFSIZE		512

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

//	사용자 정의 데이터 수신 함수
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


int main(int argc, char* argv[])
{
	int retval;
	int pi;
	::printf("플레이어 입력: ");
	scanf("%d", &pi);
	//	윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	//	socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)err_quit("socket()");

	//	connect()

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(SOCKADDR_IN));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)err_quit("connect()");

	//	데이터 통신에 사용할 변수
	char buf[BUFSIZE + 1];
	int len;
	InfoPlayer player[4];
	InfoBullet bullets[4][MAX_BULLET];

	S2CPacket s2cpacket;
	C2SPacket c2spacket;

	player[pi].m_hp = 10 + pi;
	player[pi].m_pos = Vector2D(100+pi, 100+pi);
	//	서버와 데이터 통신
	while (1) {
		int len;
		memcpy(&s2cpacket, &player[pi], sizeof(InfoPlayer));

		retval = send(sock, (char*)&s2cpacket, sizeof(S2CPacket), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}
		::printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);

		//	데이터 받기
		retval = recvn(sock, (char*)&s2cpacket, sizeof(S2CPacket), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		//	받은 데이터 출력
		buf[retval] = '\0';
		::printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", retval);
		::printf("[받은 데이터] %s\n", buf);
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			::printf("[%d]  hp = %d", i, s2cpacket.iPlayer[i].m_hp);
		}
	}

	//	close_socket()
	closesocket(sock);

	//	윈속 종료
	WSACleanup();
	::printf("프로그램이 종료됨.\n");
	_sleep(2000);
	return 0;
}