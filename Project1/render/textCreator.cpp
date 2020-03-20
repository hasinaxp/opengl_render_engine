#include "textCreator.h"
#include "../console.h"
#include "../deps/glm/gtc/matrix_transform.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H 
#include <fstream>
#include <boost/algorithm/string/classification.hpp> // Include boost::for is_any_of
#include <boost/algorithm/string/split.hpp> // Include for boost::split
#include "../deps/glad.h"

namespace sp {

	const char* xTextVertexShaderSource = R"(
	#version 330 core
	layout (location = 0) in vec4 cord_uv; // <vec2 pos, vec2 tex>
	layout (location = 1) in vec4 crop;
	layout (location = 2) in vec4 color;

	out vec2 tex_cord;
	out vec4 color_text;

	uniform mat4 projection_matrix;

	void main()
	{
		gl_Position = projection_matrix *  vec4(cord_uv.xy, 0.0, 1.0);
		tex_cord.x = crop.x + cord_uv.z * crop.z;
		tex_cord.y = crop.y + cord_uv.w * crop.w;
		color_text = color;
	}
	)";

	const char* xTextFragmentShaderSource = R"(
	#version 330 core
	in vec2 tex_cord;
	in vec4 color_text;
	out vec4 color;

	uniform sampler2D font1;

	void main()
	{
		vec4 sampled = vec4(1.0, 1.0, 1.0, texture(font1, tex_cord).r);
		color = color_text * sampled;

	}  
	)";


	TextCreator::TextCreator()
		:_docShader(nullptr),
		_docVao(nullptr),
		_currentTextSettings(TextSettings()),
		_currentParaSettings(ParaSettings()),
		_cursor({-1.0f, 1.0f}),
		_fontCount(0),
		_fontAtlasMap({}),
		_fontcharMapMap({}),
		_pixel_size(16),
		text_model({}),
		indices({}),
		_charmap({}),
		_fontAtlas(nullptr),
		_scale(glm::vec2(0.02f)),
		scale(0.2),
		_blockWidth(0.0f),
		_docWidth(0.0f),
		_blockLeft(0.0f),
		_docLeft(0.0f),
		_orthoCam(nullptr),
		_last_height(0.0f),
		_innerText(""),
		ind_cnt(0)
	{
		_orthoCam = new Camera(Camera::genOrthoMatrix(2.0f, 2.0f, true), {0.0f, 0.0f, 2.0f});
		_docShader = new ShaderProgram({ std::make_pair(xTextVertexShaderSource, ShaderSourceType::vertex), std::make_pair(xTextFragmentShaderSource, ShaderSourceType::fragment) });
	}

	TextCreator::~TextCreator()
	{
		if (_docVao != nullptr)
			delete _docVao;
		if (_docShader != nullptr)
			delete _docShader;
		for (auto fnt : _fontAtlasMap) {
			if(fnt.second != nullptr)
				delete fnt.second;
		}
		if (_fontAtlas != nullptr)
			delete _fontAtlas;
	}

	uint TextCreator::submitFont(std::string filepath, uint size)
	{
		_pixel_size = size;
		//loading the font
		SPCharMap _charmap = {};
		//font loading
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
			Console::err("could not init freetype.");

		FT_Face face;
		if (FT_New_Face(ft, filepath.c_str(), 0, &face))
			Console::err("unable to load font.", filepath);

		FT_Set_Pixel_Sizes(face, 0, _pixel_size);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
		std::vector<ImageData*> font_images = {};
		for (GLubyte c = 0; c < 127; c++)
		{
			// Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER ))
			{
				Console::err("ERROR::FREETYTPE: Failed to load Glyph", (const char*)c);
				continue;
			}

			ImageData* img = new ImageData(face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, 1);
			if (img->height > 10 && img->width > 10) {
				float filterSharp[] = {
					 0.0f , -1.0f, 0.0f,
					 -1.0f , 5.0f, -1.0f,
					0.0f , -1.0f, 0.0f,
					};
				

				img->applyFilterKernal(filterSharp, 3);
			}
			font_images.push_back(img);
			// store character 
			s_char_render_info character = {
				font_images.size() - 1,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			_charmap.insert(std::pair<GLchar, s_char_render_info>(c, character));
		}
		SpriteSheet *_font_atlas = new SpriteSheet(font_images);

		//adding data to font map
		_fontCount++;
		_fontAtlasMap.insert(std::make_pair(_fontCount, _font_atlas));
		_fontcharMapMap.insert(std::make_pair(_fontCount, _charmap));

		//memory deallocation
		FT_Done_Face(face);
		FT_Done_FreeType(ft);
		for (auto i : font_images)
		{
			delete i;
		}

		return _fontCount;
	}

	void TextCreator::beginDocument(DocSetting settings)
	{
		_innerText = "";
		ind_cnt = 0;
		if(_docVao == nullptr)
			_docVao = new VertexArray();
		_scale.y = 2.0f/ _pixel_size;
		_scale.x = 2.0f * (float)Application::getHeight() / (Application::getWidth() * _pixel_size);
		scale = settings.defaultParaSettings.textSettings.fontSize;
		_charmap = _fontcharMapMap[settings.defaultParaSettings.textSettings.fontId];

		_blockWidth = settings.defaultParaSettings.width;
		_docWidth = settings.width;
		_blockLeft = settings.defaultParaSettings.startIndent;
		_docLeft = settings.marginLeft;
		_cursor = { -1.0f + _docLeft , 1.0f - settings.marginTop };
		text_model = {};
		indices = {};

		_currentParaSettings = settings.defaultParaSettings;
		_currentTextSettings = settings.defaultParaSettings.textSettings;
		
		
	}

	void TextCreator::paragraph(std::string text)
	{
		paragraph(text, _currentParaSettings);
	}

	uint TextCreator::endDocument()
	{
		_docVao->setVertexBufferData(&text_model[0], sizeof(text_model[0]) * text_model.size(), GL_DYNAMIC_DRAW);
		_docVao->setIndexBufferVector(indices);
		_docVao->setVertexBufferLayout({ { 0, sizeof(cdata), 4, 'f' },{ offsetof(cdata,crop),sizeof(cdata),  4, 'f' }, { offsetof(cdata,color),sizeof(cdata),  4, 'f' } });
		return uint();
	}

	void TextCreator::drawDocument()
	{
		// enable alpha blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		_docShader->bind();

		_docShader->uniform_m4(_orthoCam->getViewProjectionMatrix(), "projection_matrix");


		for (auto fontAtl : _fontAtlasMap)
			fontAtl.second->getTexture()->bind(_docShader, 6+fontAtl.first, std::string("font" + std::to_string(fontAtl.first)).c_str());
		
		_docVao->draw();

		_docShader->unbind();
		//disable alpha blend
		glDisable(GL_BLEND);
	}

	void TextCreator::moveCursorVertical(float x)
	{
		_cursor.y -= x;
	}

	void TextCreator::moveCursorHorizontal(float x)
	{
		_cursor.x += x;
	}

	void TextCreator::paragraph(std::string text, ParaSettings settings)
	{
		_docLeft += settings.paddingLeft;
		_innerText += '\n';
		// adjust cursor and block
		_cursor.y -= settings.paddingTop +_last_height;
		_cursor.x =-1.0f +  _docLeft + settings.startIndent;
		_blockWidth = _docWidth - settings.paddingRight - _docLeft;
		scale = settings.textSettings.fontSize;
		//seperate into lines
		std::vector<std::string> lines;
		boost::split(lines, text, boost::is_any_of("\n"), boost::token_compress_on);
		for (std::string textLine : lines) {

			//calculate line width
			glm::vec2 cur2 = glm::vec2(0.0f);
			float adjFact = 0.03;
			for (char ch : textLine) {
					cur2.x += (_charmap[ch].advance >> 6)* _scale.x* scale  + settings.textSettings.letterSpace;
				if(ch == ' ')
					cur2.x += settings.textSettings.wordSpace;
				
			}
			if (cur2.x < _blockWidth) {
				if (settings.textAlignment == TextAlignment::center) {
					_cursor.x += (_blockWidth - cur2.x - settings.startIndent - adjFact) / 2  ;
				}
				if (settings.textAlignment == TextAlignment::right) {
					_cursor.x += (_blockWidth - cur2.x) - (settings.startIndent +adjFact);
				}
				
			}
			textBlock(textLine, settings.textSettings);
			_innerText += '\n';
			float vergap = _fontAtlas->getCellHeight() * _scale.y * scale;
			if (vergap < settings.textSettings.lineHeight) {
				vergap = settings.textSettings.lineHeight;
			}
			_cursor.y -= vergap;
			_cursor.x = -1.0f + _docLeft + settings.startIndent;
			_docLeft -= settings.paddingLeft;
		}


		// adjust cursor and block
		_cursor.y -= settings.paddingBottom;
		_cursor.x = -1.0f + _docLeft;
		_blockWidth = _docWidth;
		_last_height = settings.textSettings.lineHeight;
	}

	void TextCreator::textBlock(std::string text)
	{
		textBlock(text, _currentTextSettings);
	}

	void TextCreator::textBlock(std::string text, TextSettings settings)
	{
		_innerText += text;
		_fontAtlas = _fontAtlasMap[settings.fontId];
		scale = settings.fontSize;
		float cf = _cursor.x;
		std::string::const_iterator c;
		for (auto c : text)
		{
			s_char_render_info ch = _charmap[c];
			if (c == '\n')
			{
				float vergap = _fontAtlas->getCellHeight() * _scale.y * scale;
				if (vergap < settings.lineHeight) {
					vergap = settings.lineHeight;
				}
				_cursor.y -= vergap;
				_cursor.x = cf;
				continue;
			}
			if (c == '\t')
			{
				_cursor.x += 4 * ((ch.advance >> 6)* _scale.x* scale);
				continue;
			}
			if (c == ' ') {
				_cursor.x += settings.wordSpace;
			}

			float xpos = _cursor.x + ch.bearing.x * _scale.x * scale;
			float ypos = _cursor.y - _fontAtlas->getCellHeight() * _scale.y * scale - (ch.size.y - ch.bearing.y) * _scale.y * scale;
			float w = ch.size.x * _scale.x * scale;
			float h = ch.size.y * _scale.y * scale;

			float italicBias = 0.0f;
			if (settings.isItalic) {
				italicBias = 14.0 *_scale.y * scale;
			}
			cdata cd[4];
			cd[0].cord_uv = { xpos, ypos + h, 0.0, 0.0 };
			cd[1].cord_uv = { xpos - italicBias, ypos, 0.0, 1.0 };
			cd[2].cord_uv = { xpos + w - italicBias, ypos, 1.0, 1.0 };
			cd[3].cord_uv = { xpos + w, ypos + h, 1.0, 0.0 };

			for (int i = 0; i < 4; i++) {
				cd[i].crop = _fontAtlas->getCrop(ch.sprite_key);
				cd[i].color = glm::vec4(settings.color, settings.opacity);
			}


			text_model.push_back(cd[0]);
			text_model.push_back(cd[1]);
			text_model.push_back(cd[2]);
			text_model.push_back(cd[3]);

			indices.push_back(ind_cnt);
			indices.push_back(ind_cnt + 1);
			indices.push_back(ind_cnt + 2);
			indices.push_back(ind_cnt + 0);
			indices.push_back(ind_cnt + 2);
			indices.push_back(ind_cnt + 3);

			ind_cnt += 4;

			// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			_cursor.x += (ch.advance >> 6)* _scale.x* scale + settings.letterSpace; // Bitshift by 6 to get value in pixels (2^6 = 64)
			float cursorXNext = _cursor.x + _fontAtlas->getCellWidth() * _scale.y * scale;
			if (cursorXNext >= cf + (float)_blockWidth)
			{
				_cursor.x = cf;
				_cursor.y -= _fontAtlas->getCellHeight() * _scale.y * scale;
			}
			else if (cursorXNext >= -1.0f + (float)_docLeft + (float)_docWidth) {
				_cursor.x = -1.0f + (float)_docLeft;
				_cursor.y -= _fontAtlas->getCellHeight() * _scale.y * scale;
			}
			
		}
		_last_height = _fontAtlas->getCellHeight() * _scale.y * scale;
		if (_last_height < settings.lineHeight) {
			_last_height = settings.lineHeight;
		}
	}

};