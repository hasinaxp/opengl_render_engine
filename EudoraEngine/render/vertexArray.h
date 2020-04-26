#pragma once
#include  "../api.h"
#include "vertex.h"
#include "../deps/glad.h"
#include <functional>

namespace sp {

	//type of drawing methods
	enum class VertexDrawType
	{
		triangle = GL_TRIANGLES,
		point = GL_POINTS,
		line = GL_LINES,
		triangle_strip = GL_TRIANGLE_STRIP,
		quad = GL_QUADS
	};

	//base class representing a geometry i.e mesh in gpu
	//use instance functions to draw batches at sametime
	class SP_API VertexArray
	{
	private:
		uint _vao;
		uint _vbo;
		uint _vboi;
		uint _ebo;
		VertexDrawType _drawType;
		uint _indexCount;
		std::vector<VertexBufferLayout> _layout;
		uint _instanceCount;
		uint _layoutCount;
		uint _layoutInstanceMax;
	public:
		VertexArray();
		virtual ~VertexArray();

		uint getVertexArrayId() const { return _vao; }
		uint getVertexBufferId() const { return _vbo; }
		uint getIndexBufferId() const { return _ebo; }
		VertexDrawType getVertexDrawType() const { return _drawType; }
		uint getIndexCount() const { return _indexCount; }
		std::vector<VertexBufferLayout> getLayout() const { return _layout; }
		bool IsInstanced() { return (_vboi != 0 ? true : false); }
		uint getLayoutCount() const { return _layoutCount; }
		uint getInstanceCount() const { return _instanceCount; }

		void setVertexBufferData(const void* data, uint size_in_bytes, uint update_mode = GL_STATIC_DRAW);
		void setVertexBufferSubdata(const void* data, uint count, uint offset = 0);
		void setVertexBufferLayout(std::vector<VertexBufferLayout> layout);
		void restoreLayout();
		void setIndexBufferVector(std::vector<uint> indices);
		void setVertexDrawType(VertexDrawType mode);

		static VertexArray* genVertexArray(const void* data, uint size_in_bytes, std::vector<uint> indices, std::vector<VertexBufferLayout> layout = cnst_vertex_static_layout,uint update_mode = GL_STATIC_DRAW);
		static VertexArray* genVertexArray(RawVertexData* data);
		static VertexArray* genQuad(float scalex = 1.0f, float scaley = 1.0f);

		void bind();
		void unbind();

		virtual void draw(bool multiple = true);

		void makeInstance(void* instance_data, uint size_in_bytes, std::vector<VertexBufferLayout> layout, int count);
		void setInstanceData(void* instance_data, uint size_in_bytes, int count, bool expandable = true);
		void releaseInstance();

	private:
		int enableAttribPointer(uint index, VertexBufferLayout l);
		int enableAttribPointerInstance(uint index, VertexBufferLayout l);
	};



}