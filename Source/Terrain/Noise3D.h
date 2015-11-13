#pragma once
#include <iostream>
#include <GLM\glm.hpp>
#include <GLM\gtc\random.hpp>
#include <cmath>
#include <mutex>
#ifndef PERMUTATION_TABLE_SIZE
#define PERMUTATION_TABLE_SIZE 256
#endif 
#ifndef GRADIENT_TABLE_SIZE
#define GRADIENT_TABLE_SIZE 16
#endif
///This class has methods for returning a 3D noise value along with its directional derivatives.
///Credit goes to Ken Perlin, Inigo Quilez and Stefan Gustavsson
class Noise3D {
public:
	static void get_noise(const glm::vec3 &position, float &height, glm::vec3 &derivative) {
		//Maybe remove static casts and be happy with warnings? This code should run as fast as possible.
		int i = glm::floor(position.x);
		int j = glm::floor(position.y);
		int k = glm::floor(position.z);
		float u = position.x - i;
		float v = position.y - j;
		float w = position.z - k;

		const float du = 30.0f * u * u * (u * (u - 2.0f) + 1.0f);
		const float dv = 30.0f * v * v * (v * (v - 2.0f) + 1.0f);
		const float dw = 30.0f * w * w * (w * (w - 2.0f) + 1.0f);
		u = u * u * u * (u * (u * 6.0f - 15.0f) + 10.0f);
		v = v * v * v * (v * (v * 6.0f - 15.0f) + 10.0f);
		w = w * w * w * (w * (w * 6.0f - 15.0f) + 10.0f);

		const float a = glm::dot(get_lattice_gradient(i + 0, j + 0, k + 0), glm::vec3(u, v, w));
		const float b = glm::dot(get_lattice_gradient(i + 1, j + 0, k + 0), glm::vec3(u - 1.0f, v, w));
		const float c = glm::dot(get_lattice_gradient(i + 0, j + 1, k + 0), glm::vec3(u, v - 1.0f, w));
		const float d = glm::dot(get_lattice_gradient(i + 1, j + 1, k + 0), glm::vec3(u - 1.0f, v - 1.0f, w));
		const float e = glm::dot(get_lattice_gradient(i + 0, j + 0, k + 1), glm::vec3(u, v, w - 1.0f));
		const float f = glm::dot(get_lattice_gradient(i + 1, j + 0, k + 1), glm::vec3(u - 1, v, w - 1.0f));
		const float g = glm::dot(get_lattice_gradient(i + 0, j + 1, k + 1), glm::vec3(u, v - 1.0f, w - 1.0f));
		const float h = glm::dot(get_lattice_gradient(i + 1, j + 1, k + 1), glm::vec3(u - 1.0f, v - 1.0f, w - 1.0f));

		const float k0 = a;
		const float k1 = b - a;
		const float k2 = c - a;
		const float k3 = e - a;
		const float k4 = a - b - c + d;
		const float k5 = a - c - e + g;
		const float k6 = a - b - e + f;
		const float k7 = -a + b + c - d + e - f - g + h;

		//Set out values
		height = k0 + k1 * u + k2 * v + k3 * w + k4 * u * v + k5 * v * w + k6 * w * u + k7 * u * v * w;
		derivative.x = du * (k1 + k4 * v + k6 * w + k7 * v * w);
		derivative.y = dv * (k2 + k5 * w + k4 * u + k7 * w * u);
		derivative.z = dw * (k3 * k6 * u + k5 * v + k7 * u * v);
	}

protected:
	Noise3D() {};
	static void get_whole_and_frac(float x, int &i, float &f) { i = (int)x; f = x - (float)i; }
	static glm::vec3 get_lattice_gradient(int i, int j, int k) {
		static int p[PERMUTATION_TABLE_SIZE] = { 151,160,137,91,90,15,
			131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
			190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
			88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
			77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
			102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
			135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
			5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
			223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
			129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
			251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
			49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
			138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };


		static glm::vec3 gradient_table[GRADIENT_TABLE_SIZE] = {
			{1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
			{1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
			{0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1},
			{1,1,0},{-1,1,0},{0,-1,1},{0,-1,-1},
		};

		//Wrap around
		unsigned int n = p[i % PERMUTATION_TABLE_SIZE];
		n = p[(n + j) % PERMUTATION_TABLE_SIZE];
		n = p[(n + k) % PERMUTATION_TABLE_SIZE];
		return gradient_table[n % GRADIENT_TABLE_SIZE];
	};
};