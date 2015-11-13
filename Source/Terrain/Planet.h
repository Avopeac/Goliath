#pragma once
#include "..\View\Drawable.h"
#include "..\Terrain\QuadTree.h"
#include "..\Terrain\Skybox.h"
#include "..\View\ShaderStore.h"
#include "..\Model\CubeMap.h"

class Planet : public Drawable {
public:
	Planet(double radius) : Drawable() {

		float scale = (float)radius;
		float trans = (float)radius * 0.5f;
		//Set up planet "cube"
		_north = std::make_shared<QuadTree>(glm::mat4(1), glm::translate(glm::vec3(0, trans, 0)), scale);
		_south = std::make_shared<QuadTree>(glm::rotate(glm::pi<float>(), glm::vec3(0, 0, 1)), glm::translate(glm::vec3(0, -trans, 0)), scale);
		_west = std::make_shared<QuadTree>(glm::rotate(glm::half_pi<float>(), glm::vec3(0, 0, 1)), glm::translate(glm::vec3(-trans, 0, 0)), scale);
		_east = std::make_shared<QuadTree>(glm::rotate(glm::three_over_two_pi<float>(), glm::vec3(0, 0, 1)), glm::translate(glm::vec3(trans, 0, 0)), scale);
		_hither = std::make_shared<QuadTree>(glm::rotate(glm::half_pi<float>(), glm::vec3(1, 0, 0)), glm::translate(glm::vec3(0, 0, trans)), scale);
		_yon = std::make_shared<QuadTree>(glm::rotate(glm::three_over_two_pi<float>(), glm::vec3(1, 0, 0)), glm::translate(glm::vec3(0, 0, -trans)), scale);

		//Set up ground shaders
		_ground_shader = ShaderStore::instance().get_shader_from_store(GROUND_SHADER_PATH);
		_north->set_shader(_ground_shader);
		_south->set_shader(_ground_shader);
		_west->set_shader(_ground_shader);
		_east->set_shader(_ground_shader);
		_hither->set_shader(_ground_shader);
		_yon->set_shader(_ground_shader);

		//Set up skybox shader
		_skybox = std::make_shared<Skybox>();
		_skybox->set_shader(ShaderStore::instance().get_shader_from_store(SKYBOX_SHADER_PATH));

		//_noise1.load_cubemap("Images/noise.png", "Images/noise.png", "Images/noise.png", "Images/noise.png", "Images/noise.png", "Images/noise.png");
		//_normal_noise1.load_cubemap("Images/normal_map.png", "Images/normal_map.png", "Images/normal_map.png", "Images/normal_map.png", "Images/normal_map.png", "Images/normal_map.png");
	}

	// Inherited via Drawable
	virtual void draw(const Camera & camera, double delta_time) override;
	virtual void draw_wireframe(const Camera & camera, double delta_time) override;

private:
	std::shared_ptr<Skybox> _skybox;
	std::shared_ptr<QuadTree> _north;
	std::shared_ptr<QuadTree> _south;
	std::shared_ptr<QuadTree> _west;
	std::shared_ptr<QuadTree> _east;
	std::shared_ptr<QuadTree> _hither;
	std::shared_ptr<QuadTree> _yon;
	std::shared_ptr<Shader> _ground_shader;
	//CubeMap _noise1;
	//CubeMap _normal_noise1;
};
