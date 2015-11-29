#pragma once
#include <string>
#include <iostream>
#include <SOIL\SOIL.h>
#include <GLFW\glfw3.h>
#include <GL\glew.h>
class Texture2DLoader {
public:
	Texture2DLoader() {}
	~Texture2DLoader() {}

	static GLuint load(const std::string &path) {
		GLuint loaded_id;
		glGenTextures(1, &loaded_id);
		glBindTexture(GL_TEXTURE_2D, loaded_id);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		int w, h, c;
		unsigned char *data = SOIL_load_image(path.c_str(), &w, &h, &c, SOIL_LOAD_RGB);
		if (data != 0) {
			std::cout << "Loaded texture at path " << path << " successfully. " << std::endl;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			SOIL_free_image_data(data);
		}
		else {
			std::cerr << "Failed to load texture at path " << path << "." << std::endl;
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		return loaded_id;
	}
};