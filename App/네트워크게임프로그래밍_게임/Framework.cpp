#include "stdafx.h"
#include "Framework.h"


Framework::Framework()
{
}


Framework::~Framework()
{
}

GLvoid Framework::initialize(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WWIDTH, WHEIGHT);
	glutCreateWindow("네트워크 게임 프로그래밍 텀프로젝트");

	glewInit();
	if (glewIsSupported("GL_VERSION_3_0"))
		std::cout << " GLEW Version is 3.0\n ";
	else std::cout << "GLEW 3.0 not supported\n ";
	
	prevTime = timeGetTime();

	m_scenes[SceneType::Title] = new TitleScene();
	m_scenes[SceneType::Lobby] = new LobbyScene();
	m_scenes[SceneType::Play] = new PlayScene();

	m_currentScene = m_scenes[SceneType::Lobby];
//	m_currentScene = m_scenes[SceneType::Play];
	m_currentScene->initialize();
}

GLvoid Framework::run(GLvoid)
{
	glutDisplayFunc(::renderScene);
	glutIdleFunc(::idle);
	glutReshapeFunc(::reshape);
	glutKeyboardFunc(::keyDown);
	glutSpecialFunc(::specialKeyDown);
	glutKeyboardUpFunc(::keyUp);
	glutSpecialUpFunc(::specialKeyUp);
	glutMouseFunc(::mouseInput);
	glutMainLoop();
}

GLvoid Framework::renderScene(GLvoid)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.7f, 0.7f, 0.9f, 1.0f);

	m_currentScene->render();

	curTime = timeGetTime();

	float elapsedTime = (curTime - prevTime)*0.001;
	if (elapsedTime > (1.0 / 60))
	{
		m_currentScene->update(elapsedTime);
		prevTime = curTime;
	}

	glutSwapBuffers();
}

GLvoid Framework::reshape(int w, int h)
{
	glLoadIdentity();

	glViewport(0, 0, w, h);

	glOrtho(0, w, 0, h, -1000.0, 1000.0);

	gluLookAt(0.0f, 0.0f, 100.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);
	glMatrixMode(GL_MODELVIEW);
}

GLvoid Framework::idle(GLvoid)
{
	renderScene();
}

GLvoid Framework::mouseInput(int button, int state, int x, int y)
{
	m_currentScene->mouseInput(button, state, x, WHEIGHT - y);

	renderScene();
}

GLvoid Framework::keyDown(unsigned char key, int x, int y)
{
	m_currentScene->keyDown(key, x, y);

	renderScene();
}

GLvoid Framework::keyUp(unsigned char key, int x, int y)
{
	m_currentScene->keyUp(key, x, y);

	renderScene();
}

GLvoid Framework::specialKeyDown(int key, int x, int y)
{
	m_currentScene->specialKeyDown(key, x, y);

	renderScene();
}

GLvoid Framework::specialKeyUp(int key, int x, int y)
{
	m_currentScene->specialKeyUp(key, x, y);

	renderScene();
}

bool Framework::changeScene(int idx)
{
	switch (idx)
	{
	case SceneType::Title:
		m_currentScene->leave();
		m_currentScene = m_scenes[SceneType::Title];
		break;
	case SceneType::Lobby:
		m_currentScene->leave();
		m_currentScene = m_scenes[SceneType::Lobby];
		break;
	case SceneType::Play:
		m_currentScene->leave();
		m_currentScene = m_scenes[SceneType::Play];
		break;
	default:
		return false;
	}
	m_currentScene->initialize();
	return true;
}
