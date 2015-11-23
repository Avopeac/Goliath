#pragma once
#include <random>
#include <vector>
#include <string>
#include <GLM\glm.hpp>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#ifndef PERMUTATION_NUMBER 
#define PERMUTATION_NUMBER 256
#endif
#ifndef GRADIENT_NUMBER
#define GRADIENT_NUMBER 16
#endif

class Noise3D {
public:
	Noise3D() {}
	~Noise3D() {}
	void initialize(unsigned int seed);
	GLuint get_gradient_texture_id() const { return _g_tex_id; }
	GLuint get_permutation_texture_id() const { return _p_tex_id; }
	void generate_gradient_texture();
	void generate_permutation_texture();
	const std::vector<unsigned char> &get_permutations() const { return _permutation_bytes; }
	const std::vector<unsigned char> &get_gradients() const { return _gradients; }
	void save_textures_to_disk(std::string permutation_filename, std::string gradient_filename);

private:
	void initialize_permutations();
	void initialize_gradients();
	std::vector<unsigned char> _permutations;
	std::vector<unsigned char> _permutation_bytes;
	std::vector<unsigned char> _permutation_bytes_2d;
	std::vector<unsigned char> _gradients;
	std::vector<unsigned char> _gradient_bytes;
	std::vector<unsigned char> _gradient_perm_bytes;
	unsigned int _seed;
	GLuint _p_tex_id;
	GLuint _p2d_tex_id;
	GLuint _g_tex_id;
	GLuint _gp_tex_id;
};