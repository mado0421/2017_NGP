#pragma once
#include "Scene.h"
#include "Texture.h"


GLvoid initialize(GLvoid);
GLvoid run(GLvoid);

GLvoid renderScene(GLvoid);
GLvoid reshape(int w, int h);
GLvoid idle(GLvoid);
GLvoid mouseInput(int button, int state, int x, int y);
GLvoid keyDown(unsigned char key, int x, int y);
GLvoid keyUp(unsigned char key, int x, int y);
GLvoid specialKeyDown(int key, int x, int y);
GLvoid specialKeyUp(int key, int x, int y);

class Framework
{
private:
	Scene*	m_scenes[3];
	Scene*	m_currentScene;

	DWORD	prevTime;
	DWORD	curTime;
public:
	Texture *m_pTexture = NULL;
public:
	Framework();
	~Framework();

public:
	GLvoid initialize(int argc, char **argv);
	GLvoid run(GLvoid);

	GLvoid renderScene(GLvoid);
	GLvoid reshape(int w, int h);
	GLvoid idle(GLvoid);
	GLvoid mouseInput(int button, int state, int x, int y);
	GLvoid keyDown(unsigned char key, int x, int y);
	GLvoid keyUp(unsigned char key, int x, int y);
	GLvoid specialKeyDown(int key, int x, int y);
	GLvoid specialKeyUp(int key, int x, int y);

	bool changeScene(int idx, void* data);
};

