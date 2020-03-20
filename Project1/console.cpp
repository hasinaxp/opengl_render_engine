#include "console.h"
#include <iostream>
#include <iomanip>
#include <conio.h>
#include "deps/glad.h"
#include <boost/stacktrace.hpp>
#include <SDL.h>

#include <Windows.h>

namespace sp {


	int Console::_log_filter = 1;


	void Console::setConsoleColor(ConsoleColorType color)
	{
		int color_code = static_cast<int>(color); //white
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color_code);
	}

	void Console::setConsoleColor(int color_code)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color_code);
	}

	void Console::vec4(glm::vec4 v)
	{
		std::cout << "( " << v.x << ", " << v.y << ", " << v.z << ", " << v.w << " )" << std::endl;
	}
	void Console::vec3(glm::vec3 v)
	{
		std::cout << "( " << v.x << ", " << v.y << ", " << v.z << " )" << std::endl;
	}
	void Console::mat4(glm::mat4 m)
	{
		setConsoleColor(ConsoleColorType::forground_green);
		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 4; x++)
			{
				std::cout << std::setw(6) << std::setprecision(4) << m[y][x];
			}
			std::cout << std::endl;
		}
		setConsoleColor(ConsoleColorType::forground_white);
	}
	void Console::str(std::string s)
	{
		std::cout << s << std::endl;
	}
	void Console::label(std::string s, float f)
	{
		setConsoleColor(ConsoleColorType::forground_yellow);
		std::cout << s << std::endl;
		setConsoleColor(ConsoleColorType::forground_white);
		Console::real(f);
	}
	void Console::label(std::string s, glm::vec3 v)
	{
		setConsoleColor(ConsoleColorType::forground_yellow);
		std::cout << s << std::endl;
		setConsoleColor(ConsoleColorType::forground_white);
		Console::vec3(v);
	}
	void Console::label(std::string s, glm::vec4 v)
	{
		setConsoleColor(ConsoleColorType::forground_yellow);
		std::cout << s << std::endl;
		setConsoleColor(ConsoleColorType::forground_white);
		Console::vec4(v);
	}
	void Console::label(std::string s, glm::mat4 m)
	{
		setConsoleColor(ConsoleColorType::forground_yellow);
		std::cout << s << std::endl;
		setConsoleColor(ConsoleColorType::forground_white);
		Console::mat4(m);
	}
	void Console::err(std::string s, std::string trace)
	{
		setConsoleColor(ConsoleColorType::forground_yellow);
		std::cout << "err : ";
		setConsoleColor(ConsoleColorType::forground_red);
		std::cout << s << std::endl;
		if (trace != "")
		{
			setConsoleColor(ConsoleColorType::forground_yellow);
			std::cout << "trace : ";
			setConsoleColor(ConsoleColorType::forground_red);
			std::cout << trace << std::endl;
		}
		setConsoleColor(ConsoleColorType::forground_yellow);
		std::cout << "stack trace : " << std::endl;
		setConsoleColor(ConsoleColorType::forground_red);
		std::cout << boost::stacktrace::stacktrace() << std::endl;
		SDL_Quit();
		setConsoleColor(ConsoleColorType::forground_white);
		std::cout << "press any key to terminate!..";
		exit(1);

	}
	
	void Console::real(float f)
	{
		std::cout << f << std::endl;
	}
	void Console::real(unsigned int u)
	{
		std::cout << u << std::endl;
	}
	void Console::v_vec3(std::vector<glm::vec3>& v)
	{
		for (auto it = v.begin(); it != v.end(); it++)
		{
			vec3(*it);
		}
	}
	void Console::gl_debug(unsigned int type, const char * message)
	{
		std::string t = "";
		if (type == GL_DEBUG_TYPE_ERROR && _log_filter > 0)
		{
			t = "error";
		}
		else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR && _log_filter > 0)
		{
			t = "undefined behavior";
		}
		else if (type == GL_DEBUG_TYPE_PERFORMANCE && _log_filter > 1)
		{
			t = "performance";
		}
		else if (type == GL_DEBUG_TYPE_OTHER && _log_filter > 2)
		{
			t = "debug other";
		}
		if (t != "") {
			setConsoleColor(ConsoleColorType::forground_yellow);
			std::cout << "opengl debug: " << "( " << t << " )" << std::endl;
			setConsoleColor(ConsoleColorType::forground_green);
			std::cout << "message:" << std::endl;
			setConsoleColor(ConsoleColorType::forground_white);
			std::cout << message << std::endl;

		}
	}


}