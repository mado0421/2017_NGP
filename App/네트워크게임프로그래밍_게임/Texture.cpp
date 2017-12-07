#include "stdafx.h"
#include "Texture.h"


Texture::Texture()
{
}


Texture::~Texture()
{
}

void Texture::init()
{
	char name[64];

	glGenTextures(6, m_textures);
	for (int i = 0; i < MAX_TEXTURE; i++)
	{
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);


		sprintf(name, "assets/img/bmp%d.bmp", i);
		m_texBits[i] = LoadDIBitmap(name, &m_bitInfo[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, m_texBits[i]);

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
	glBindTexture(GL_TEXTURE_2D, textures[0]);

	// top
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex3fv(Point[0].arr);
	glTexCoord2f(0.0f, 0.0f); glVertex3fv(Point[1].arr);
	glTexCoord2f(1.0f, 0.0f); glVertex3fv(Point[2].arr);
	glTexCoord2f(1.0f, 1.0f); glVertex3fv(Point[3].arr);
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
