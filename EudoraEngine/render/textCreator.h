#pragma once
#include "../api.h"
#include "../deps/glm/glm.hpp"
#include "../application.h"
#include "vertexArray.h"
#include "texture.h"
#include "shaderProgram.h"
#include "../control/camera.h"
#include <string>
#include <map>

namespace sp {

	enum class TextAlignment {
		center = 0,
		left = 1,
		right = 2
	};

	struct TextSettings {
		float fontSize = 0.03f;
		int id = 0;
		glm::vec3 color = glm::vec3(0.2f);
		float opacity = 1.0;
		float lineHeight = 0.04f;
		float letterSpace = 0.001f;
		float wordSpace = 0.02f;
		int fontId = 1;
		bool isItalic = false;
	};

	struct ParaSettings {
		float width = 2.0f;
		TextSettings textSettings;
		TextAlignment textAlignment = TextAlignment::left;
		float paddingTop = 0.01f;
		float paddingBottom = 0.01f;
		float startIndent = 0.01f;
		float paddingLeft = 0.01f;
		float paddingRight = 0.01f;
		glm::vec4 backgroundColor = glm::vec4(0.0f); 
	};

	struct DocSetting {
		float width = 2.0f;
		float height;
		float marginTop = 0.01f;
		float marginLeft = 0.01f;
		ParaSettings defaultParaSettings;
	};


	struct s_char_render_info
	{
		uint     sprite_key;  // ID handle of the glyph texture
		glm::ivec2 size;     // Size of glyph
		glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
		GLuint     advance;    // Offset to advance to next glyph
	};

	struct cdata
	{
		glm::vec4 cord_uv;
		glm::vec4 crop;
		glm::vec4 color = glm::vec4(0.2f);
	};

	typedef std::map<GLchar, s_char_render_info> SPCharMap;

	class SP_API TextCreator {
	private:
		ShaderProgram* _docShader;
		VertexArray* _docVao;
		TextSettings _currentTextSettings;
		ParaSettings _currentParaSettings;
		glm::vec2 _cursor;
		int _fontCount;
		std::map<uint, SpriteSheet*> _fontAtlasMap;
		std::map<uint, SPCharMap> _fontcharMapMap;
		uint _pixel_size;
		std::vector<cdata> text_model;
		std::vector<uint> indices;
		SPCharMap _charmap;
		SpriteSheet* _fontAtlas;
		glm::vec2 _scale;
		float scale; //!change the name
		float _blockWidth;
		float _docWidth;
		float _blockLeft;
		float _docLeft;

		float _widthViewport;
		float _heightViewport;
		Camera* _orthoCam;
		float _last_height;
		std::string _innerText;
		int ind_cnt;

	public:
		TextCreator();
		~TextCreator();

		uint submitFont(std::string filepath, uint size = 72);
		SpriteSheet* getFontSpriteSheet(int id) { return _fontAtlasMap[id]; }
		SPCharMap getFontCharMap(int id) { return _fontcharMapMap[id]; }
		VertexArray* getDocumentVertexArray() const { return _docVao; }

		void beginDocument(DocSetting settings);
		void paragraph(std::string text);
		void paragraph(std::string text, ParaSettings settings);
		void textBlock(std::string text);
		void textBlock(std::string text, TextSettings settings);
		//void number(int &number, TextSettings settings);
		//void verText(std::string text, TextSettings settings);
		//void line(glm::vec2 p1, glm::vec2 p2, float width);


		uint endDocument();

		void drawDocument();

		void moveCursorVertical(float x);
		void moveCursorHorizontal(float x);
		glm::vec2 getCursor() const { return _cursor; }
		void setCursor(glm::vec2 p) { _cursor = p; }

		ParaSettings getCurrentParaSettings() const { return _currentParaSettings; }
		TextSettings getCurrentTextSettings() const { return _currentTextSettings; }
		void setCurrentParaSettings(ParaSettings settings){ _currentParaSettings = settings; }
		void setCurrentTextSettings(TextSettings settings){ _currentTextSettings = settings; }

		Camera* getCam() const { return _orthoCam; }

		std::string getInnerText() const { return _innerText; }

	};

};
