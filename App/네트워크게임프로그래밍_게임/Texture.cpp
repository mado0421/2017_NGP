#include "stdafx.h"
#include "Texture.h"


Texture::Texture()
{
}


Texture::~Texture()
{
}

#define TEST
void Texture::init()
{
	char name[64];

	glGenTextures(MAX_TEXTURE, m_textures);
	for (int i = 0; i < MAX_TEXTURE; i++)
	{
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);

#ifdef TEST
		sprintf(name, "assets/testImg/bmp%d.bmp", i);
		m_texBits[i] = LoadDIBitmap(name, &m_bitInfo[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1024, 1024, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, m_texBits[i]);
#else
		sprintf(name, "assets/img/bmp%d.bmp", i);
		m_texBits[i] = LoadDIBitmap(name, &m_bitInfo[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, m_texBits[i]);
#endif


		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
	glEnable(GL_TEXTURE_2D);

}

void Texture::render()
{
	float left		= (WWIDTH / 4.0);
	float right		= (3 * WWIDTH / 4.0);
	float bottom	= (WHEIGHT / 4.0);
	float top		= (3 * WHEIGHT / 4.0);

	glBindTexture(GL_TEXTURE_2D, m_textures[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(left, bottom);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(left, top);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(right, top);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(right, bottom);
	glEnd();
}

void Texture::render(float bottom, float top, float left, float right, int texIdx)
{
	glBindTexture(GL_TEXTURE_2D, m_textures[texIdx]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(left, bottom);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(left, top);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(right, top);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(right, bottom);
	glEnd();
}

void Texture::render(float bottom, float top, float left, float right, int texIdx, int wid, int hei, int x, int y)
{
	glBindTexture(GL_TEXTURE_2D, m_textures[texIdx]);
	glBegin(GL_QUADS);
	glTexCoord2f((1.0 / wid)*x,			1.0-(1.0 / hei)*((y + 1)));			glVertex2f(left, bottom);
	glTexCoord2f((1.0 / wid)*x,			1.0-(1.0 / hei)*(y));					glVertex2f(left, top);
	glTexCoord2f((1.0 / wid)*(x + 1),	1.0-(1.0 / hei)*(y));				glVertex2f(right, top);
	glTexCoord2f((1.0 / wid)*(x + 1),	1.0-(1.0 / hei)*((y + 1)));			glVertex2f(right, bottom);
	glEnd();
}

GLubyte * Texture::LoadDIBitmap(const char * filename, BITMAPINFO ** info)
{
	FILE				* fp;
	GLubyte				* bits;
	int					bitsize;
	int					infosize;
	BITMAPFILEHEADER	header;

	if ((fp = fopen(filename, "rb")) == nullptr) return nullptr;

	if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1)
	{
		fclose(fp);
		return nullptr;
	}

	if (header.bfType != 'MB')
	{
		fclose(fp);
		return nullptr;
	}

	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);

	if ((*info = (BITMAPINFO*)malloc(infosize)) == nullptr)
	{
		fclose(fp);
		exit(0);
		return nullptr;
	}

	if (fread(*info, 1, infosize, fp) < (unsigned int)infosize)
	{
		free(*info);
		fclose(fp);
		return nullptr;
	}

	if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
		bitsize = ((*info)->bmiHeader.biWidth *
		(*info)->bmiHeader.biBitCount + 7) / 8.0 *
		abs((*info)->bmiHeader.biHeight);

	if ((bits = (unsigned char *)malloc(bitsize)) == nullptr)
	{
		free(*info);
		fclose(fp);
		return nullptr;
	}

	if (fread(bits, 1, bitsize, fp) < (unsigned int)bitsize)
	{
		free(*info);
		free(bits);
		fclose(fp);
		return nullptr;
	}

	fclose(fp);
	return bits;
}
