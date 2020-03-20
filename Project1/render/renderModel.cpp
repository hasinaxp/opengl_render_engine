#include  "renderModel.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../console.h"
#include "../deps/glad.h"

namespace sp {

	std::unordered_map<std::string, Texture*> RenderModelLoader::_texture_cache = {};

	glm::mat4 cnvt_mat4(const aiMatrix4x4& AssimpMatrix)
	{
		glm::mat4 m(1.0);
		m[0][0] = AssimpMatrix.a1;
		m[0][2] = AssimpMatrix.a2;
		m[0][2] = AssimpMatrix.a3;
		m[0][3] = AssimpMatrix.a4;
		m[1][0] = AssimpMatrix.b1;
		m[1][3] = AssimpMatrix.b2;
		m[1][2] = AssimpMatrix.b3;
		m[1][3] = AssimpMatrix.b4;
		m[2][0] = AssimpMatrix.c1;
		m[2][4] = AssimpMatrix.c2;
		m[2][2] = AssimpMatrix.c3;
		m[2][3] = AssimpMatrix.c4;
		m[3][0] = AssimpMatrix.d1;
		m[3][5] = AssimpMatrix.d2;
		m[3][2] = AssimpMatrix.d3;
		m[3][3] = AssimpMatrix.d4;
		return m;
	}

	RenderModelLoader::RenderModelLoader(RenderModel* model)
		: _model(model)
	{
	}

	void RenderModelLoader::load_file(std::string name, std::string filepath, bool is_animated)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(
			filepath,
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_JoinIdenticalVertices |
			aiProcess_GenNormals);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			Console::err("assimp scene couldnot been loaded!", importer.GetErrorString());
		}
		_directory = filepath.substr(0, filepath.find_last_of('/'));
		RenderModelEntity entity;
		entity.name = name;
		Transform trans;
		//trans.set_model_matrix(cnvt_mat4(scene->mRootNode->mTransformation)); //producing error
		entity.trans = _model->localTransforms.size();
		_model->localTransforms.push_back(trans);
		process_node(scene->mRootNode, scene, is_animated, entity, -1);
		_model->entities.push_back(entity);
	}


	Texture* RenderModelLoader::genTextureFlat(std::string filepath)
	{
		if (_texture_cache.find(filepath) == _texture_cache.end())
		{
			Texture* t = Texture::genTextureFlat(filepath.c_str());
			_texture_cache[filepath] = t;
			return t;
		}
		else
			return _texture_cache[filepath];
	}

	Texture* RenderModelLoader::genTextureCubemap(std::vector<std::string> filepaths)
	{
		std::vector<const char*> fps = {};
		for (auto it = filepaths.begin(); it != filepaths.end(); it++)
			fps.push_back((*it).c_str());
		return Texture::genTextureCubemap(fps);
	}

	void RenderModelLoader::process_node(void* nod, const void* sce, bool is_animated, RenderModelEntity& entity, int index)
	{
		aiNode* node = reinterpret_cast<aiNode*>(nod);
		aiScene* scene = reinterpret_cast<aiScene*>((void*)sce);

		for (uint i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			//do mesh loading
			if (!is_animated)
			{
				std::vector<Vertex_static> vertices = {};
				std::vector<uint> indices = {};
				std::vector<std::pair<Texture*, std::string>> textures = {};
				for (uint i = 0; i < mesh->mNumVertices; i++)
				{
					Vertex_static vertex;
					// process vertex positions, normals and texture coordinates
					glm::vec3 vector;
					vector.x = mesh->mVertices[i].x;
					vector.y = mesh->mVertices[i].y;
					vector.z = mesh->mVertices[i].z;
					vertex.position = vector;

					vector.x = mesh->mNormals[i].x;
					vector.y = mesh->mNormals[i].y;
					vector.z = mesh->mNormals[i].z;
					vertex.normal = vector;

					if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
					{
						glm::vec2 vec;
						vec.x = mesh->mTextureCoords[0][i].x;
						vec.y = mesh->mTextureCoords[0][i].y;
						vertex.uv = vec;
					}
					else
						vertex.uv = glm::vec2(0.0f, 0.0f);

					vertices.push_back(vertex);
				}

				// process indices
				for (uint i = 0; i < mesh->mNumFaces; i++)
				{
					aiFace face = mesh->mFaces[i];
					for (unsigned int j = 0; j < face.mNumIndices; j++)
						indices.push_back(face.mIndices[j]);
				}
				// process material
				if (mesh->mMaterialIndex >= 0)
				{
					aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

					std::vector<std::pair<aiTextureType, std::string>> texture_types = {
						{aiTextureType_DIFFUSE, "tex_diffuse"},
						{aiTextureType_AMBIENT, "tex_ambient"},
						{aiTextureType_SPECULAR, "tex_specular"},
						{aiTextureType_REFLECTION, "tex_reflection"},
						{aiTextureType_SHININESS, "tex_shininess"},
						{aiTextureType_NORMALS, "tex_normal"},
						{aiTextureType_HEIGHT, "tex_height"},
					};

					for (auto tt : texture_types)
					{
						for (uint i = 0; i < material->GetTextureCount(tt.first); i++)
						{
							aiString str;
							material->GetTexture(tt.first, i, &str);

							Texture* tex = genTextureFlat(std::string(_directory + '/' + str.C_Str()).c_str());
							textures.push_back(std::make_pair(tex, std::string(tt.second) + std::to_string(i)));
						}
					}
				}

				//add data to model
				RenderModelInfo model_map;

				VertexArray* vao = VertexArray::genVertexArray(
					&vertices[0],
					sizeof(vertices[0]) * vertices.size(),
					indices,
					cnst_vertex_static_layout,
					GL_STATIC_DRAW);
				_model->vaos.push_back(vao);
				model_map.vao = _model->vaos.size() - 1;
				for (auto t : textures)
				{
					_model->textures.push_back(t.first);
					model_map.textures.push_back(_model->textures.size() - 1);
					model_map.texture_names.push_back(t.second);
				}
				
				model_map.parent_index = index;

				index = entity._modelMaps.size();
				entity._modelMaps.push_back(model_map);
			}
			else //if the node is animated
			{
			}
		}
		// then do the same for each of its children
		for (uint i = 0; i < node->mNumChildren; i++)
		{
			process_node(node->mChildren[i], scene, is_animated, entity, index);
		}
	}


	uint RenderCommand::startingTextureSlot = 0;


	void RenderCommand::renderModelEntity(RenderModel* model, uint entity_index, ShaderProgram* sp, glm::mat4 world_transform, bool bind_shader)
	{
		if (bind_shader)
			sp->bind();
		RenderModelEntity& e = model->entities[entity_index];

		sp->uniform_m4(world_transform * model->localTransforms[e.trans].getModelMatrix(), cnst_txt_matrix_model);
		for (auto it = e._modelMaps.begin(); it != e._modelMaps.end(); it++)
		{
			RenderModelInfo& m = *it;
			for (int i = 0; i < m.textures.size(); i++)
			{
				model->textures[m.textures[i]]->bind(sp, startingTextureSlot + i, m.texture_names[i].c_str());
			}
			model->vaos[m.vao]->draw();
		}
	}

	void RenderCommand::renderModelEntityInstanced(RenderModel* model, uint entity_index, ShaderProgram* sp, std::vector<glm::mat4> world_transforms, bool bind_shader)
	{

		if (bind_shader)
			sp->bind();
		RenderModelEntity& e = model->entities[entity_index];
		for (auto it = e._modelMaps.begin(); it != e._modelMaps.end(); it++)
		{
			RenderModelInfo& m = *it;
			for (int i = 0; i < m.textures.size(); i++)
			{
				model->textures[m.textures[i]]->bind(sp, startingTextureSlot + i, m.texture_names[i].c_str());
			}
			VertexArray* vao = model->vaos[m.vao];
			if (!vao->IsInstanced())
			{
				vao->makeInstance(&world_transforms[0], sizeof(glm::mat4) * world_transforms.size(),
					{ {0, sizeof(glm::mat4), 4, 'm'} },
					world_transforms.size());
			}
			else
			{
				vao->setInstanceData(&world_transforms[0], sizeof(glm::mat4) * world_transforms.size(), world_transforms.size());
			}

			vao->draw(vao->IsInstanced());
		}
	}

	void RenderCommand::renderModel(RenderModel* model, ShaderProgram* sp, glm::mat4 world_transform, bool bind_shader)
	{
		if (bind_shader)
			sp->bind();

		for (uint i = 0; i < model->entities.size(); i++)
		{
			RenderCommand::renderModelEntity(model, i, sp, world_transform, false);
		}
	}

	void RenderCommand::renderModelInstanced(RenderModel* model, ShaderProgram* sp, std::vector<glm::mat4> world_transforms, bool bind_shader)
	{
		if (bind_shader)
			sp->bind();

		for (uint i = 0; i < model->entities.size(); i++)
		{
			RenderCommand::renderModelEntityInstanced(model, i, sp, world_transforms, false);
		}
	}

	void RenderCommand::renderVertexArray(VertexArray* vao)
	{
		if (vao != nullptr)
			vao->draw(vao->IsInstanced());
	}

	void RenderCommand::renderVertexArrayInstanced(VertexArray* vao, std::vector<glm::mat4> world_transforms)
	{
		if (!vao->IsInstanced())
		{
			vao->makeInstance(&world_transforms[0], sizeof(glm::mat4) * world_transforms.size(),
				{ {0, sizeof(glm::mat4), 4, 'm'} },
				world_transforms.size());
		}
		else
		{
			vao->setInstanceData(&world_transforms[0], sizeof(glm::mat4) * world_transforms.size(), world_transforms.size());
		}

		vao->draw(vao->IsInstanced());
	}

	void RenderCommand::uploadTextures(ShaderProgram* sp, std::vector<Texture*> textures, uint startingSlot, std::string namePrefix, bool bind_shader)
	{
		if (bind_shader)
			sp->bind();
		for (int i = 0; i < textures.size(); i++)
		{
			textures[i]->bind(sp, startingSlot + i,	std::string(namePrefix + std::to_string(i)).c_str());
		}
	}

	void RenderCommand::alphaBlend(bool should)
	{
		if (should) {
			glEnable(GL_BLEND);
		}
		else {
			glDisable(GL_BLEND);
		}
	}

	void RenderCommand::setClearColor(glm::vec3 color)
	{
		glClearColor(color.r, color.g, color.b, 1.0f);
	}



};