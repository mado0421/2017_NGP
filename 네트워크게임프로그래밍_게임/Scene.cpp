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
}

void LobbyScene::update(float elapsedTime)
{
}

void LobbyScene::render()
{
}

void LobbyScene::mouseInput(int button, int state, int x, int y)
{
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
