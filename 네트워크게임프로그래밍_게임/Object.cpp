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
	move(elapsedTime);
}

void Object::render() const
{
	glPushMatrix();
	glColor3f(m_color.r, m_color.g, m_color.b);
	glTranslatef(m_pos.x, m_pos.y, 0.0f);

	glutSolidCube(m_size * 2.0);

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

		for (auto tp = m_tileList.cbegin(); tp != m_tileList.cend(); ++tp)
		{
			if (tp->getType() == tile::Wall)
			{
				if (p->isCollide(*tp))
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
	for (auto p = m_tileList.cbegin(); p != m_tileList.cend(); ++p) p->render();

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
	Vector2D dir = Vector::normalize(Vector2D(m_dirX, m_dirY));
	float spd = getSpd();

	pos.x += val * spd * dir.x;
	pos.y += val * spd * dir.y;
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

Tile::Tile()
{
}

Tile::~Tile()
{
}

void Tile::render() const
{
	if (type == tile::Wall)
	{
		Color color = getColor();
		Vector2D pos = getPos();
		float size = getSize();
		glPushMatrix();
		glColor3f(color.r, color.g, color.b);
		glTranslatef(pos.x, pos.y, 0.0f);

		glutSolidCube(size * 2.0);

		glPopMatrix();
	}
}
