#pragma once
#include "ObjectManager.h"

#define MAX_KEY 256

class Scene
{
private:
public:
	Scene();
	~Scene();
public:
	virtual void initialize() = 0;
	virtual void leave() = 0;

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
	virtual void leave();

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
	SOCKADDR_IN serveraddr;
	WSADATA		wsa;
	SOCKET		sock;
	int			retval;
	char		ipAddr[BUFSIZE];

	bool		m_lightOn;
	bool		m_connected;
public:
	LobbyScene();
	~LobbyScene();
public:
	virtual void initialize();
	virtual void leave();

	virtual void update(float elapsedTime);
	virtual void render();

	virtual void mouseInput(int button, int state, int x, int y);
	virtual void keyDown(unsigned char key, int x, int y);
	virtual void keyUp(unsigned char key, int x, int y);
	virtual void specialKeyDown(int key, int x, int y);
	virtual void specialKeyUp(int key, int x, int y);

	virtual bool checkMsg();
	virtual void sendMsg(int player, int type);
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
	virtual void leave();

	virtual void update(float elapsedTime);
	virtual void render();

	virtual void mouseInput(int button, int state, int x, int y);
	virtual void keyDown(unsigned char key, int x, int y);
	virtual void keyUp(unsigned char key, int x, int y);
	virtual void specialKeyDown(int key, int x, int y);
	virtual void specialKeyUp(int key, int x, int y);
};