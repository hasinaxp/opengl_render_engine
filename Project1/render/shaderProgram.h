#pragma once
#include "../api.h"
#include "../deps/glad.h"
#include "../deps/glm/glm.hpp"
#include <unordered_map>
#include <string>


namespace sp {


	//constants for default prefix
	const std::string cnst_prefix_texture = "tex_";
	const std::string cnst_prefix_font = "font_";

	//default constants for shaders
	const char* const cnst_txt_matrix_model = "model_matrix";
	const char* const cnst_txt_matrix_projection = "projection_matrix";
	const char* const cnst_txt_matrix_view = "view_matrix";

	const char* const cnst_txt_font_texture = "font_texture";
	const char* const cnst_txt_font_color = "font_color";


	//types of all the available shader
	enum class ShaderSourceType
	{
		vertex = GL_VERTEX_SHADER,
		fragment = GL_FRAGMENT_SHADER,
		geometry = GL_GEOMETRY_SHADER,
		tesselation_control = GL_TESS_CONTROL_SHADER,
		tesselation_evaluation = GL_TESS_EVALUATION_SHADER,
		compute = GL_COMPUTE_SHADER,
	};

	//this class can create glsl shaders from sources
	//all basic functionalityies of a shader program are implemented here
	class SP_API ShaderProgram
	{
	private:
		uint _program;
		mutable std::unordered_map<std::string, int> _locationCache;

	public:
		ShaderProgram(std::vector<std::string> paths);
		ShaderProgram(std::vector<std::pair<std::string, ShaderSourceType>> sources);
		~ShaderProgram();

		unsigned int get_program() const { return _program; }

		void compile(std::vector<std::pair<std::string, ShaderSourceType>> sources);
		void bind();
		void unbind();

		//uniform uploading functions
		void uniform_v3(glm::vec3 v, const char* name);
		void uniform_v3_vector(std::vector<glm::vec3> vs, const char* name);
		void uniform_v4(glm::vec4 v, const char* name);
		void uniform_v4_vector(std::vector<glm::vec4> vs, const char* name);
		void uniform_m4(glm::mat4 m, const char* name);
		void uniform_m4_vector(std::vector<glm::mat4> ms, const char* name);
		void uniform_f(float f, char* name);
		void uniform_i(int i, char* name);

	private:
		uint getUniformLocation(std::string name) const;
	};


	namespace ColorReferance {

		namespace Color {
			const glm::vec3 White = glm::vec3(1.0f);
			const glm::vec3 PaperWhite = glm::vec3(0.953f, 0.898f, 0.961f);
			const glm::vec3 Red = glm::vec3(0.835f, 0.000f, 0.000f);
			const glm::vec3 Tomato = glm::vec3(1.000f, 0.388f, 0.278f);
			const glm::vec3 Brick = glm::vec3(0.718f, 0.110f, 0.110f);
			const glm::vec3 Pink = glm::vec3(0.961f, 0.000f, 0.341f);
			const glm::vec3 LightPink = glm::vec3(1.000f, 0.502f, 0.671f);
			const glm::vec3 SalmonRed = glm::vec3(0.937f, 0.325f, 0.314f);
			const glm::vec3 Purple = glm::vec3(0.533f, 0.055f, 0.310f);
			const glm::vec3 Skin = glm::vec3(1.000f, 0.541f, 0.502f);
			const glm::vec3 PurpleBlue = glm::vec3(0.290f, 0.078f, 0.549f);
			const glm::vec3 IndigoBLue = glm::vec3(0.188f, 0.247f, 0.624f);
			const glm::vec3 Blue = glm::vec3(0.188f, 0.310f, 0.996f);
			const glm::vec3 SkyBlue = glm::vec3(0.733f, 0.871f, 0.984f);
			const glm::vec3 CerulianBlue = glm::vec3(0.259f, 0.647f, 0.961f);
			const glm::vec3 NeviBlue = glm::vec3(0.000f, 0.000f, 0.502f);
			const glm::vec3 Cyan = glm::vec3(0.161f, 0.714f, 0.965f);
			const glm::vec3 GreenBlue = glm::vec3(0.000f, 0.898f, 1.000f);
			const glm::vec3 Teal = glm::vec3(0.000f, 0.588f, 0.533f);
			const glm::vec3 Green = glm::vec3(0.000f, 0.784f, 0.325f);
			const glm::vec3 SapGreen = glm::vec3(0.220f, 0.557f, 0.235f);
			const glm::vec3 DarkSapGreen = glm::vec3(0.106f, 0.369f, 0.125f);
			const glm::vec3 OliveGreen = glm::vec3(0.463f, 1.000f, 0.012f);
			const glm::vec3 LightGreen = glm::vec3(0.392f, 0.867f, 0.090f);
			const glm::vec3 GrassGreen = glm::vec3(0.408f, 0.624f, 0.220f);
			const glm::vec3 Lime = glm::vec3(0.776f, 1.000f, 0.000f);
			const glm::vec3 Yellow = glm::vec3(1.000f, 1.000f, 0.000f);
			const glm::vec3 Orange = glm::vec3(0.976f, 0.659f, 0.145f);
			const glm::vec3 IndianYellow = glm::vec3(1.000f, 0.671f, 0.000f);
			const glm::vec3 Steel = glm::vec3(0.329f, 0.431f, 0.478f);
			const glm::vec3 InkBlack = glm::vec3(0.149f, 0.196f, 0.220f);
			const glm::vec3 Black = glm::vec3(0.0f);
			const glm::vec3 Grey = glm::vec3(0.459f, 0.459f, 0.459f);
			const glm::vec3 LightGrey = glm::vec3(0.741f, 0.741f, 0.741f);
			const glm::vec3 Brown = glm::vec3(0.427f, 0.298f, 0.255f);
			const glm::vec3 TreeBrown = glm::vec3(0.545f, 0.217f, 0.075f);
			
		};

		inline glm::vec3 ColorHex(uint hexValue) {
			glm::vec3 rgbColor;
			rgbColor.r = ((hexValue >> 16) & 0xFF) / 255.0;  // Extract the RR byte
			rgbColor.g = ((hexValue >> 8) & 0xFF) / 255.0;   // Extract the GG byte
			rgbColor.b = ((hexValue) & 0xFF) / 255.0;
			return rgbColor;
		}
	};


};