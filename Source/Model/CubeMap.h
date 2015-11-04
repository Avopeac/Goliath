#pragma once
#include <iostream>
#include <array>
#include <SOIL\soil.h>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#ifndef CUBEMAP_FACES
#define CUBEMAP_FACES 6
#endif
class CubeMap {
public:
	CubeMap() {}

	CubeMap(const char *right, const char *left, const char *top, const char *bottom, const char *front, const char * back) {
		load_cubemap(right, left, top, bottom, front, back);
	}

	void load_cubemap(const char *right, const char *left, const char *top, const char *bottom, const char *front, const char * back) {
		glGenTextures(1, &texid);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texid); 
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		unsigned char *data = nullptr;
		std::array <const char*, CUBEMAP_FACES> cubemap_images = { right, left, top, bottom, front, back };
		for (int i = 0; i < cubemap_images.size(); ++i) {
			data = SOIL_load_image(cubemap_images[i], &width, &height, &channels, SOIL_LOAD_RGB);
			if (data == 0) {
				std::cout << "Error loading cubemap image with name \"" << cubemap_images[i] << "\". Please check filename. " << std::endl;
			}
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	GLuint texid;
	int width, height, channels;
};