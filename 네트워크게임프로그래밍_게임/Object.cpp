#include "stdafx.h"
#include "Object.h"


Object::Object()
{
}

Object::~Object()
{
}

void Object::update(float elapsedTime)
{
	/*elapsedTime을 받아야 하는데...*/
	move(elapsedTime);
}

void Object::render() const
{
	glPushMatrix();
	glColor3f(m_color.r, m_color.g, m_color.b);
	glTranslatef(m_pos.x, m_pos.y, 0.0f);

	glutSolidCube(m_size);

	glPopMatrix();
}

bool Object::isOut()
{
	return(m_pos.x - m_size < 0 || m_pos.x + m_size > WWIDTH ||
		m_pos.y - m_size < 0 || m_pos.y + m_size > WHEIGHT);

}

bool Object::isDead()
{
	return (m_hp <= 0);
}

bool Object::isCollide(const Object & other)
{
	return ((m_size + other.m_size)*(m_size + other.m_size) >=
		(m_pos.x - other.m_pos.x)*(m_pos.x - other.m_pos.x) +
		(m_pos.y - other.m_pos.y)*(m_pos.y - other.m_pos.y));
}

void Object::move(float val)
{
}

//////////////////////////////////////////////////////////////////////////

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
	//std::ifstream mapFile("assets/map/map_1.txt");

	//if (mapFile.is_open())
	//{
	//	char buf[4];
	//	bool isSolid;
	//	short texIdx;
	//	short x;
	//	short y;
	//	while (!mapFile.eof())
	//	{
	//		

	//	}
	//}
	//else
	//{
	//	std::cout << "파일을 불러오는데 실패하였습니다." << std::endl;
	//}

	//mapFile.close();

	m_playerList.emplace_back(3, Vector2D(WWIDTH / 2.0, WHEIGHT / 2.0), PLAYERSPD,
		10.0f, PLAYER_0);

	m_playerList.emplace_back(3, Vector2D(WWIDTH / 4.0, WHEIGHT / 4.0), PLAYERSPD,
		10.0f, PLAYER_1);
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
		/*업데이트*/
		p->update(elapsedTime);

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

		/*체력검사*/
		if (p->isDead())
		{
			p = m_playerList.erase(p);
			if (p == m_playerList.end()) break;
			continue;
		}
	}
	for (auto p = m_bulletList.begin(); p != m_bulletList.end(); ++p)
	{
		p->update(elapsedTime);

		//for (auto wp = m_wallList.cbegin(); wp != m_wallList.cend(); ++wp)
		//{
		//	if (p->isCollide(*wp))
		//	{
		//		p->addHp(-1);
		//	}
		//}

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
	}
	for (auto p = m_itemList.begin(); p != m_itemList.end(); ++p)
	{
		p->update(elapsedTime);

		if (p->isDead())
		{
			p = m_itemList.erase(p);
			if (p == m_itemList.end()) break;
			continue;
		}
	}
}

void ObjectManager::render()
{
	for (auto p = m_playerList.cbegin(); p != m_playerList.cend(); ++p) p->render();
	for (auto p = m_bulletList.cbegin(); p != m_bulletList.cend(); ++p) p->render();
	for (auto p = m_itemList.cbegin(); p != m_itemList.cend(); ++p) p->render();
	for (auto p = m_wallList.cbegin(); p != m_wallList.cend(); ++p) p->render();

}

//////////////////////////////////////////////////////////////////////////

Player::Player()
{
}

Player::~Player()
{
}

void Player::changeMoveDirection(unsigned char dir, float val)
{
	switch (dir)
	{
	case 'x':
		m_dirX = val;
		break;
	case 'y':
		m_dirY = val;
		break;
	default:
		break;
	}
}

int Player::getMoveDirection(unsigned char dir)
{
	switch (dir)
	{
	case 'x':
		return m_dirX;
	case 'y':
		return m_dirY;
	default:
		break;
	}
}

void Player::update(float elapsedTime)
{
	move(elapsedTime);
}

void Player::move(float val)
{
	Vector2D pos = getPos();
	float spd = getSpd();

	pos.x += val * spd * m_dirX;
	pos.y += val * spd * m_dirY;
	setPos(pos);
}

void Player::fire()
{
	m_ammo--;
}

void Player::reload()
{
	m_ammo = MAX_AMMO;
}

void Player::dash()
{
}

//////////////////////////////////////////////////////////////////////////

Bullet::Bullet()
{
}

Bullet::~Bullet()
{
}

void Bullet::move(float val)
{
	Vector2D pos = getPos();
	float spd = getSpd();

	pos.x += val * spd * m_direction.x;
	pos.y += val * spd * m_direction.y;
	setPos(pos);
}

//////////////////////////////////////////////////////////////////////////

Item::Item()
{
}

Item::~Item()
{
}

Wall::Wall()
{
}

Wall::~Wall()
{
}
