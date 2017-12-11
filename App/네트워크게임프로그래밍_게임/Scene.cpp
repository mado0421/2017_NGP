#include "stdafx.h"
#include "Scene.h"
#include "Framework.h"
std::chrono::system_clock::time_point start;
std::chrono::duration<double> sec;


Scene::Scene()
{
}

Scene::Scene(Framework * pFramework)
{
	m_pFramework = pFramework;
}

Scene::~Scene()
{
}

//////////////////////////////////////////////////////////////////////////

TitleScene::TitleScene()
{
}

TitleScene::TitleScene(Framework * pFramework)
{
	m_pFramework = pFramework;
}

TitleScene::~TitleScene()
{
}

void TitleScene::initialize(void* data)
{

}

void TitleScene::leave()
{
}

void TitleScene::update(float elapsedTime)
{
}

void TitleScene::render()
{

}

void TitleScene::mouseInput(int button, int state, int x, int y)
{
}

void TitleScene::keyDown(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'r':
	case 'R':
		changeScene(SceneType::Lobby);
		break;
	default:
		break;
	}
}

void TitleScene::keyUp(unsigned char key, int x, int y)
{
}

void TitleScene::specialKeyDown(int key, int x, int y)
{
}

void TitleScene::specialKeyUp(int key, int x, int y)
{
}


//////////////////////////////////////////////////////////////////////////

LobbyScene::LobbyScene()
{
}

LobbyScene::LobbyScene(Framework * pFramework)
{
	m_pFramework = pFramework;
}

LobbyScene::~LobbyScene()
{

}

void LobbyScene::initialize(void* data)
{
	m_lightOn = false;
	m_connected = false;
	m_networkData = new NetworkData();

	// 윈속 초기화
	if (WSAStartup(MAKEWORD(2, 2), &m_networkData->wsa) != 0) exit(1);

	// socket()
	m_networkData->sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_networkData->sock == INVALID_SOCKET) err_quit("socket()");
}

void LobbyScene::leave()
{	
	
}

void LobbyScene::update(float elapsedTime)
{
	//if (m_connected && m_lightOn)
	//{
	//	//char msg[MSGSIZE];
	//	//msg[0] = msg::STARTPLAY;
	//	//send(m_networkData->sock, msg, MSGSIZE, 0);
	//	if (!checkMsg()) {
	//		std::cout << "checkMsg()에서 문제!" << std::endl;
	//		exit(1);
	//	}
	//}
	if(m_networkData->state==waitState::success) changeScene(SceneType::Play, m_networkData);
}

void LobbyScene::render()
{
	glPushMatrix();
	glTranslatef(WWIDTH / 2.0, WHEIGHT / 2.0, 0);
	glColor3f(0.1f, 0.1f, 0.1f);
	glutSolidSphere(120.0f, 32, 8);

	if (m_lightOn) glColor3f(0.0f, 1.0f, 0.0f);
	else if(m_connected) glColor3f(1.0f, 0.0f, 0.0f);
	else glColor3f(0.2f, 0.2f, 0.2f);
	glutSolidSphere(100.0f, 32, 8);

	glPopMatrix();
}

void LobbyScene::mouseInput(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (!m_connected)
		{
			if (!accessLobby()) {
				std::cout << "accessLobby()에서 문제!" << std::endl;
				exit(1);
			}
			if (!checkMsg()) {
				std::cout << "checkMsg()에서 문제!" << std::endl;
				exit(1);
			}
			hThread = CreateThread(NULL, 0, waitThreadFunc, (void*)m_networkData, 0, NULL);

			m_connected = true;
			m_lightOn = true;

		}
		//else if (m_connected && m_lightOn)
		//{
		//	char msg[MSGSIZE];
		//	msg[0] = msg::STARTPLAY;
		//	send(m_networkData->sock, msg, MSGSIZE, 0);
		//	if (!checkMsg()) {
		//		std::cout << "checkMsg()에서 문제!" << std::endl;
		//		exit(1);
		//	}
		//}
		//else
		//{
		//	//char msg[MSGSIZE];
		//	//msg[0] = msg::TEST;
		//	//send(m_networkData->sock, msg, MSGSIZE, 0);
		//	//if (!checkMsg()) {
		//	//	std::cout << "checkMsg()에서 문제!" << std::endl;
		//	//	exit(1);
		//	//}
		//}
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		if (m_connected)
		{
			leaveServer();
		}
	}
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
	}
}

void LobbyScene::keyDown(unsigned char key, int x, int y)
{
}

void LobbyScene::keyUp(unsigned char key, int x, int y)
{
}

void LobbyScene::specialKeyDown(int key, int x, int y)
{
}

void LobbyScene::specialKeyUp(int key, int x, int y)
{
}

bool LobbyScene::checkMsg()
{
	char msg[MSGSIZE];
	retval = recvn(m_networkData->sock, msg, MSGSIZE, 0);
	if (retval == SOCKET_ERROR) return false;
	switch (msg[0])
	{
	case msg::TEAMNO:

		retval = recvn(m_networkData->sock, msg, MSGSIZE, 0);
		if (retval == SOCKET_ERROR) return false;
		m_networkData->m_myTeamNo = msg[0];
		return true;

	//case msg::ISREADY:
	//	msg[0] = msg::OK;
	//	retval = send(m_networkData->sock, msg, MSGSIZE, 0);
	//	if (retval == SOCKET_ERROR) return false;
	//	return true;

	//case msg::STARTPLAY:
	//	msg[0] = msg::OK;
	//	retval = send(m_networkData->sock, msg, MSGSIZE, 0);
	//	if (retval == SOCKET_ERROR) return false;
	//	changeScene(SceneType::Play, m_networkData);
	//	return true;

	case msg::TEST:
		printf("이걸 나한테 왜 보내?\n");
		return true;

	case msg::OK:
		return true;

	default:
		return false;
	}
	return false;
}
#define AUTOIP
bool LobbyScene::accessLobby()
{
	char ipAddr[256];

#ifndef AUTOIP
	printf("주소입력\n");
	scanf("%s", ipAddr);
#else
	strcpy(ipAddr, "127.0.0.1");
#endif
	// connet()
	ZeroMemory(&m_networkData->serveraddr, sizeof(m_networkData->serveraddr));
	m_networkData->serveraddr.sin_family = AF_INET;
	m_networkData->serveraddr.sin_addr.s_addr = inet_addr(ipAddr);
	m_networkData->serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(m_networkData->sock, (SOCKADDR *)&m_networkData->serveraddr, sizeof(m_networkData->serveraddr));
	if (retval == SOCKET_ERROR) return false;

	return true;
}

void LobbyScene::leaveServer()
{
	char msg[MSGSIZE];
	msg[0] = msg::LEAVE;
	send(m_networkData->sock, msg, MSGSIZE, 0);
	closesocket(m_networkData->sock);
}

void Scene::changeScene(int idx, void *data)
{
	m_pFramework->changeScene(idx, data);
}

//////////////////////////////////////////////////////////////////////////

PlayScene::PlayScene()
{
}

PlayScene::PlayScene(Framework * pFramework)
{
	m_pFramework = pFramework;
}

PlayScene::~PlayScene()
{
	delete m_objMng;
}

void PlayScene::initialize(void* data)
{
	if (data != NULL)
	{
		m_networkData = (NetworkData*)data;
	}
	else
	{
		changeScene(SceneType::Title);
	}
	m_myTeam_No = m_networkData->m_myTeamNo;

	m_objMng = new ObjectManager();
	m_objMng->initialize(m_networkData->m_myTeamNo);
	hCommunicateEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	hUpdateEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	hThread = CreateThread(NULL, 0, communicateThreadFunc, (void*)this, 0, NULL);
}

void PlayScene::leave()
{

}

void PlayScene::update(float elapsedTime)
{
	m_objMng->update(elapsedTime);
	SetEvent(hCommunicateEvent);
	ResetEvent(hUpdateEvent);
}

void PlayScene::render()
{
	m_objMng->render();
	WaitForSingleObject(hUpdateEvent, 1000);
}

void PlayScene::mouseInput(int button, int state, int x, int y)
{
	printf("mouse wait 전\n");
	WaitForSingleObject(hUpdateEvent, INFINITE);
	printf("mouse wait 후\n");
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		m_objMng->addBullet(x, y, m_myTeam_No);
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		m_objMng->reloadAmmo(m_myTeam_No);
	}
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		/*for Test*/
	}
	SetEvent(hCommunicateEvent);
	ResetEvent(hUpdateEvent);
	
}

void PlayScene::keyDown(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'r':
	case 'R':
		//		m_objMng->reloadAmmo(PLAYER_0);
		break;
	case 'w':
	case 'W':
		m_objMng->changePlayerDirection('y', 1, m_myTeam_No);
		break;
	case 's':
	case 'S':
		m_objMng->changePlayerDirection('y', -1, m_myTeam_No);
		break;
	case 'a':
	case 'A':
		m_objMng->changePlayerDirection('x', -1, m_myTeam_No);
		break;
	case 'd':
	case 'D':
		m_objMng->changePlayerDirection('x', 1, m_myTeam_No);
		break;
	case 'P':
	case 'p':
		Sleep(2000);
		break;
	default:
		break;
	}
}

void PlayScene::keyUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
	case 'W':
		/*w키가 눌려있을 때 w키를 떼면 방향 값을 0으로 한다.*/
		if (1 == m_objMng->getPlayerDirection('y', m_myTeam_No))
			m_objMng->changePlayerDirection('y', 0, m_myTeam_No);
		break;
	case 's':
	case 'S':
		if (-1 == m_objMng->getPlayerDirection('y', m_myTeam_No))
			m_objMng->changePlayerDirection('y', 0, m_myTeam_No);
		break;
	case 'a':
	case 'A':
		if (-1 == m_objMng->getPlayerDirection('x', m_myTeam_No))
			m_objMng->changePlayerDirection('x', 0, m_myTeam_No);
		break;
	case 'd':
	case 'D':
		if (1 == m_objMng->getPlayerDirection('x', m_myTeam_No))
			m_objMng->changePlayerDirection('x', 0, m_myTeam_No);
		break;
	default:
		break;
	}
}

void PlayScene::specialKeyDown(int key, int x, int y)
{
}

void PlayScene::specialKeyUp(int key, int x, int y)
{
}


//InfoPlayer p[4];
//InfoBullet b[72];

DWORD WINAPI communicateThreadFunc(LPVOID arg)
{
	PlayScene* playScene = (PlayScene*)arg;
	int retval;
	start = std::chrono::system_clock::now();
	SOCKET sock = playScene->getNetworkData()->sock;
	int num = playScene->getNetworkData()->m_myTeamNo;
	playScene->getObjectManager()->updatePlayerInfoFirst();
	//이건 씬 생성시로 옮겨도 될 듯
	send(sock, (char*)&c2spacket, sizeof(C2SPacket), 0);
	retval = recvn(sock, (char*)&s2cpacket, sizeof(S2CPacket), 0);
	ResetEvent(hCommunicateEvent);
	SetEvent(hUpdateEvent);
	while (1)
	{
		
		sec = std::chrono::system_clock::now() - start;
		//if (sec.count()>1 / 60)
		{
			printf("communicate wait 전\n");
			WaitForSingleObject(hCommunicateEvent, INFINITE);
			printf("communicate wait 후\n");
			send(sock, (char*)&c2spacket, sizeof(C2SPacket), 0);
			retval = recvn(sock, (char*)&s2cpacket, sizeof(S2CPacket), 0);
			if (retval == SOCKET_ERROR)
			{
				ResetEvent(hCommunicateEvent);
				SetEvent(hUpdateEvent);
				playScene->changeScene(SceneType::Title);
				ExitThread(0);
			}
			switch (s2cpacket.Message)
			{
			case DATA:
				start = std::chrono::system_clock::now();
				ResetEvent(hCommunicateEvent);
				SetEvent(hUpdateEvent);
				
				break;
			case STARTGAME:
				break;
			case ENDGAME:
				ResetEvent(hCommunicateEvent);
				SetEvent(hUpdateEvent);
				playScene->changeScene(SceneType::Title);
				ExitThread(0);
				break;
			}
		}
	}

	return 0;
}

DWORD waitThreadFunc(LPVOID arg)
{
	char msg[MSGSIZE];
	int retval;
	NetworkData *np = (NetworkData*)arg;
	while (true)
	{
		retval = recvn(np->sock, msg, MSGSIZE, 0);
		if (retval == SOCKET_ERROR) break;
		switch (msg[0])
		{
		case msg::ISREADY:
			msg[0] = msg::OK;
			retval = send(np->sock, msg, MSGSIZE, 0);
			if (retval == SOCKET_ERROR) break;
			break;

		case msg::STARTPLAY:
			msg[0] = msg::OK;
			retval = send(np->sock, msg, MSGSIZE, 0);
			if (retval == SOCKET_ERROR) break;
			np->state = waitState::success;
			goto end;
			break;
		default:
			break;
		}
	}
end:

	return 0;
}
