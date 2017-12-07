#include "h.h"
#include "Room.h"

#define SERVERPORT 9000
#define BUFSIZE 256

Room * g_testRoom;


int main(int argc, char *argv[])
{
	int retval;

	g_testRoom = new Room();
	g_testRoom->initialize();

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;

	//---------------------for test about single thread---------------
	int playerNum;
	//----------------------------------------------------------------

	while (1) {
		//accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}
		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		char msg[MSGSIZE];
		playerNum = g_testRoom->playerArrive(client_sock);
		/*플레이어 Number 보냄*/
		msg[0] = msg::TEAMNO;
		send(g_testRoom->m_teamList[playerNum].m_socket, msg, MSGSIZE, 0);
		if (retval == SOCKET_ERROR) return false;

		msg[0] = playerNum;
		send(g_testRoom->m_teamList[playerNum].m_socket, msg, MSGSIZE, 0);
		if (retval == SOCKET_ERROR) return false;

		printf("\n[TCP 로비] 클라이언트 슬롯: 슬롯 번호=%d\n",
			playerNum);

		while (1)
		{
			if (!g_testRoom->checkMsg(playerNum)) break;
		}

		g_testRoom->m_teamList[playerNum].m_socket = NULL;

		// closesocket()
		closesocket(client_sock);
		printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();

	delete g_testRoom;

	return 0;
}