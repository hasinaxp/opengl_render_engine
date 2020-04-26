#pragma once
#include "api.h"
#include <vector>



namespace sp {

	//class responsible for creating window
	class SP_API Display
	{
	private:
		void* _window = nullptr;
		void* _context = nullptr;
		int _width = 640;
		int _height = 480;
	public:
		Display(int width, int height, const char* title);
		~Display();
		void show(bool should = true);
		void onUpdate();
		void resizeWindow(int width, int height);
		int getWidth() const { return _width; }
		int getHeight() const { return _height; }
		void* getWindowHandle() { return _window; }

		static void setClearColor(float r, float g, float b, float a);
	};

}