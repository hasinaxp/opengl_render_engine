#include "../api.h"
#include "texture.h"
#include <map>


namespace sp {


	// framebuffer is like a offscreen canvas for drawing
	// bind to draw offscreen
	// call get texture to get results
	class SP_API FrameBuffer
	{
	private:
		uint _resolution;
		uint _fbo;
		uint _rbo;
		Texture* _texture;
		bool _retain_texture;
	public:
		FrameBuffer(uint width , uint height , uint resolution = 1, bool retain_texture = false); // if retain texture is set to true the texture will not delete even thought the framebuffer is deleted
		~FrameBuffer();

		void bind();
		void bindScreen();

		uint getFrameBufferId() const { return _fbo; }
		Texture* getTexture() const { return _texture; }
		bool get_is_retaining_texture() const { return _retain_texture; }
		uint get_resolution() const { return _resolution; }

		void setDimension(int width, int height, uint resolution = 1);
	};

	//parent class of all renderers.
	//renderers can be attached together by setting previous pass for multistage pipeline
	class SP_API RenderInterface
	{
	private:
		FrameBuffer* _frame_buffer;
		RenderInterface* _previous_pass_renderer = nullptr;
		std::map<std::string, Texture*> _texture_map;
		std::string _name = "base_renderer";

	public:
		RenderInterface(std::string name, uint width, uint height, uint resolution = 1);
		virtual ~RenderInterface();
		virtual void onInit() {};
		virtual void onRender() = 0;
		virtual void onDestroy() = 0;

		void render(bool screen = true);
		void resize(uint width, uint height, uint resolution = 4);
		void renderPipe(bool last = true);
		void pushTexture(std::string name, Texture* textute);
		Texture* getTexture(std::string name);

		FrameBuffer* getFrameBuffer() const { return _frame_buffer; }
		RenderInterface* getPreviousPassRenderer() const { return _previous_pass_renderer; }
		std::string getName() const { return _name; }

		void setPreviousPassRenderer(RenderInterface* renderer) { _previous_pass_renderer = renderer; };
		void setName(std::string name) { _name = name; }
	};

}