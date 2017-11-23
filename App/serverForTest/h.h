#pragma once
#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#pragma warning(disable:4996)

#define MSGSIZE 1

struct Vector2D
{
	float x, y;
	Vector2D() { x = 0.0f; y = 0.0f; }
	Vector2D(float x, float y)
		:x(x), y(y)
	{}
	~Vector2D() {}
};