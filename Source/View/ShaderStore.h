#pragma once
#include <map>
#include <memory>
#include <string>
#include "..\Model\Shader.h"
#include "ShaderStore.h"

#ifndef SKYBOX_SHADER_PATH
#define SKYBOX_SHADER_PATH "Shaders/skyboxshader"
#endif SKYBOX_SHADER_PATH

#ifndef ADDITIVE_SHADER_PATH
#define ADDITIVE_SHADER_PATH "Shaders/additiveshader"
#endif ADDITIVE_SHADER_PATH

#ifndef ATMOSPHERE_SHADER_PATH
#define ATMOSPHERE_SHADER_PATH "Shaders/atmosphereshader"
#endif ATMOSPHERE_SHADER_PATH

#ifndef BLUR_SHADER_PATH
#define BLUR_SHADER_PATH "Shaders/blurfiltershader"
#endif BLUR_SHADER_PATH

#ifndef GAMMA_TONEMAP_SHADER_PATH
#define GAMMA_TONEMAP_SHADER_PATH "Shaders/gammatonemapshader"
#endif GAMMA_TONEMAP_SHADER_PATH

#ifndef THRESHOLD_SHADER_PATH
#define THRESHOLD_SHADER_PATH "Shaders/thresholdshader"
#endif THRESHOLD_SHADER_PATH

#ifndef PLAIN_TEXTURE_SHADER_PATH
#define PLAIN_TEXTURE_SHADER_PATH "Shaders/plaintextureshader"
#endif PLAIN_TEXTURE_SHADER_PATH

#ifndef STANDARD_SHADER_PATH
#define STANDARD_SHADER_PATH "Shaders/standard"
#endif STANDARD_SHADER_PATH

#ifndef GROUND_SHADER_PATH
#define GROUND_SHADER_PATH "Shaders/groundshader"
#endif GROUND_SHADER_PATH

#ifndef WATER_SHADER_PATH 
#define WATER_SHADER_PATH "Shaders/water"
#endif WATER_SHADER_PATH 

class ShaderStore {
public:
	static ShaderStore &instance() {
		static ShaderStore store;
		return store;
	}
	
	void stock_inventory() {
		add_shader_to_store(SKYBOX_SHADER_PATH);
		add_shader_to_store(ADDITIVE_SHADER_PATH);
		add_shader_to_store(ATMOSPHERE_SHADER_PATH);
		add_shader_to_store(BLUR_SHADER_PATH);
		add_shader_to_store(GAMMA_TONEMAP_SHADER_PATH);
		add_shader_to_store(THRESHOLD_SHADER_PATH);
		add_shader_to_store(PLAIN_TEXTURE_SHADER_PATH);
		add_shader_to_store(GROUND_SHADER_PATH);
		add_shader_to_store(STANDARD_SHADER_PATH);
		add_shader_to_store(WATER_SHADER_PATH);
	}

	void add_shader_to_store(const std::string &item) {
		std::string vert_name = item;
		vert_name.append(".vert");
		std::string frag_name = item;
		frag_name.append(".frag");
		std::pair<std::string, std::shared_ptr<Shader>> pair;
		pair = std::make_pair(item, std::make_shared<Shader>(vert_name.c_str(), frag_name.c_str()));
		_store.insert(pair);
	}

	std::shared_ptr<Shader> get_shader_from_store(const std::string &item) {
		auto it = _store.find(item);
		if (it != _store.end()) {
			return it->second;
		}
		return nullptr;
	}

private:
	ShaderStore() {}
	std::map<std::string, std::shared_ptr<Shader>> _store;
};