#include <limits>
#include <memory>
#include <GLM\gtc\matrix_transform.hpp>
#include "..\Math\MathHelp.h"
#include "Tile.h"

void Tile::generate(int resolution, const glm::dmat4 &rotation, const glm::highp_dvec3 &translation, const glm::dmat4 &scale, bool normalize, bool gen_normals) {
	generate(resolution, nullptr, rotation, translation, scale, normalize, gen_normals);
}

void Tile::generate(int resolution, std::shared_ptr<HeightFunction> function, const glm::dmat4 &rotation, const glm::highp_dvec3 &translation,
	const glm::dmat4 &scale, bool normalize, bool gen_normals) {
	//Loop counters
	int j, i;
	//Step size
	double delta = 1.0 / resolution; 
	//The offset from origin
	double bias = 0.5; 
	//Highest valued height values
	double minHeight = std::numeric_limits<double>().max();
	double maxHeight = std::numeric_limits<double>().min();
	//Generate vertex buffer for tile mesh, n + 1 vertices, gives n tiles
	for (j = 0; j <= resolution; ++j) {
		for (i = 0; i <= resolution; ++i) {
			//Next step
			double di = delta * i;
			double dj = delta * j;
			//Create a high precision vector
			glm::highp_dvec4 temp(di - bias, 0, dj - bias, 0.0);
			//Mesh normalization enables us to "push" out the mesh into a direction
			if (normalize) {
				//Scale is applied afterwards if normalized
				temp = rotation * temp;
				temp = temp + scale * glm::normalize(glm::highp_dvec4(temp.x, temp.y, temp.z, 0.0));
			}
			else {
				//Scale is applied first
				temp = rotation * scale * temp;
			}
			//Translation is done lastly
			temp.x += translation.x;
			temp.y += translation.y;
			temp.z += translation.z;
			//Apply height function
			if (function != nullptr) {
				//Offset y-coordinate with the height value
				temp.y += function->get_value(glm::highp_dvec3(temp.x, temp.y, temp.z));
				//Find max and min height value
				if (temp.y > maxHeight) {
					maxHeight = temp.y;
				}
				if (temp.y < minHeight) {
					minHeight = temp.y;
				}
			}
			//Convert vertex to floating point precision, using 2 floats
			float xp, xl, yp, yl, zp, zl;
			Vertex v;
			MathHelp::double_to_two_floats(temp.x, xp, xl);
			MathHelp::double_to_two_floats(temp.y, yp, yl);
			MathHelp::double_to_two_floats(temp.z, zp, zl);
			v._position_high = glm::highp_vec3(xp, yp, zp);
			v._position_low = glm::highp_vec3(xl, yl, zl);
			v._texcoord = { di, dj };
			v._normal = { 0, 0, 0 };
			//Add to vertex list
			_mesh.vertices.push_back(v);
		}
		//Set the found max and min height values
		_min_height = minHeight;
		_max_height = maxHeight;
	}
	//Generate index buffer for tile mesh
	unsigned int step = resolution + 1;
	for (j = 0; j < resolution; ++j) {
		for (i = 0; i < resolution; ++i) {
			unsigned int idx0 = i + (j + 1) * step;
			unsigned int idx1 = i + 1 + j * step;
			_mesh.indices.push_back(i + j * step);
			_mesh.indices.push_back(idx0);
			_mesh.indices.push_back(idx1);
			_mesh.indices.push_back(idx1);
			_mesh.indices.push_back(idx0);
			_mesh.indices.push_back(i + 1 + (j + 1) * step);
		}
	}
	//Generate normals
	if (gen_normals) {
		generate_normals();
	}
}

void Tile::generate_normals() {
	//Find normals by averaging close by vertices and using the cross product
	int count = _mesh.indices.size();
	for (int j = 0; j < count; j += 3) {
		//Get indices for nearby points
		const int ia(_mesh.indices[j + 0]);
		const int ib(_mesh.indices[j + 1]);
		const int ic(_mesh.indices[j + 2]);
		//Find vertices
		Vertex &v1(_mesh.vertices[ia]);
		Vertex &v2(_mesh.vertices[ib]);
		Vertex &v3(_mesh.vertices[ic]);
		glm::dvec3 v1c = v1._position_high + v1._position_low;
		glm::dvec3 v2c = v2._position_high + v2._position_low;
		glm::dvec3 v3c = v3._position_high + v3._position_low;
		//Compute normal
		glm::dvec3 normal(glm::cross(v1c - v2c,
			v3c - v1c));
		//Add normal to each vertex, this will be normalized in the shader, no point in doing it here
		_mesh.vertices[ia]._normal += normal;
		_mesh.vertices[ib]._normal += normal;
		_mesh.vertices[ic]._normal += normal;
	}
}