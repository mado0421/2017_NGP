#pragma once
#include "Object.h"

class ObjectManager
{
private:
	std::vector<Player> m_playerList;
	//std::vector<Bullet> m_bulletList;
	Bullet m_myBulletList[MAX_BULLET];
	Bullet m_OtherBulletList[MAX_BULLET * 3];
	int m_bulletCount = 0;
	std::vector<Item>	m_itemList;
	std::vector<Tile>	m_tileList;
	int m_myTeamNo;
public:
	ObjectManager();
	~ObjectManager();

	void initialize();

	void addBullet(float x, float y, int team);
	void reloadAmmo(int team);
	void addItem(float x, float y);

	void changePlayerDirection(unsigned char key, int state, int team);
	int getPlayerDirection(unsigned char key, int team);

	void update(float elapsedTime);
	void render();

	void updatePlayerInfo(InfoPlayer* p, InfoBullet* b);
	void setPlayerNum(int number) { m_myTeamNo = number; }
};
