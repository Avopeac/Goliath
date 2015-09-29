#include "Model.h"
#include <SOIL\SOIL.h>
#include <iostream>

GLint TextureFromFile(const char* path, std::string directory);

void Model::Draw(Shader &shader) {
	for (Mesh &mesh : meshes) {
		mesh.Draw(shader);
	}
}

void Model::DrawWireframe(Shader &shader) {
	for (Mesh &mesh : meshes) {
		mesh.DrawWireframe(shader);
	}
}

void Model::LoadModel(const std::string &path) {
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_GenSmoothNormals);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::FAILED_TO_LOAD_MODEL::" << importer.GetErrorString() << std::endl;
		return;
	}

	this->directory = path.substr(0, path.find_last_of("\\"));
	this->ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode *node, const aiScene *scene) {
	for (GLuint i = 0; i < node->mNumMeshes; ++i) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->ProcessMesh(mesh, scene));
	}

	for (GLuint i = 0; i < node->mNumChildren; ++i) {
		this->ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene){
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	for (GLuint i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;
		glm::vec3 v;
		v.x = mesh->mVertices[i].x;
		v.y = mesh->mVertices[i].y;
		v.z = mesh->mVertices[i].z;
		vertex.positionHigh = v;

		glm::vec3 n;
		n.x = mesh->mNormals[i].x;
		n.y = mesh->mNormals[i].y;
		n.z = mesh->mNormals[i].z;
		vertex.normal = n;

		if (mesh->mTextureCoords[0]) {
			glm::vec2 t;
			t.x = mesh->mTextureCoords[0][i].x;
			t.y = mesh->mTextureCoords[0][i].y;
			vertex.texcoord = t;
		} else {
			vertex.texcoord = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; ++j) {
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = this->LoadMaterialTextures(material, aiTextureType_DIFFUSE, Texture::DIFFUSE_IDENTIFIER);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> specularMaps = this->LoadMaterialTextures(material, aiTextureType_SPECULAR, Texture::SPECULAR_IDENTIFIER);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	std::cout << "Loaded mesh with "
		<< vertices.size() << " vertices, "
		<< indices.size() << " indices and "
		<< textures.size() << " textures. " << std::endl;

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName) {
	std::vector<Texture> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); ++i) {
		aiString str;
		mat->GetTexture(type, i, &str);
		GLboolean skip = false;

		for (GLuint j = 0; j < this->texturesLoaded.size(); ++j) {
			if (this->texturesLoaded[j].path == str) {
				textures.push_back(texturesLoaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip) {
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);
			this->texturesLoaded.push_back(texture);
		}
	}

	return textures;
}

GLint TextureFromFile(const char* path, std::string directory) {
	//Generate texture ID and load texture data 
	std::string filename = std::string(path);
	filename = directory + "\\" + filename;

	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;

	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	if (image == nullptr) {
		std::cout << "ERROR::SOIL::FAILED_TO_LOAD_IMAGE" << std::endl;
	}

	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);

	return textureID;
}

