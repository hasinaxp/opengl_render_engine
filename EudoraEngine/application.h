#pragma once
#include "api.h"
#include "display.h"
#include "deps/glad.h"
#include "eventSystem.h"
#include <string>


namespace sp {

	//base class for application layer
	// must be implemented(inherited) with two methods:
	// init(), update(float dt);
	class SP_API ApplicationLayer
	{
	private:
		float _dt; //time in second
		std::string _name;
	public:
		ApplicationLayer(std::string name) { _name = name; };
		virtual ~ApplicationLayer() {};
		virtual void onInit() = 0;
		virtual void onLoad() {};
		virtual void onExit() {};
		virtual void onUpdate(float dt) = 0; // time in second
		virtual void fastUpdate(float dt) {}; // runs on different thread. // used variables must be deadlock protected
		virtual void clean() {};
		void appUpdate(float dt)
		{
			onUpdate(dt);
			_dt = dt;
		}
		
		virtual void onRender() {};
		float getDeltaTime_s() const { return _dt; }
		float getDeltaTime_ms() const { return _dt * 1000; }
		std::string getName() const { return _name; }

	};

	class SP_API Application
	{
	private:
		static Display* _display;
		static float _accumulatedTime;
		static float _currentTime;
		static float _lastTime;
		static int _frameRate;
		static ApplicationLayer* _currentLayer;
		static ApplicationLayer* _overlayLayer;
		static std::vector<ApplicationLayer*> _application_layers;
	public:
		static void create(int width = 640, int height = 480, const char* title = "application");
		static void run();
		static void close();
		static void destroy();
		static void resizeWindow(int width, int height);
		static void lockMouse(bool lock);
		static void pushLayer(ApplicationLayer* layer);
		static void pushOverlayLayer(std::string name);
		static void popOverlayLayer();
		static void switchLayer(std::string name);

		static int getWidth() { return _display->getWidth(); }
		static int getHeight() { return _display->getHeight(); }
		static int getFrameRate() { return _frameRate; }
		static Display* getMainDisplay() { return _display; }


		static void setFrameRate(int frameRate) { _frameRate = frameRate; }

		static void fastLoop();
	private:
		static void renderLoop();
		static void loop();


	};

};