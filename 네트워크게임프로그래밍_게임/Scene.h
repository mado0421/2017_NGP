#pragma once
#include "Object.h"

#define MAX_KEY 256

class Scene
{
private:
public:
	Scene();
	~Scene();
public:
	virtual void initialize() = 0;

	virtual void update(float elapsedTime) = 0;
	virtual void render() = 0;

	virtual void mouseInput(int button, int state, int x, int y) = 0;
	virtual void keyDown(unsigned char key, int x, int y) = 0;
	virtual void keyUp(unsigned char key, int x, int y) = 0;
	virtual void specialKeyDown(int key, int x, int y) = 0;
	virtual void specialKeyUp(int key, int x, int y) = 0;
};

class TitleScene : public Scene
{
private:
public:
	TitleScene();
	~TitleScene();
public:
	virtual void initialize();

	virtual void update(float elapsedTime);
	virtual void render();

	virtual void mouseInput(int button, int state, int x, int y);
	virtual void keyDown(unsigned char key, int x, int y);
	virtual void keyUp(unsigned char key, int x, int y);
	virtual void specialKeyDown(int key, int x, int y);
	virtual void specialKeyUp(int key, int x, int y);
};

class LobbyScene : public Scene
{
private:
public:
	LobbyScene();
	~LobbyScene();
public:
	virtual void initialize();

	virtual void update(float elapsedTime);
	virtual void render();

	virtual void mouseInput(int button, int state, int x, int y);
	virtual void keyDown(unsigned char key, int x, int y);
	virtual void keyUp(unsigned char key, int x, int y);
	virtual void specialKeyDown(int key, int x, int y);
	virtual void specialKeyUp(int key, int x, int y);
};

class PlayScene : public Scene
{
private:
	ObjectManager* m_objMng;
public:
	PlayScene();
	~PlayScene();
public:
	virtual void initialize();

	virtual void update(float elapsedTime);
	virtual void render();

	virtual void mouseInput(int button, int state, int x, int y);
	virtual void keyDown(unsigned char key, int x, int y);
	virtual void keyUp(unsigned char key, int x, int y);
	virtual void specialKeyDown(int key, int x, int y);
	virtual void specialKeyUp(int key, int x, int y);
};