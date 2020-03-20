#pragma once
#include "../api.h"
#include "../control/transform.h"
#include "vertexArray.h"
#include "texture.h"
#include "shaderProgram.h"
#include <vector>
#include <list>
#include <string>

namespace sp {

	struct SP_API  RenderModelInfo
	{
		int parent_index = -1; // -1 root
		uint vao = 0;
		std::vector<uint> textures = {};
		std::vector<std::string> texture_names = {};
	};

	struct SP_API  RenderModelEntity
	{
		std::string name = "render_model_entity";
		std::vector<RenderModelInfo> _modelMaps;
		uint trans;
	};

	struct SP_API RenderModel
	{
		std::vector<VertexArray*> vaos = {};
		std::vector<Texture*> textures = {};
		std::vector<Transform> localTransforms = {};
		std::vector<RenderModelEntity> entities = {};
	};

	// class responsible for loading a model file
	class SP_API RenderModelLoader
	{
	private:
		RenderModel* _model;
		std::string _directory;
		static std::unordered_map<std::string, Texture*> _texture_cache;

	public:
		RenderModelLoader(RenderModel* model = nullptr);
		void load_file(std::string name, std::string filepath, bool is_animated = false);
		void setRenderModelReferance(RenderModel* model) { _model = model; };


		static Texture* genTextureFlat(std::string filepath);
		static Texture* genTextureCubemap(std::vector<std::string> filepaths);

	private:
		void process_node(void* node, const void* scene, bool is_animated, RenderModelEntity& entity, int index);
	};


	//class to draw models
	class SP_API RenderCommand
	{
	public:
		static uint startingTextureSlot;
		RenderCommand() {};
		static void renderModelEntity(RenderModel* model, uint entity_index, ShaderProgram* sp, glm::mat4 world_transform = glm::mat4(1.0f), bool bind_shader = true);
		static void renderModelEntityInstanced(RenderModel* model, uint entity_index, ShaderProgram* sp, std::vector<glm::mat4> world_transforms, bool bind_shader = true);
		static void renderModel(RenderModel* model, ShaderProgram* sp, glm::mat4 world_transform = glm::mat4(1.0f), bool bind_shader = true);
		static void renderModelInstanced(RenderModel* model, ShaderProgram* sp, std::vector<glm::mat4> world_transforms, bool bind_shader = true);
		static void renderVertexArray(VertexArray* vao);
		static void renderVertexArrayInstanced(VertexArray* vao, std::vector<glm::mat4> world_transforms);
		static void uploadTextures(ShaderProgram* sp, std::vector<Texture*> textures,uint startingSlot, std::string namePrefix = "texr", bool bind_shader = true);
		static void alphaBlend(bool should = true);
		static void setClearColor(glm::vec3 color);
	};


};