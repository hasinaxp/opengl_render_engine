#include "display.h"
#include <SDL.h>
#include <iostream>
#include "deps/glad.h"
#include "console.h"

namespace sp {


	//opengl debug callback
	void GLAPIENTRY error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* useParam)
	{
		sp::Console::gl_debug(type, message);
	}



	Display::Display(int width, int height, const char* title)
		:_width(width),
		_height(height)
	{
		//initialize sdl
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0) // on success returns 0
		{
			Console::err("cannot innitialize sdl", SDL_GetError());
		}
		SDL_GL_LoadLibrary(NULL);
		_window = SDL_CreateWindow(title,	//title
				SDL_WINDOWPOS_UNDEFINED,	//x cord
				SDL_WINDOWPOS_UNDEFINED,	//ycord
				_width,						//width
				_height,					//height
				SDL_WINDOW_OPENGL);			//window type flag

		if (!_window) {
			Console::err("window construction failed", "Display.create");
		}
		else {
			//create opengl context
			_context = SDL_GL_CreateContext(reinterpret_cast<SDL_Window*>(_window));
			//set opengl hints
			SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
			SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

			SDL_GetWindowSize(reinterpret_cast<SDL_Window*>(_window), &_width, &_height);

			//initialize glad
			if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
				Console::err("failed to initialize glad", "application.cpp");
			}

			glEnable(GL_DEBUG_OUTPUT);
			glDebugMessageCallback(error_callback, 0);


			glViewport(0, 0, getWidth(), getHeight());
			glEnable(GL_DEPTH_TEST);
			SDL_GL_SetSwapInterval(1);

			setClearColor(0.16f, 0.713f, 0.964f, 1);
		}

	}

	Display::~Display()
	{
		if (_window) {
			//cleanup sdl
			SDL_GL_DeleteContext(reinterpret_cast<SDL_GLContext>(_context));
			SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(_window));
			SDL_Quit();
			_window = nullptr;
			_context = nullptr;

		}
	}

	void Display::show(bool should)
	{
		if (_window) {
			SDL_Window* win = reinterpret_cast<SDL_Window*>(_window);
			if (should)
				SDL_ShowWindow(win);
			else
				SDL_HideWindow(win);
		}
		else {
			std::cout << "err: Display is not initialized. window returns nullptr\n";
		}
	}

	void Display::onUpdate()
	{
		SDL_GL_SwapWindow(reinterpret_cast<SDL_Window*>(_window));
	
	}

	void Display::resizeWindow(int width, int height)
	{
		SDL_SetWindowSize(reinterpret_cast<SDL_Window*>(_window), width, height);
		SDL_GetWindowSize(reinterpret_cast<SDL_Window*>(_window), &_width, &_height);
	}

	void Display::setClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}


};

