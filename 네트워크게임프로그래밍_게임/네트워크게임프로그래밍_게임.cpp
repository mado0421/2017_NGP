// 네트워크게임프로그래밍_게임.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Framework.h"

namespace
{
	Framework framework;
}

GLvoid initialize(int argc, char **argv);
GLvoid renderScene(GLvoid);
GLvoid reshape(int w, int h);
GLvoid idle(GLvoid);
GLvoid mouseInput(int button, int state, int x, int y);
GLvoid keyDown(unsigned char key, int x, int y);
GLvoid keyUp(unsigned char key, int x, int y);
GLvoid specialKeyDown(int key, int x, int y);
GLvoid specialKeyUp(int key, int x, int y);

int main(int argc, char **argv)
{
	framework.initialize(argc, argv);
	framework.run();

	return 0;
}

GLvoid initialize(int argc, char **argv)
{
	framework.initialize(argc, argv);
}

GLvoid renderScene(GLvoid)
{
	framework.renderScene();
}

GLvoid reshape(int w, int h)
{
	framework.reshape(w, h);
}

GLvoid idle(GLvoid)
{
	framework.idle();
}

GLvoid mouseInput(int button, int state, int x, int y)
{
	framework.mouseInput(button, state, x, y);
}

GLvoid keyDown(unsigned char key, int x, int y)
{
	framework.keyDown(key, x, y);
}

GLvoid keyUp(unsigned char key, int x, int y)
{
	framework.keyUp(key, x, y);
}

GLvoid specialKeyDown(int key, int x, int y)
{
	framework.specialKeyDown(key, x, y);
}

GLvoid specialKeyUp(int key, int x, int y)
{
	framework.specialKeyUp(key, x, y);
}
