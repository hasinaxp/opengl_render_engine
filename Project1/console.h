#pragma once
#include "api.h"
#include <vector>
#include <string>
#include <typeinfo>
#include "deps/glm/glm.hpp"



namespace sp {

	//types for console font color
	enum class SP_API ConsoleColorType
	{
		forground_blue = 9,
		forground_sky = 11,
		forground_grey = 10,
		forground_red = 12,
		forground_pink = 13,
		forground_green = 10,
		forground_yellow = 14,
		forground_white = 15
	};

	//class responsible for logging things in console
	class SP_API Console
	{

	private:
		static int _log_filter;

	public:
		static void setConsoleColor(ConsoleColorType color);
		static void setConsoleColor(int color_code);

		static void vec4(glm::vec4 v);
		static void vec3(glm::vec3 v);
		static void mat4(glm::mat4 m);
		static void str(std::string s);
		template <typename T>
		static void vector(std::vector<T> v)
		{
			for (auto it = v.begin(); it != v.end(); it++)
			{
				if (typeid(T) == typeid(glm::vec3))
					vec3(glm::vec3(*it));
				else if (typeid(T) == typeid(glm::mat4))
					mat4((glm::mat4)(*it));
				else if (typeid(T) == typeid(glm::vec4))
					vec4((glm::vec4)(*it));
				else
					real(float(*it));

			}
		}
		static void label(std::string s, float f);
		static void label(std::string s, glm::vec3 v);
		static void label(std::string s, glm::vec4 v);
		static void label(std::string s, glm::mat4 v);
		static void err(std::string s, std::string trace = "");
		static void real(float f);
		static void real(unsigned int u);
		static void v_vec3(std::vector<glm::vec3>& v);
		static void gl_debug(unsigned int type, const char* message);

		static void set_log_filter(int i) { _log_filter = i; }
	};

};