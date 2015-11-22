#include "Terrain\Noise3D.h"
#include <GLM\gtc\random.hpp>
#include <iostream>
#include <SOIL\SOIL.h>

void Noise3D::initialize(unsigned int seed) {
	_seed = seed;
	initialize_permutations();
	initialize_gradients();
}

void Noise3D::initialize_permutations() {
	unsigned int i;
	std::srand(time(NULL));
	std::vector<int> temp_permutations;
	//Fill 1D array of permutation values of 255
	for (i = 0; i < PERMUTATION_NUMBER; ++i) { temp_permutations.push_back(-1); }
	for (i = 0; i < PERMUTATION_NUMBER; ++i) {
		for (;;) {
			int perm = glm::linearRand(0, PERMUTATION_NUMBER) % PERMUTATION_NUMBER;
			if (temp_permutations.at(perm) == -1) {
				temp_permutations.at(perm) = i;
				break;
			}
		}
	}

	for (i = 0; i < PERMUTATION_NUMBER; ++i) {
		unsigned char convert_perm = static_cast<unsigned char>(temp_permutations.at(i));
		_permutations.push_back(convert_perm);
		_permutation_bytes.push_back(convert_perm);
		_permutation_bytes.push_back(convert_perm);
		_permutation_bytes.push_back(convert_perm);
		_permutation_bytes.push_back(convert_perm);
	}
}

void Noise3D::initialize_gradients() {
	_gradients = {
		255, 255, 128,
		0, 255, 128,
		255, 0, 128,
		0, 0, 128, 
		255,128,255,
		0, 128, 255, 
		255, 128, 0,
		0, 128, 0, 
		128, 255, 255,
		128, 0, 255,
		128, 255, 0,
		128, 0, 0,
		255, 255, 128,
		128, 0, 255,
		0, 255, 128,
		128, 0, 0
	};
	
	unsigned int i;
	for (i = 0; i < GRADIENT_NUMBER; ++i) {
		unsigned char x = _gradients[i + 0];
		unsigned char y = _gradients[i + 1];
		unsigned char z = _gradients[i + 2];
		_gradient_bytes.push_back(x);
		_gradient_bytes.push_back(y);
		_gradient_bytes.push_back(z);
		_gradient_bytes.push_back(255);
	}
}

void Noise3D::generate_permutation_texture() {
	glGenTextures(1, &_p_tex_id);
	glBindTexture(GL_TEXTURE_1D, _p_tex_id);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, PERMUTATION_NUMBER, 0, GL_RGBA, GL_UNSIGNED_BYTE, &_permutation_bytes[0]);
}

void Noise3D::generate_gradient_texture() {
	glGenTextures(1, &_g_tex_id);
	glBindTexture(GL_TEXTURE_1D, _g_tex_id);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, GRADIENT_NUMBER, 0, GL_RGBA, GL_UNSIGNED_BYTE, &_gradient_bytes[0]);
}

void Noise3D::save_textures_to_disk(std::string permutation_filename, std::string gradient_filename) {
	SOIL_save_image(permutation_filename.append(".tga").c_str(), SOIL_SAVE_TYPE_TGA, PERMUTATION_NUMBER, 1, 4, &_permutation_bytes[0]);
	SOIL_save_image(gradient_filename.append(".tga").c_str(), SOIL_SAVE_TYPE_TGA, GRADIENT_NUMBER, 1, 4, &_gradient_bytes[0]);
}