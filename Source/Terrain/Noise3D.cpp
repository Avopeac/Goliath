#include "Terrain\Noise3D.h"
#include <cmath>
//#include <SOIL\SOIL.h>

void Noise3D::initialize() {
	generate_gradient_texture();
	generate_permutation_texture();
}

void Noise3D::generate_permutation_texture() {
	glGenTextures(1, &_p_tex_id);
	glBindTexture(GL_TEXTURE_1D, _p_tex_id);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_R8, PERMUTATION_NUMBER, 0, GL_RED, GL_UNSIGNED_BYTE, &_permutations[0]);
}

void Noise3D::generate_gradient_texture() {
	glGenTextures(1, &_g_tex_id);
	glBindTexture(GL_TEXTURE_1D, _g_tex_id);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, GRADIENT_NUMBER, 0, GL_RGB, GL_UNSIGNED_BYTE, &_gradients[0]);
}

//void Noise3D::save_textures_to_disk(std::string permutation_filename, std::string gradient_filename) {
//	SOIL_save_image(permutation_filename.append(".tga").c_str(), SOIL_SAVE_TYPE_TGA, PERMUTATION_NUMBER, 1, 4, &_permutation_bytes[0]);
//	SOIL_save_image(gradient_filename.append(".tga").c_str(), SOIL_SAVE_TYPE_TGA, GRADIENT_NUMBER, 1, 4, &_gradient_bytes[0]);
//}


const unsigned int Noise3D::_permutations[PERMUTATION_NUMBER] = { 151, 160, 137, 91, 90, 15,
131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180, 
//Repeat
151, 160, 137, 91, 90, 15,
131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180 };

const int Noise3D::_gradients[GRADIENT_NUMBER][RGB_BYTES]{
	{  1,  1,  0 },
	{ -1,  1,  0 },
	{  1, -1,  0 },
	{ -1, -1,  0 },
	{  1,  0,  1 }, 
	{ -1,  0,  1 },
	{  1,  0, -1 },
	{ -1,  0, -1 },
	{  0,  1,  1 },
	{  0, -1,  1 },
	{  0,  1, -1 },
	{  0, -1, -1 },
	{  1,  1,  0 },
	{  0, -1,  1 },
	{ -1,  1,  0 },
	{  0, -1, -1 },
};

const unsigned int Noise3D::_gradients_rgb[RGB_BYTES * GRADIENT_NUMBER] = {
	0xFF, 0xFF, 0x80, //  1  1  0
	0x00, 0xFF, 0x80, // -1  1  0 
	0xFF, 0x00, 0x80, //  1 -1  0
	0x00, 0x00, 0x80, // -1 -1  0
	0xFF, 0x80, 0xFF, // etc.
	0x00, 0x80, 0xFF, 
	0xFF, 0x80, 0x00,
	0x00, 0x80, 0x00,
	0x80, 0xFF, 0xFF,
	0x80, 0x00, 0xFF,
	0x80, 0xFF, 0x00,
	0x80, 0x00, 0x00,
	0xFF, 0xFF, 0x80,
	0x80, 0x00, 0xFF,
	0x00, 0xFF, 0x80,
	0x80, 0x00, 0x00,
};
