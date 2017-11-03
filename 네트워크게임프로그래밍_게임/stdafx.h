// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include "Dependencies\glew.h"
#include "Dependencies\freeglut.h"

#include <vector>

#define PLAYER_0 0
#define PLAYER_1 1
#define PLAYER_2 2
#define PLAYER_3 3

#define WWIDTH 640
#define WHEIGHT 480

struct Color
{
	float r, g, b, a;
	Color()
		: r(0.0f)
		, g(0.0f)
		, b(0.0f)
		, a(0.0f)
	{}
	Color(float r, float g, float b, float a)
		: r(r)
		, g(g)
		, b(b)
		, a(a)
	{}
	~Color() {}
};

struct Vector2D
{
	float x, y;
	Vector2D() { x = 0.0f; y = 0.0f; }
	Vector2D(float x, float y)
		:x(x), y(y)
	{}
	~Vector2D() {}
};

namespace Vector
{
	inline float length(Vector2D val) { return sqrt(val.x*val.x + val.y*val.y); }
	inline Vector2D normalize(Vector2D val) {float len = Vector::length(val);return Vector2D(val.x / len, val.y / len);}
	inline Vector2D add(Vector2D a, Vector2D b) { return Vector2D(b.x + a.x, b.y + a.y); }
	inline Vector2D sub(Vector2D from, Vector2D to) { return Vector2D(to.x - from.x, to.y - from.y); }
}


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
