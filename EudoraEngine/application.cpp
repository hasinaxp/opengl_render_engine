#include "application.h"
#include "SDL.h"
#include "console.h"
#include <thread>



namespace sp {

	Display* Application::_display = nullptr;
	float Application::_accumulatedTime = 0.0f;
	float Application::_currentTime = 0.0f;
	float Application::_lastTime = 0.0f;
	int Application::_frameRate = 60;
	ApplicationLayer* Application::_currentLayer = nullptr;
	ApplicationLayer* Application::_overlayLayer = nullptr;
	std::vector<ApplicationLayer*> Application::_application_layers = {};
	
	void Application::create(int width, int height, const char* title)
	{

		//create display
		_display = new Display(width, height, title);
		
	}

	void Application::run()
	{
		_display->show(true);
		// initialize all application layers
		for (auto it = _application_layers.begin(); it != _application_layers.end(); it++)
		{
			(*it)->onInit();
		}
		if (_currentLayer != nullptr)
			_currentLayer->onLoad();
		
		std::thread t([]() {
			while (!EventSystem::shouldQuit())
			{
				Application::fastLoop();
			}
		});
		while (!EventSystem::shouldQuit())
		{
			EventSystem::resetControlWord();
			EventSystem::pollFromWindow(_display);
			EventSystem::resolve();
			if (_currentLayer != nullptr) {
				loop();
				renderLoop();
			}
			_display->onUpdate();
		}
		t.join();
		for (auto it = _application_layers.begin(); it != _application_layers.end(); it++)
		{
			(*it)->clean();
		}
	}

	void Application::close()
	{
		SDL_Event sdlevent;
		sdlevent.type = SDL_QUIT;
		SDL_PushEvent(&sdlevent);
	}

	void Application::destroy()
	{
		//clean all the layers
		for (auto it = _application_layers.begin(); it != _application_layers.end(); it++)
		{
			delete (*it);
		}
		if (_display)
			delete _display;
	}

	void Application::resizeWindow(int width, int height)
	{
		_display->resizeWindow(width, height);
	}

	void Application::lockMouse(bool lock)
	{
		if (lock)
		{
			SDL_WarpMouseInWindow(reinterpret_cast<SDL_Window*>(_display->getWindowHandle()), getWidth() / 2, getHeight() / 2);
			SDL_SetRelativeMouseMode(SDL_TRUE);
		}
		else
		{
			SDL_SetRelativeMouseMode(SDL_FALSE);
		}
	}

	void Application::pushLayer(ApplicationLayer* layer)
	{
		_application_layers.push_back(layer);
		if (_application_layers.size() > 0) {
			_currentLayer = _application_layers[0];
		}
	}

	void Application::pushOverlayLayer(std::string name)
	{
		for (auto it = _application_layers.begin(); it != _application_layers.end(); it++)
		{
			if ((*it)->getName() == name) {
				_overlayLayer = *it;
			}
		}
		_overlayLayer->onInit();
		_overlayLayer->onLoad();
	}

	void Application::popOverlayLayer()
	{
		_overlayLayer->onExit();
		_overlayLayer->clean();
		_overlayLayer = nullptr;
	}

	void Application::switchLayer(std::string name)
	{
		_currentLayer->onExit();
		for (auto it = _application_layers.begin(); it != _application_layers.end(); it++)
		{
			if ((*it)->getName() == name) {
				_currentLayer = *it;
			}
		}
		_currentLayer->onLoad();
	}

	void Application::fastLoop() {
		if(_currentLayer != nullptr)
		_currentLayer->fastUpdate(_accumulatedTime / 1000);
	}


	void Application::loop()
	{
		//time calculation
		_currentTime = SDL_GetTicks();
		_accumulatedTime = _currentTime - _lastTime;
		_lastTime = _currentTime;

		//update and render all application layers
		_currentLayer->appUpdate(_accumulatedTime/1000);
		if (_overlayLayer != nullptr) {
			_overlayLayer->appUpdate(_accumulatedTime / 1000);
			}

	}
	void Application::renderLoop()
	{
		//clear renderer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		_currentLayer->onRender();
		if (_overlayLayer != nullptr) {
			_overlayLayer->onRender();
		}
	}

}
