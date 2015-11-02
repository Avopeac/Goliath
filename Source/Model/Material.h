#pragma once
#include "GLM\glm.hpp"
struct Material {
	Material(const glm::vec3 albedo = { 0.2,0.01,0.2 }, float roughness = 0.5f, float absorption = 0.5f, float gaussian = 0.5f, float refraction = 4.0f) 
		: albedo(albedo), roughness(roughness), absorption(absorption), gaussian(gaussian), refraction(refraction) {};
	glm::vec3 albedo; //Color of the surface, keep to a maximum channel value of around 0.2
	float roughness; //How rough the surface is, light will be distributed more diffusely if high
	float absorption; //How much light does the surface absorb, a low absorption gives a more specular highlights
	float gaussian; //How deep are the gaussian distributed microfacets of the surface, gives a rough look if high
	float refraction; //The refraction index for a dielectric material, gives more fresnel if high
};
