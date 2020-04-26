#pragma once
#include "../api.h"
#include "../deps/glm/glm.hpp"
#include <vector>
#include <cstdlib>

namespace sp {

	
	struct Vertex_static
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;
	};

	struct  Vertex_static_skinned
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;
		glm::ivec4 bone_ids;
		glm::vec4 bone_weights;
	};

	struct  VertexBufferLayout
	{
		uint offset = 0;
		uint stride = 3 * sizeof(float);
		uint count = 3;
		char type = 'f';
	};

	const std::vector<VertexBufferLayout> cnst_vertex_static_layout = {
		{ 0, sizeof(Vertex_static), 3, 'f' },
		{ offsetof(Vertex_static,normal), sizeof(Vertex_static), 3, 'f' },
		{ offsetof(Vertex_static,uv), sizeof(Vertex_static),  2, 'f' }
	};

	struct RawVertexData {
		bool deleteInnerData = true;
		void* data = nullptr;
		uint size = 0;
		std::vector<uint> indices;
		std::vector< VertexBufferLayout> layout = cnst_vertex_static_layout;
		~RawVertexData() {
		}
	};

	namespace GeometryReferance {


		const std::vector<float> QuadVertices =
		{
			-1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
			1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
			1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
		};

		const std::vector<uint> QuadIndices = { 0, 1, 2, 0, 2, 3 };

		const std::vector<float> CubemapVertices =
		{
			// positions          // normals
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
			0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
			-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
			-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
			0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
			-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
		};

		const std::vector<uint> CubemapIndices = {
			 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
			12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
			24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35

		};

	};

};