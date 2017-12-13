#pragma once
#include "ObjectManager.h"

#define MAX_KEY 256
//extern HANDLE hCommunicateEvent;
//extern HANDLE hUpdateEvent;
//extern C2SPacket c2spacket;
//extern S2CPacket s2cpacket;
class Framework;

class Scene
{
protected:
	Framework *m_pFramework;
public:
	Scene();
	Scene(Framework *pFramework);
	~Scene();
public:
	virtual void initialize(void* data = nullptr) = 0;
	virtual void leave() = 0;

	virtual void update(float elapsedTime) = 0;
	virtual void render() = 0;

	virtual void mouseInput(int button, int state, int x, int y) = 0;
	virtual void keyDown(unsigned char key, int x, int y) = 0;
	virtual void keyUp(unsigned char key, int x, int y) = 0;
	virtual void specialKeyDown(int key, int x, int y) = 0;
	virtual void specialKeyUp(int key, int x, int y) = 0;

	virtual void changeScene(int idx, void *data = nullptr);
	
};

class TitleScene : public Scene
{
private:
	int m_nIpAddrIcon = 0;
	bool m_bNumpadIconHighlight[3][4];
	bool m_bButtonIconHighlight[2];
	char m_aIpAddr[16];
public:
	NetworkData *m_networkData = NULL;
public:
	TitleScene();
	TitleScene(Framework *pFramework);
	~TitleScene();
public:
	virtual void initialize(void* data = nullptr);
	virtual void leave();

	virtual void update(float elapsedTime);
	virtual void render();

	virtual void mouseInput(int button, int state, int x, int y);
	virtual void keyDown(unsigned char key, int x, int y);
	virtual void keyUp(unsigned char key, int x, int y);
	virtual void specialKeyDown(int key, int x, int y);
	virtual void specialKeyUp(int key, int x, int y);

	virtual bool accessLobby();

};

class LobbyScene : public Scene
{
private:
public:
	NetworkData *m_networkData = NULL;
	HANDLE			hThread;

	int			retval;
	//--------------------for Test----------------------
	bool		m_lightOn;
	bool		m_connected;
	//--------------------------------------------------
public:
	LobbyScene();
	LobbyScene(Framework *pFramework);
	~LobbyScene();
public:
	virtual void initialize(void* data = nullptr);
	virtual void leave();

	virtual void update(float elapsedTime);
	virtual void render();

	virtual void mouseInput(int button, int state, int x, int y);
	virtual void keyDown(unsigned char key, int x, int y);
	virtual void keyUp(unsigned char key, int x, int y);
	virtual void specialKeyDown(int key, int x, int y);
	virtual void specialKeyUp(int key, int x, int y);

	virtual bool checkMsg();

	virtual void leaveServer();

	
};

class PlayScene : public Scene
{
private:
	NetworkData		*m_networkData = NULL;
	ObjectManager*  m_objMng = NULL;
	HANDLE			hThread;
	int				m_myTeam_No = 1;
public:
	PlayScene();
	PlayScene(Framework *pFramework);
	~PlayScene();
public:
	virtual void initialize(void* data = nullptr);
	virtual void leave();

	virtual void update(float elapsedTime);
	virtual void render();

	virtual void mouseInput(int button, int state, int x, int y);
	virtual void keyDown(unsigned char key, int x, int y);
	virtual void keyUp(unsigned char key, int x, int y);
	virtual void specialKeyDown(int key, int x, int y);
	virtual void specialKeyUp(int key, int x, int y);
	NetworkData* getNetworkData() { return m_networkData; }
	ObjectManager* getObjectManager() { return m_objMng; }
};

DWORD WINAPI communicateThreadFunc(LPVOID arg);
DWORD WINAPI waitThreadFunc(LPVOID arg);