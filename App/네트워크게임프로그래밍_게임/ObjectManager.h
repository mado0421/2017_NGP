#pragma once
#include "Object.h"
extern HANDLE hCommunicateEvent;
extern HANDLE hUpdateEvent;
extern C2SPacket c2spacket;
extern S2CPacket s2cpacket;

class Texture;

class ObjectManager
{
private:
	std::vector<Player> m_playerList;
	
	Bullet m_myBulletList[MAX_BULLET];
	Bullet m_OtherBulletList[MAX_BULLET * 3];
	
	Item m_itemList[MAX_ITEM];
	std::vector<Tile>	m_tileList;
	int m_myTeamNo;

	Texture *m_pTexture=NULL;

public:
	ObjectManager();
	~ObjectManager();

	void initialize(int team);
	void setTexture(Texture* pTexture);

	void addBullet(float x, float y, int team);
	void reloadAmmo(int team);
	void addItem(float x, float y);

	void changePlayerDirection(unsigned char key, int state, int team);
	int getPlayerDirection(unsigned char key, int team);

	void update(float elapsedTime);
	void render();

	void updatePlayerInfo();
	void updatePlayerInfoFirst();
	void setPlayerNum(int number) { m_myTeamNo = number; }
	Bullet* getBulletList() { return m_myBulletList; }
};