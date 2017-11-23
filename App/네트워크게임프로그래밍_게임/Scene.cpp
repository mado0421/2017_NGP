#include "stdafx.h"
#include "Scene.h"


Scene::Scene()
{
}

Scene::~Scene()
{
}

//////////////////////////////////////////////////////////////////////////

PlayScene::PlayScene()
{
}

PlayScene::~PlayScene()
{
	delete m_objMng;
}

void PlayScene::initialize()
{
	m_objMng = new ObjectManager();
	m_objMng->initialize();
}

void PlayScene::leave()
{
}

void PlayScene::update(float elapsedTime)
{
	m_objMng->update(elapsedTime);
}

void PlayScene::render()
{
	m_objMng->render();
}

void PlayScene::mouseInput(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		m_objMng->addBullet(x, y, PLAYER_0);
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		m_objMng->reloadAmmo(PLAYER_0);
	}
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		/*for Test*/


	}
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
		m_objMng->changePlayerDirection('y', 1, PLAYER_0);
		break;
	case 's':
	case 'S':
		m_objMng->changePlayerDirection('y', -1, PLAYER_0);
		break;
	case 'a':
	case 'A':
		m_objMng->changePlayerDirection('x', -1, PLAYER_0);
		break;
	case 'd':
	case 'D':
		m_objMng->changePlayerDirection('x', 1, PLAYER_0);
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
		if (1 == m_objMng->getPlayerDirection('y', PLAYER_0))
			m_objMng->changePlayerDirection('y', 0, PLAYER_0);
		break;
	case 's':
	case 'S':
		if (-1 == m_objMng->getPlayerDirection('y', PLAYER_0))
			m_objMng->changePlayerDirection('y', 0, PLAYER_0);
		break;
	case 'a':
	case 'A':
		if (-1 == m_objMng->getPlayerDirection('x', PLAYER_0))
			m_objMng->changePlayerDirection('x', 0, PLAYER_0);
		break;
	case 'd':
	case 'D':
		if (1 == m_objMng->getPlayerDirection('x', PLAYER_0))
			m_objMng->changePlayerDirection('x', 0, PLAYER_0);
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

//////////////////////////////////////////////////////////////////////////

TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::initialize()
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

LobbyScene::~LobbyScene()
{

}

void LobbyScene::initialize()
{
	m_lightOn = false;
	m_connected = false;

	// 윈속 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) exit(1);

	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");


}

void LobbyScene::leave()
{	
	//closesocket()
	closesocket(sock);
}

void LobbyScene::update(float elapsedTime)
{
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
			m_connected = true;
			// connet()
			ZeroMemory(&serveraddr, sizeof(serveraddr));
			serveraddr.sin_family = AF_INET;
			serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
			serveraddr.sin_port = htons(SERVERPORT);
			retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
		}
		else
		{
			sendMsg(PLAYER_0, 0);
			checkMsg();
		}
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
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
	retval = recvn(sock, msg, MSGSIZE, 0);
	if (retval == 0) return false;
	switch (msg[0])
	{
	case 0x00:
		m_lightOn = true;
		break;
	case 0x01:
		m_lightOn = true;

		break;
	case 0x02:
		m_lightOn = true;

		break;
	default:
		return false;
	}
	return true;
}

void LobbyScene::sendMsg(int player, int type)
{
	char msg[MSGSIZE];
	switch (type)
	{
	case 0:
		msg[0] = 0x01;
		break;
	case 1:
		msg[0] = 0x02;
		break;
	default:
		break;
	}
	send(sock, msg, MSGSIZE, 0);
	return;
}
