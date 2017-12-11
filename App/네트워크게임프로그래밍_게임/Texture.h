#pragma once

enum tex {
	
	
	title_bg=0,
	title_button,
	etc,

	MAX_TEXTURE
};


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
	void render(float bottom, float top, float left, float right, int texIdx);
	void render(float bottom, float top, float left, float right, int texIdx,
		int width, int height, int x, int y);

	GLubyte * LoadDIBitmap(const char * filename, BITMAPINFO **info);
};

