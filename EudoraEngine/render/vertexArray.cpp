#include "vertexArray.h"

namespace sp {


	VertexArray::VertexArray()
		:_vao(0),
		_vbo(0),
		_vboi(0),
		_ebo(0),
		_instanceCount(0),
		_drawType(VertexDrawType::triangle),
		_layoutCount(0),
		_layoutInstanceMax(0),
		_indexCount(0)
	{

		glGenVertexArrays(1, &_vao);
		glGenBuffers(1, &_ebo);
		glGenBuffers(1, &_vbo);
		glBindVertexArray(_vao);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
		glBindVertexArray(0);
	}


	VertexArray::~VertexArray()
	{
		if (_vao != 0)
		{
			glBindVertexArray(_vao);
			if (_vbo != 0)
			{
				glDeleteBuffers(1, &_vbo);
				_vbo = 0;
			}
			if (_vboi != 0)
			{
				glDeleteBuffers(1, &_vboi);
				_vboi = 0;
			}
			if (_ebo != 0)
			{
				glDeleteBuffers(1, &_ebo);
				_ebo = 0;
			}
			glDeleteVertexArrays(1, &_vao);
		}
	}

	void VertexArray::setVertexBufferData(const void* data, uint size_in_bytes, uint update_mode)
	{
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, size_in_bytes, data, update_mode);
	}

	void VertexArray::setVertexBufferSubdata(const void* data, uint size_in_bytes, uint offset)
	{
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferSubData(GL_ARRAY_BUFFER, offset, size_in_bytes, data);
	}

	void VertexArray::setVertexBufferLayout(std::vector<VertexBufferLayout> layout)
	{
		_layout = layout;
		glBindVertexArray(_vao);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		uint i;
		for (i = 0; i < layout.size(); i++)
		{
			auto l = layout[i];
			enableAttribPointer(i, l);
		}
		_layoutCount = i;
	}

	void VertexArray::restoreLayout()
	{
		setVertexBufferLayout(_layout);
	}

	void VertexArray::setIndexBufferVector(std::vector<uint> indices)
	{
		glBindVertexArray(_vao);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);
		_indexCount = indices.size();
	}

	void VertexArray::setVertexDrawType(VertexDrawType mode)
	{
		_drawType = mode;
	}

	VertexArray* VertexArray::genVertexArray(const void* data, uint size_in_bytes, std::vector<uint> indices, std::vector<VertexBufferLayout> layout, uint update_mode)
	{
		VertexArray* varray = new VertexArray();
		varray->setVertexBufferLayout(layout);
		varray->setVertexBufferData(data, size_in_bytes, update_mode);
		varray->setIndexBufferVector(indices);
		return varray;
	}

	VertexArray* VertexArray::genVertexArray(RawVertexData* data)
	{
		if (data->size == 0 || data->indices.size() == 0 || data->data == nullptr)
			return nullptr;

		return genVertexArray(data->data, data->size, data->indices,data->layout);

	}


	VertexArray* VertexArray::genQuad(float scalex, float scaley)
	{
		scalex /= 2;
		scaley /= 2;
		std::vector<float> QuadVertices = GeometryReferance::QuadVertices;
		for (int i = 0; i < 4; i++) {
			QuadVertices[i * 8] *= scalex;
			QuadVertices[i * 8 + 3] *= scalex;
			QuadVertices[i * 8 + 1] *= scaley;
			QuadVertices[i * 8 + 4] *= scaley;
		}
		return genVertexArray(&QuadVertices[0],sizeof(float) *GeometryReferance::QuadVertices.size(), GeometryReferance::QuadIndices);
	}

	void VertexArray::bind()
	{
		glBindVertexArray(_vao);
	}

	void VertexArray::unbind()
	{
		glBindVertexArray(0);
	}

	void VertexArray::draw(bool multiple)
	{

		glBindVertexArray(_vao);
		if (_vboi == 0)
			glDrawElements(static_cast<GLenum>(_drawType), _indexCount, GL_UNSIGNED_INT, 0);
		else if (multiple)
			glDrawElementsInstanced(static_cast<GLenum>(_drawType), _indexCount, GL_UNSIGNED_INT, 0, _instanceCount);

	}


	void VertexArray::makeInstance(void* instance_data, uint size_in_bytes, std::vector<VertexBufferLayout> layout, int count)
	{
		if (!IsInstanced())
		{
			glBindVertexArray(_vao);
			glGenBuffers(1, &_vboi);
			glBindBuffer(GL_ARRAY_BUFFER, _vboi);
			glBufferData(GL_ARRAY_BUFFER, size_in_bytes, instance_data, GL_DYNAMIC_DRAW);

			int i;
			for (i = 0; i < layout.size(); i++)
			{
				auto l = layout[i];
				uint index = _layoutCount + i;
				i = enableAttribPointerInstance(index, l);
			}
			_layoutInstanceMax = i + _layoutCount;
			_instanceCount = count;
		}
	}

	void VertexArray::setInstanceData(void* instance_data, uint size_in_bytes, int count, bool expandable)
	{
		glBindBuffer(GL_ARRAY_BUFFER, _vboi);
		if (expandable)
			glBufferData(GL_ARRAY_BUFFER, size_in_bytes, instance_data, GL_DYNAMIC_DRAW);
		else
			glBufferSubData(GL_ARRAY_BUFFER, 0, size_in_bytes, instance_data);
		_instanceCount = count;
	}

	void VertexArray::releaseInstance()
	{
		if (_vboi != 0)
		{
			glBindVertexArray(_vao);
			glDeleteBuffers(1, &_vboi);
			for (int i = _layoutCount; i < _layoutInstanceMax; i++)
			{
				glDisableVertexAttribArray(i);
			}
			_vboi = 0;
		}
	}

	int VertexArray::enableAttribPointer(uint index, VertexBufferLayout l)
	{
		if (l.type == 'f')
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, l.count, GL_FLOAT, GL_FALSE, l.stride, (GLvoid*)l.offset);

		}
		else if (l.type == 'm')
		{

			glEnableVertexAttribArray(index);
			for (int i = 0; i < 4; i++)
			{
				glEnableVertexAttribArray(index + i);
				glVertexAttribPointer(i + index, 4, GL_FLOAT, GL_FALSE, l.stride, (void*)(l.offset + i * 4 * sizeof(float)));
			}
			index += 3;

		}

		return index;

	}

	int VertexArray::enableAttribPointerInstance(uint index, VertexBufferLayout l)
	{
		if (l.type == 'f')
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, l.count, GL_FLOAT, GL_FALSE, l.stride, (GLvoid*)l.offset);
			glVertexAttribDivisor(index, 1);
		}
		else if (l.type == 'm')
		{

			glEnableVertexAttribArray(index);
			for (int i = 0; i < 4; i++)
			{
				glEnableVertexAttribArray(index + i);
				glVertexAttribPointer(i + index, 4, GL_FLOAT, GL_FALSE, l.stride, (void*)(l.offset + i * 4 * sizeof(float)));
				glVertexAttribDivisor(index + i, 1);
			}
			index += 3;

		}

		return index;
	}


};