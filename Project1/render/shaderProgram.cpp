#include "shaderProgram.h"
#include <utility>
#include "../deps/glm/gtc/type_ptr.hpp"
#include "../console.h"
#include <fstream>
#include <sstream>


namespace sp {


	ShaderProgram::ShaderProgram(std::vector<std::string> paths)
	{
		_program = 0;
		std::vector<std::pair<std::string, ShaderSourceType>> sources = {};
		for (std::string fl : paths)
		{
			ShaderSourceType type;
			std::string ext = fl.substr(fl.find_last_of(".") + 1);
			if (ext == "vert")
				type = ShaderSourceType::vertex;
			else if (ext == "frag")
				type = ShaderSourceType::fragment;
			else if (ext == "geom")
				type = ShaderSourceType::geometry;
			else
			{
				Console::err("unknown extension type > " + ext, fl);
			}
			std::ifstream file(fl);
			if (!file.is_open())
			{
				Console::err("file could not be opened!", fl);
			}
			std::stringstream ss;
			ss << file.rdbuf();
			file.close();
			std::string text = ss.str();
			sources.push_back(std::make_pair(text, type));
		}
		compile(sources);
	}

	ShaderProgram::ShaderProgram(std::vector<std::pair<std::string, ShaderSourceType>> sources)
	{
		_program = 0;
		compile(sources);
	}

	ShaderProgram::~ShaderProgram()
	{
		glDeleteProgram(_program);
	}


	void ShaderProgram::compile(std::vector<std::pair<std::string, ShaderSourceType>> sources)
	{

		std::vector<uint> shader_ids = {};
		int success;
		char info_log[512];
		//compile all the shaders
		for (auto source : sources)
		{
			if (_program != 0)
			{
				glDeleteProgram(_program);
				_program = 0;
			}
			const char* source_code = source.first.c_str();
			GLenum shaderType = static_cast<GLenum>(source.second);
			uint shader_id = glCreateShader(shaderType);
			glShaderSource(shader_id, 1, &source_code, 0);
			glCompileShader(shader_id);
			glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader_id, 512, 0, info_log);
				std::string shader_type_txt;
				switch (source.second) {
				case ShaderSourceType::vertex:
					shader_type_txt = "vertex shader";
					break;
				case ShaderSourceType::fragment:
					shader_type_txt = "fragment shader";
					break;
				case ShaderSourceType::geometry:
					shader_type_txt = "geometry shader";
					break;
				}
				std::string msg = "could not compile Shader > " + shader_type_txt;
				Console::err(msg, info_log);
			}
			shader_ids.push_back(shader_id);
		}
		//link all the shaders and create program
		_program = glCreateProgram();
		for (uint id : shader_ids)
		{
			glAttachShader(_program, id);
		}
		glLinkProgram(_program);
		glGetProgramiv(_program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(_program, 512, 0, info_log);
			Console::err("could not link program", info_log);
		}
		//remove all shaders
		for (uint id : shader_ids)
		{
			glDetachShader(_program, id);
			glDeleteShader(id);
		}

	}

	void ShaderProgram::bind()
	{
		glUseProgram(_program);
	}

	void ShaderProgram::unbind()
	{
		glUseProgram(0);
	}

	void ShaderProgram::uniform_v3(glm::vec3 v, const char* name)
	{
		glUniform3f(getUniformLocation(name), v.x, v.y, v.z);
	}

	void ShaderProgram::uniform_v3_vector(std::vector<glm::vec3> vs, const char* name)
	{
		glUniform3fv(getUniformLocation(name), vs.size(), (float*)(&vs[0]));
	}

	void ShaderProgram::uniform_v4(glm::vec4 v, const char* name)
	{
		glUniform4f(getUniformLocation(name), v.x, v.y, v.z, v.w);
	}

	void ShaderProgram::uniform_v4_vector(std::vector<glm::vec4> vs, const char* name)
	{
		glUniform4fv(getUniformLocation(name), vs.size(), (float*)(&vs[0]));
	}

	void ShaderProgram::uniform_m4(glm::mat4 m, const char* name)
	{
		glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(m));
	}

	void ShaderProgram::uniform_m4_vector(std::vector<glm::mat4> ms, const char* name)
	{
		glUniformMatrix4fv(getUniformLocation(name), ms.size(), GL_FALSE, (float*)(&ms[0]));
	}

	void ShaderProgram::uniform_f(float f, char* name)
	{
		glUniform1f(getUniformLocation(name), f);
	}

	void ShaderProgram::uniform_i(int i, char* name)
	{
		glUniform1i(getUniformLocation(name), i);
	}

	uint ShaderProgram::getUniformLocation(std::string name) const
	{
		if (_locationCache.find(name) != _locationCache.end())
			return _locationCache[name];
		_locationCache[name] = glGetUniformLocation(_program, name.c_str());
		return _locationCache[name];
	}



};