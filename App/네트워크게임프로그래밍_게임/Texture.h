#pragma once

#define MAX_TEXTURE 1


class Texture
{
	GLubyte *m_texBits[MAX_TEXTURE];
	BITMAPINFO *m_bitInfo[MAX_TEXTURE];
	GLuint m_textures[MAX_TEXTURE];

public:
	Texture();
	~Texture();

	void init();
	void render();

	GLubyte * LoadDIBitmap(const char * filename, BITMAPINFO **info);
};

