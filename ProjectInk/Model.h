#pragma once
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Material.h"
#include "OBJLoader.h"

class Model
{
private:
	Material* material;
	Texture* overrideTextureDiffuse;
	Texture* overrideTextureSpecular;
	std::vector<Mesh*> meshes;
	glm::vec3 position;

	void updateUniforms()
	{

	}

public:
	Model(glm::vec3 position,
		Material* material,
		Texture* orTexDif,
		Texture* orTexSpec,
		std::vector<Mesh*> meshes
	)
	{
		this->position = position;
		this->material = material;
		this->overrideTextureDiffuse = orTexDif;
		this->overrideTextureSpecular = orTexSpec;

		for (auto* i : meshes)
		{
			this->meshes.push_back(new Mesh(*i));
		}

		for (auto& i : this->meshes)
		{
			i->move(this->position);
			i->setOrigin(this->position);
		}
	}

	//OBJ file loaded model
	Model(glm::vec3 position,
		Material* material,
		Texture* orTexDif,
		Texture* orTexSpec,
		const char* objFile
	)
	{
		this->position = position;
		this->material = material;
		this->overrideTextureDiffuse = orTexDif;
		this->overrideTextureSpecular = orTexSpec;

		std::vector<Vertex> mesh = loadOBJ(objFile);
		this->meshes.push_back(new Mesh(mesh.data(), mesh.size(), NULL, 0, glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(0),
			glm::vec3(0),
			glm::vec3(1)));

		for (auto& i : this->meshes)
		{
			i->move(this->position);
			i->setOrigin(this->position);
		}
	}

	~Model()
	{
		for (auto*& i : this->meshes)
		{
			delete i;
		}
	}

	//Accessors

	//Functions
	void rotate(const glm::vec3 rotation)
	{
		for (auto& i : this->meshes)
		{
			i->rotate(rotation);
		}
	}

	void update()
	{
		
	}

	void render(Shader* shader)
	{
		//update the uniforms
		this->updateUniforms();

		//Update Uniforms
		this->material->sendToShader(*shader);

		//Use a program
		shader->use();

		//draw
		for(auto& i : this->meshes)
		{
			//Activate texture
			this->overrideTextureDiffuse->bind(0);
			this->overrideTextureSpecular->bind(1);

			//activate shader
			i->render(shader);
		}
	}

};