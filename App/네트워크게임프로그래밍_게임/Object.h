#pragma once
#include "Texture.h"

#define DEFAULTHP 1
#define BULLETSPD 320.0f
#define BULLETSIZE 2.5f
#define PLAYERSPD 125.0f
#define PLAYERSIZE 7.5f
#define MAX_AMMO 6

#define BULLETIMGSIZE 5.0f
#define WALLSIDESIZE 16.0f
#define WALLTOPSIZE 8.0f

enum tile
{
	Wall = 0,
	PSpawn,
	ISpawn
};

class Object
{
protected:
	int			m_hp;
	Vector2D	m_pos;
	Vector2D	m_dir;
	float		m_spd;
	float		m_size;
	Color		m_color;

	float		m_standardTime;
	float		m_interval;
	bool		m_dead;

	Texture		*m_pTexture;
public:
	Object();
	Object(Texture *pTexture, int hp, Vector2D pos, float spd, float size)
		: m_pTexture(pTexture)
		, m_hp(hp)
		, m_pos(pos)
		, m_spd(spd)
		, m_size(size)
	{
		m_standardTime = 0.0f;
		m_dead = false;
	}
	~Object();

	virtual void update(float elapsedTime);
	virtual void render() const;

	bool isOut();
	bool isDead();
	bool isCollide(const Object &other);
	bool isCollideRect(const Object &other);


	virtual void move(float val);

	void setHp(int val)				{ m_hp = val; }
	void setPos(Vector2D pos)		{ m_pos = pos; }
	void setSpd(float val)			{ m_spd = val; }
	void setColor(Color color)		{ m_color = color; }
	void setStandardTime(float val) { m_standardTime = val; }
	void setSize(float f)			{ m_size = f; }
	void setTexture(Texture* pTexture) { m_pTexture = pTexture; }

	void addHp(int val)				{ m_hp += val; }
	void addPos(Vector2D pos)		{ m_pos.x += pos.x; m_pos.y += pos.y; }
	void addSpd(float val)			{ m_spd += val; }


	Vector2D getPos() const			{ return m_pos; }
	int getHp()	const				{ return m_hp; }
	float getSpd() const			{ return m_spd; }
	float getSize()	const			{ return m_size; }
	Color getColor() const			{ return m_color; }
	float getStandardTime()			{ return m_standardTime; }
	float getInterval()				{ return m_interval; }
};

class Player : public Object
{
private:
	int m_team;
	int m_ammo;
	int m_state;
	int m_dirX;
	int m_dirY;
public:
	Player();
	Player(Texture *pTexture, int hp, Vector2D pos, float spd, float size, int team, int state=1)
		: Object(pTexture, hp, pos, spd, size)
		, m_team(team)
	{
		if (team == PLAYER_0) setColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
		else if (team == PLAYER_1) setColor(Color(0.0f, 1.0f, 0.0f, 1.0f));
		else if (team == PLAYER_2) setColor(Color(0.0f, 0.0f, 1.0f, 1.0f));
		else if (team == PLAYER_3) setColor(Color(1.0f, 1.0f, 0.0f, 1.0f));

		m_ammo = MAX_AMMO;
		m_dirX = 0;
		m_dirY = 0;
	}
	~Player();

	virtual void render() const;

	void changeMoveDirection(unsigned char dir, float val);
	int getMoveDirection(unsigned char dir);

	virtual void update(float elapsedTime);
	virtual void move(float val);
	void changeDirByCollide(const Object &other);

	void fire();
	void reload();
	void dash();

	int getTeam() const { return m_team; }
	int getAmmo() const { return m_ammo; }
	void setHp(int h) { m_hp = h; }
	int getState() const { return m_state; }
	void setState(int s) { m_state = s; }
	bool isCanFire() const { return m_ammo; }
};

class Bullet : public Object
{
private:
	int			m_team;
	int			m_damage;
public:
	Bullet();
	Bullet(Texture *pTexture, Vector2D pos, Vector2D dir, int team, int damage)
		: Object(pTexture, DEFAULTHP, pos, BULLETSPD, BULLETSIZE)
		, m_team(team), m_damage(damage)
	{
		m_dir = dir;
		if (team == PLAYER_0) setColor(Color(1.0f, 1.0f, 0.0f, 1.0f));
		else setColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
	}
	Bullet(Texture *pTexture, int hp, Vector2D pos, float spd, float size, Vector2D dir, int team, int damage)
		: Object(pTexture, hp, pos, spd, size)
		, m_team(team), m_damage(damage)
	{
		m_dir = dir;
		if (team == PLAYER_0) setColor(Color(1.0f, 1.0f, 0.0f, 1.0f));
		else setColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
	}
	~Bullet();

	virtual void render() const;

	virtual void move(float val);

	void setDirection(Vector2D dir) { m_dir = dir; }

	int getTeam() const { return m_team; }
	void setTeam(int val) { m_team = val; }
	Vector2D getDirection() const { return m_dir; }
	int getDamage() const { return m_damage; }
	void setDamage(int val) { m_damage = val; }
};

class Item : public Object
{
private:
public:
	Item();
	Item(Texture *pTexture, int hp, Vector2D pos, float spd, float size)
		: Object(pTexture, hp, pos, spd, size)
	{
		setColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	}
	~Item();

	virtual void render() const;

};

class Tile : public Object
{
private:
	int type;
	int texIdx;
public:
	Tile();
	Tile(Texture *pTexture, Vector2D pos, int type, int texIdx)
		: Object(pTexture, 0.0f, pos, 0.0f, TILESIZE)
		, type(type)
		, texIdx(texIdx)
	{
		setColor(Color(0.3f, 0.3f, 0.5f, 1.0f));
	}
	~Tile();

	virtual void render() const;

	int getType()const { return type; }

};

