#include "stdafx.h"
#include "ObjectManager.h"


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

void ObjectManager::initialize()
{
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
			m_tileList.emplace_back(Vector2D(
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
			m_playerList.emplace_back(3, tp->getPos(), PLAYERSPD, PLAYERSIZE, i++);
		}
	}
}

void ObjectManager::addBullet(float x, float y, int team)
{
	for (auto p = m_playerList.begin(); p != m_playerList.end(); ++p)
	{
		if (p->getTeam() == team)
		{
			if (p->isCanFire())
			{
				m_bulletList.emplace_back(p->getPos(),
					Vector::normalize(Vector2D(Vector::sub(p->getPos(), Vector2D(x, y)))), PLAYER_0);
				p->fire();
			}
			break;
		}
	}
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
		for (auto bp = m_bulletList.begin(); bp != m_bulletList.end(); ++bp)
		{
			if (bp->getTeam() != team)
			{
				if (p->isCollide(*bp))
				{
					p->addHp(-1);
					bp->addHp(-1);
				}
			}
		}
		for (auto ip = m_itemList.begin(); ip != m_itemList.end(); ++ip)
		{
			if (p->isCollide(*ip))
			{
				ip->addHp(-1);
			}
		}
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
		if (p->isDead())
		{
			p = m_playerList.erase(p);
			if (p == m_playerList.end()) break;
			continue;
		}

		/*업데이트*/
		p->update(elapsedTime);
	}

	for (auto p = m_bulletList.begin(); p != m_bulletList.end(); ++p)
	{
		for (auto tp = m_tileList.cbegin(); tp != m_tileList.cend(); ++tp)
		{
			if (tp->getType() == tile::Wall)
			{
				if (p->isCollideRect(*tp))
				{
					p->addHp(-1);
				}
			}
		}

		if (p->isDead())
		{
			p = m_bulletList.erase(p);
			if (p == m_bulletList.end()) break;
			continue;
		}
		if (p->isOut())
		{
			p = m_bulletList.erase(p);
			if (p == m_bulletList.end()) break;
			continue;
		}

		p->update(elapsedTime);
	}

	for (auto p = m_itemList.begin(); p != m_itemList.end(); ++p)
	{
		if (p->isDead())
		{
			p = m_itemList.erase(p);
			if (p == m_itemList.end()) break;
			continue;
		}

		p->update(elapsedTime);
	}
}

void ObjectManager::render()
{
	for (auto p = m_playerList.cbegin(); p != m_playerList.cend(); ++p) p->render();
	for (auto p = m_bulletList.cbegin(); p != m_bulletList.cend(); ++p) p->render();
	for (auto p = m_itemList.cbegin(); p != m_itemList.cend(); ++p) p->render();
	for (auto p = m_tileList.cbegin(); p != m_tileList.cend(); ++p) p->render();

}

void ObjectManager::updatePlayerInfo(InfoPlayer * p)
{
	if (m_playerList.size() == 4)
	{
		for (int i = 0; i < 4; ++i)
		{
			/*if (i == playerNum)
				continue;*/
			m_playerList[i].setPos(p[i].m_pos);
			//printf("%f,%f\n", m_playerList[i].getPos().x, m_playerList[i].getPos().y);
		}
	}
	p[0].m_pos = m_playerList[0].getPos();
}
