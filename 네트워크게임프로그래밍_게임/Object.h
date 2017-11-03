#pragma once

#define DEFAULTHP 1
#define BULLETSPD 2.0f
#define BULLETSIZE 2.5f
#define PLAYERSPD 0.45f
#define PLAYERSIZE 7.5f
#define MAX_AMMO 6

enum tile
{
	Wall = 0,
	PSpawn,
	ISpawn
};

class Object
{
private:
	int			m_hp;
	Vector2D	m_pos;
	float		m_spd;
	float		m_size;
	Color		m_color;

	float		m_standardTime;
	float		m_interval;
	bool		m_dead;
public:
	Object();
	Object(int hp, Vector2D pos, float spd, float size)
		: m_hp(hp)
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


	virtual void move(float val);

	void setHp(int val)				{ m_hp = val; }
	void setPos(Vector2D pos)		{ m_pos = pos; }
	void setSpd(float val)			{ m_spd = val; }
	void setColor(Color color)		{ m_color = color; }
	void setStandardTime(float val) { m_standardTime = val; }

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

	int m_dirX;
	int m_dirY;
public:
	Player();
	Player(int hp, Vector2D pos, float spd, float size, int team)
		: Object(hp, pos, spd, size)
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

	void changeMoveDirection(unsigned char dir, float val);
	int getMoveDirection(unsigned char dir);

	virtual void update(float elapsedTime);
	virtual void move(float val);

	void fire();
	void reload();
	void dash();

	int getTeam() const { return m_team; }
	int getAmmo() const { return m_ammo; }

	bool isCanFire() const { return m_ammo; }
};

class Bullet : public Object
{
private:
	int			m_team;
	Vector2D	m_direction;
public:
	Bullet();
	Bullet(Vector2D pos, Vector2D dir, int team)
		: Object(DEFAULTHP, pos, BULLETSPD, BULLETSIZE)
		, m_team(team)
		, m_direction(dir)
	{
		if (team == PLAYER_0) setColor(Color(1.0f, 1.0f, 0.0f, 1.0f));
		else setColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
	}
	Bullet(int hp, Vector2D pos, float spd, float size, Vector2D dir, int team)
		: Object(hp, pos, spd, size)
		, m_team(team)
		, m_direction(dir)
	{
		if (team == PLAYER_0) setColor(Color(1.0f, 1.0f, 0.0f, 1.0f));
		else setColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
	}
	~Bullet();

	virtual void move(float val);

	void setDirection(Vector2D dir) { m_direction = dir; }

	int getTeam() const { return m_team; }
	Vector2D getDirection() const { return m_direction; }
};

class Item : public Object
{
private:
public:
	Item();
	Item(int hp, Vector2D pos, float spd, float size)
		: Object(hp, pos, spd, size)
	{
		setColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	}
	~Item();
};

class Tile : public Object
{
private:
	int type;
	int texIdx;
public:
	Tile();
	Tile(Vector2D pos, int type, int texIdx)
		: Object(0.0f, pos, 0.0f, TILESIZE)
		, type(type)
		, texIdx(texIdx)
	{
		setColor(Color(0.3f, 0.3f, 0.5f, 1.0f));
	}
	~Tile();

	virtual void render() const;

	int getType()const { return type; }

};

class ObjectManager
{
private:
	std::vector<Player> m_playerList;
	std::vector<Bullet> m_bulletList;
	std::vector<Item>	m_itemList;

	std::vector<Tile>	m_tileList;
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

};
