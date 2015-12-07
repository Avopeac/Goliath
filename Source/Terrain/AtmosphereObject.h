#pragma once
#include <iostream>
#include <GLM/gtx/transform.hpp>
#include "View/Drawable.h"
#include "View/ShaderStore.h"
class AtmosphereObject : public Drawable {
public:
	AtmosphereObject(double radius) : Drawable(), _radius(radius), _atmosphere_radius(radius * 1.025), _factor(1.0 / _atmosphere_radius) {
		set_shader(sky_from_space_shader);
		generate_mesh(512, 512, glm::scale(glm::dvec3(_atmosphere_radius)), glm::translate(glm::dvec3(0)));
	}

	void generate_mesh(unsigned int latitudes, unsigned int longitudes, const glm::dmat4 &scale, const glm::dmat4 &translation) {
		glm::dmat4 transform = translation * scale;

		//Vertices
		mesh.vertices.reserve((latitudes + 1) * (longitudes + 1));
		for (unsigned int lat = 0; lat <= latitudes; ++lat) {
			double theta = lat * glm::pi<double>() / latitudes;
			double sin_theta = glm::sin(theta);
			double cos_theta = glm::cos(theta);
			for (unsigned int lon = 0; lon <= longitudes; ++lon) {
				double phi = lon * glm::two_pi<double>() / longitudes;
				double sin_phi = glm::sin(phi);
				double cos_phi = glm::cos(phi);
				glm::dvec3 r = glm::dvec3(cos_phi * sin_theta, cos_theta, sin_phi * sin_theta);
				r = glm::dvec3(transform * glm::dvec4(r, 1));

				//Convert to floating-point precision
				glm::vec3 h_pos, l_pos;
				double_vec_to_float_vecs(r, h_pos, l_pos);
				glm::vec2 uv = glm::vec2(1.0 - (lon / longitudes), 1.0 - (lat / latitudes));
				glm::vec3 scaled_pos = r * _factor;
				mesh.vertices.push_back(Vertex(h_pos, scaled_pos, uv, l_pos, l_pos));
			}
		}
		//Indices
		mesh.indices.reserve(latitudes * longitudes * 6);
		for (unsigned int lat = 0; lat < latitudes; ++lat) {
			for (unsigned int lon = 0; lon < longitudes; ++lon) {
				unsigned int first = (lat * (longitudes + 1)) + lon;
				unsigned int second = first + longitudes + 1;
				mesh.indices.push_back(first + 1);
				mesh.indices.push_back(second);
				mesh.indices.push_back(first);
				mesh.indices.push_back(second);
				mesh.indices.push_back(first + 1);
				mesh.indices.push_back(second + 1);
			}
		}
		mesh.setup_mesh();
	}

	// Inherited via Drawable
	virtual void draw(const Camera & camera, double delta_time) override;
	virtual void draw_wireframe(const Camera & camera, double delta_time) override;

private:
	void begin_draw(const Camera &camera);
	void end_draw();
	bool _first_render = true;
	const double _radius;
	const double _atmosphere_radius;
	const double _factor;


	inline void double_to_floats(double double_value, float &high, float &low) {
		high = (float)double_value;
		low = (float)(double_value - high);
	}

	inline void double_vec_to_float_vecs(const glm::dvec3 &double_vec, glm::vec3 &high_vec, glm::vec3 &low_vec) {
		float hx, hy, hz, lx, ly, lz;
		double_to_floats(double_vec.x, hx, lx);
		double_to_floats(double_vec.y, hy, ly);
		double_to_floats(double_vec.z, hz, lz);
		high_vec = glm::vec3(hx, hy, hz);
		low_vec = glm::vec3(lx, ly, lz);
	}

	std::shared_ptr<Shader> sky_from_space_shader = std::shared_ptr<Shader>(ShaderStore::instance().get_shader_from_store(SKY_FROM_SPACE_SHADER_PATH));
	std::shared_ptr<Shader> sky_from_atmosphere_shader = std::shared_ptr<Shader>(ShaderStore::instance().get_shader_from_store(SKY_FROM_ATMOSPHERE_SHADER_PATH));

};






