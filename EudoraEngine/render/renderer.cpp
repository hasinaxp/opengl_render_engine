#include "renderer.h"
#include "../console.h"
#include "../deps/glad.h"

namespace sp {

	FrameBuffer::FrameBuffer(uint width, uint height, uint resolution, bool retain_texture)
	{
		_resolution = resolution;
		_retain_texture = retain_texture;
		glGenFramebuffers(1, &_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

		_texture = new Texture(width * _resolution, height * resolution);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture->getTextureId(), 0);

		glGenRenderbuffers(1, &_rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width * _resolution, height * _resolution);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Console::err("frame buffer is not complete", "frame_buffer constructor failed");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	FrameBuffer::~FrameBuffer()
	{
		glDeleteFramebuffers(1, &_fbo);
		glDeleteRenderbuffers(1, &_rbo);
		if (!_retain_texture)
			delete _texture;
	}

	void FrameBuffer::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	}

	void FrameBuffer::bindScreen()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	void FrameBuffer::setDimension(int width, int height, uint resolution)
	{
		if (!_retain_texture)
			delete _texture;

		_resolution = resolution;

		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
		_texture = new Texture(width * _resolution, height * _resolution);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture->getTextureId(), 0);
		glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width * _resolution, height * _resolution);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Console::err("frame buffer is not complete", "frame_buffer constructor failed");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	RenderInterface::RenderInterface(std::string name, uint width, uint height, uint resolution)
		: _name(name)
	{
		_frame_buffer = new FrameBuffer(width, height, resolution);
		_previous_pass_renderer = nullptr;
	}

	RenderInterface::~RenderInterface()
	{
		delete _frame_buffer;
	}

	void RenderInterface::render(bool screen)
	{
		if (screen)
		{
			_frame_buffer->bindScreen();
			onRender();
		}
		else
		{
			_frame_buffer->bind();
			onRender();
			_frame_buffer->bindScreen();
		}
	}

	void RenderInterface::resize(uint width, uint height, uint resolution)
	{
		_frame_buffer->setDimension(width, height, resolution);
	}

	void RenderInterface::renderPipe(bool last)
	{
		if (_previous_pass_renderer != nullptr)
		{
			RenderInterface* ppr = _previous_pass_renderer->getPreviousPassRenderer();
			if (ppr != nullptr)
			{
				_previous_pass_renderer->renderPipe(false);
			}
			else
			{
				_previous_pass_renderer->render(false);
				pushTexture(
					cnst_prefix_texture + _previous_pass_renderer->getName(),
					_previous_pass_renderer->getFrameBuffer()->getTexture());
			}
		}
		if (last)
			render(true);
		else
			render(false);
	}

	void RenderInterface::pushTexture(std::string name, Texture* tex)
	{
		_texture_map[name] = tex;
	}
	Texture* RenderInterface::getTexture(std::string name)
	{
		if (_texture_map.find(name) == _texture_map.end())
		{
			return nullptr;
		}
		else
		{
			return _texture_map[name];
		}
	}

};