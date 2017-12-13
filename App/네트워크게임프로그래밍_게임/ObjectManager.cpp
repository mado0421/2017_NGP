#include "stdafx.h"
#include "ObjectManager.h"
#include "Texture.h"

C2SPacket c2spacket;
S2CPacket s2cpacket;
HANDLE			hCommunicateEvent;
HANDLE			hUpdateEvent;
ObjectManager::ObjectManager()
{

}

ObjectManager::~ObjectManager()
{
}

struct wall
{
	bool isSolid;
	int texIdx;
	int x;
	int y;
};

void ObjectManager::initialize(int team)
{
	m_myTeamNo = team;
	///*여기서 Wall 맵을 받아서 그걸 넣어줘야 함*/
	std::ifstream mapFile("assets/maps/map_1.map");
	if (mapFile.is_open())
	{
		int type;
		int texIdx;
		int x;
		int y;
		while (!mapFile.eof())
		{
			mapFile >> type >> texIdx >> x >> y;
			m_tileList.emplace_back(m_pTexture, Vector2D(
				x * TILESIZE * 2.0 + TILESIZE,
				y* TILESIZE * 2.0 + TILESIZE),
				type, texIdx);
		}
		m_tileList.pop_back();
	}
	mapFile.close();

	int i = 0;
	for (auto tp = m_tileList.cbegin(); tp != m_tileList.cend() && i < 4; ++tp)
	{
		if (tp->getType() == tile::PSpawn)
		{
			m_playerList.emplace_back(m_pTexture ,3, tp->getPos(), PLAYERSPD, PLAYERSIZE, i++);
		}
	}
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		m_playerList[i].setState(1);
	}
	
	for (int i = 0; i < MAX_BULLET; ++i)
	{
		m_myBulletList[i].setPos(Vector2D(0, 0));
		m_myBulletList[i].setDirection(Vector2D(0, 0));
		m_myBulletList[i].setTexture(m_pTexture);
		m_myBulletList[i].setDamage(0);
		m_myBulletList[i].setSize(2.5f);
		m_myBulletList[i].setHp(1);
		m_myBulletList[i].setTexture(m_pTexture);
	}
	for (int i = 0; i < MAX_BULLET * 3; ++i)
	{
		m_OtherBulletList[i].setSize(2.5f);
		m_OtherBulletList[i].setHp(1);
		m_OtherBulletList[i].setTexture(m_pTexture);
	}

	for (int i = 0; i < MAX_ITEM; ++i)
	{
		m_itemList[i].setTexture(m_pTexture);
	}

}

void ObjectManager::setTexture(Texture * pTexture)
{
	m_pTexture = pTexture;
}

void ObjectManager::addBullet(float x, float y, int team)
{
	
	for (auto p = m_playerList.begin(); p != m_playerList.end(); ++p)
	{
		if (p->getTeam() == team)
		{
			if (p->isCanFire()&&p->getHp()>0)
			{
				/*m_bulletList.emplace_back(p->getPos(),
				Vector::normalize(Vector2D(Vector::sub(p->getPos(), Vector2D(x, y)))), PLAYER_0);*/
				for (int i = 0; i < MAX_BULLET; ++i)
				{
					if (m_myBulletList[i].getDamage() == 0)
					{
						m_myBulletList[i] = Bullet(m_pTexture, p->getPos(),
							Vector::normalize(Vector2D(Vector::sub(p->getPos(), Vector2D(x, y)))), PLAYER_0, p->getState());
						//b[m_myTeamNo*MAX_BULLET + i].m_pos = m_myBulletList[i].getPos();
						break;
					}
				}
				p->fire();
			}
			break;
		}
	}
	updatePlayerInfoFirst();
}

void ObjectManager::reloadAmmo(int team)
{
	for (auto p = m_playerList.begin(); p != m_playerList.end(); ++p)
	{
		if (p->getTeam() == team)
		{
			if (!p->isCanFire()) p->reload();
			break;
		}
	}
}

void ObjectManager::changePlayerDirection(unsigned char key, int state, int team)
{
	for (auto p = m_playerList.begin(); p != m_playerList.end(); ++p)
	{
		if (p->getTeam() == team)
		{
			p->changeMoveDirection(key, state);
			break;
		}
	}
}

int ObjectManager::getPlayerDirection(unsigned char key, int team)
{
	for (auto p = m_playerList.begin(); p != m_playerList.end(); ++p)
	{
		if (p->getTeam() == team)
		{
			return p->getMoveDirection(key);
		}
	}
}

void ObjectManager::update(float elapsedTime)
{
	for (auto p = m_playerList.begin(); p != m_playerList.end(); ++p)
	{
		/*충돌검사*/
		int team = p->getTeam();
		for (auto tp = m_tileList.cbegin(); tp != m_tileList.cend(); ++tp)
		{
			if (tp->getType() == tile::Wall)
			{
				if (p->isCollideRect(*tp))
				{
					p->changeDirByCollide(*tp);
				}
			}
		}
		/*체력검사*/
		/*if (p->isDead())
		{
			p = m_playerList.erase(p);
			if (p == m_playerList.end()) break;
			continue;
		}*/

		/*업데이트*/
		p->update(elapsedTime);
	}

	for (int i = 0; i<MAX_BULLET; ++i)
	{
		for (auto tp = m_tileList.cbegin(); tp != m_tileList.cend(); ++tp)
		{
			if (tp->getType() == tile::Wall)
			{
				if (m_myBulletList[i].isCollideRect(*tp))
				{
					m_myBulletList[i].addHp(-1);
				}
			}
		}

		if (m_myBulletList[i].isDead())
		{
			m_myBulletList[i].setDamage(0);
			continue;
		}
		if (m_myBulletList[i].isOut())
		{
			m_myBulletList[i].setDamage(0);
			continue;
		}
		m_myBulletList[i].update(elapsedTime);
	}
	updatePlayerInfo();
	//for (auto p = m_itemList.begin(); p != m_itemList.end(); ++p)
	//{
	//	if (p->isDead())
	//	{
	//		p = m_itemList.erase(p);
	//		if (p == m_itemList.end()) break;
	//		continue;
	//	}

	//	p->update(elapsedTime);
	//}
	//통신 쓰레드 이벤트 생성 및 깨우기
}

void ObjectManager::render()
{
	for (auto p = m_playerList.cbegin(); p != m_playerList.cend(); ++p)if(p->getHp()>0) p->render();
	for (int i = 0; i < MAX_BULLET; ++i)
		if (m_myBulletList[i].getDamage() > 0)
			m_myBulletList[i].render();
			//m_myBulletList[i].render();
	for (int i = 0; i < MAX_BULLET * 3; ++i)
		if (m_OtherBulletList[i].getDamage()>0)
			m_OtherBulletList[i].render();
//	for (auto p = m_itemList.cbegin(); p != m_itemList.cend(); ++p) p->render();
	for (int i = 0; i < MAX_ITEM; ++i)
		if(m_itemList[i].getState()>0)
			m_itemList[i].render();
	for (auto p = m_tileList.cbegin(); p != m_tileList.cend(); ++p) p->render();


	/*remain ammo*/
	m_pTexture->render(0, 200, 600, 800, tex::etc, 4, 4, 2, 2);
	switch (m_playerList[m_myTeamNo].getAmmo())
	{
	case 6:
		m_pTexture->render(116, 166, 675, 725, tex::etc, 16, 16, 12, 8);
	case 5:
		m_pTexture->render(95, 145, 711, 761, tex::etc, 16, 16, 12, 8);
	case 4:
		m_pTexture->render(52, 102, 711, 761, tex::etc, 16, 16, 12, 8);
	case 3:
		m_pTexture->render(32, 82, 675, 725, tex::etc, 16, 16, 12, 8);
	case 2:
		m_pTexture->render(52, 102, 638, 688, tex::etc, 16, 16, 12, 8);
	case 1:
		m_pTexture->render(95, 145, 638, 688, tex::etc, 16, 16, 12, 8);
		break;
	case 0:
		m_pTexture->render(75, 125, 600, 800, tex::etc, 4, 16, 1, 9);
		break;
	default:
		break;
	}

	/*remain HP*/
	m_pTexture->render(700, 800, 0, 200, tex::etc, 8, 16, 4, 12);
	if (m_playerList[m_myTeamNo].getHp())
		m_pTexture->render(700, 800, 200, 400, tex::etc, 8, 16, 5 + (3 - m_playerList[m_myTeamNo].getHp()), 12);


}

void ObjectManager::updatePlayerInfo()
{
	
	for (int i = 0, k = 0; i < 4; ++i, ++k)
	{
		m_playerList[i].setHp(s2cpacket.iPlayer[i].m_hp);
		if (i == m_myTeamNo)
		{
			for (int j = 0; j < MAX_BULLET; ++j)
			{
				if(m_myBulletList[j].getDamage()!=0)
					m_myBulletList[j].setDamage(s2cpacket.iBullet[i][j].m_damage);
			}
			m_playerList[i].setState(s2cpacket.iPlayer[i].m_state);
			--k;
			continue;
		}
		m_playerList[i].setPos(s2cpacket.iPlayer[i].m_pos);
		for (int j = 0; j < MAX_BULLET; ++j)
		{
			m_OtherBulletList[k*MAX_BULLET + j].setTeam(5);	//???????
			m_OtherBulletList[k*MAX_BULLET + j].setPos(s2cpacket.iBullet[i][j].m_pos);
		}
	}
	for (int i = 0; i < MAX_ITEM; ++i)
	{
		m_itemList[i].setPos(s2cpacket.iItem[i].m_pos);
		m_itemList[i].setState(s2cpacket.iItem[i].m_type);
	}
	c2spacket.player.m_pos = m_playerList[m_myTeamNo].getPos();
	c2spacket.player.m_hp = m_playerList[m_myTeamNo].getHp();
	c2spacket.player.m_state = m_playerList[m_myTeamNo].getState();
	for (int i = 0; i < MAX_BULLET; ++i)
	{
		c2spacket.Bullets[i].m_pos = m_myBulletList[i].getPos();
		c2spacket.Bullets[i].m_damage = m_myBulletList[i].getDamage();
	}
	
}

void ObjectManager::updatePlayerInfoFirst()
{
	c2spacket.player.m_pos = m_playerList[m_myTeamNo].getPos();
	c2spacket.player.m_hp = m_playerList[m_myTeamNo].getHp();
	c2spacket.player.m_state = m_playerList[m_myTeamNo].getState();
	for (int i = 0; i < MAX_BULLET; ++i)
	{
		c2spacket.Bullets[i].m_pos = m_myBulletList[i].getPos();
		c2spacket.Bullets[i].m_damage = m_myBulletList[i].getDamage();
	}
}

