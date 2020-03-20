#pragma once
#include "../api.h"
#include "../deps/glad.h"
#include "shaderProgram.h"
#include <map>
#include <list>


namespace sp {

	//datas tructure represent a raw image 
	struct SP_API ImageData
	{
		uint num_channel;
		uint buffer_size;
		byte* buffer;
		uint width;
		uint height;

		ImageData(byte* data, uint w, uint h, uint nc = 4);
		~ImageData();
		void applyFilterKernal(float kernal[], int kernalWidth);
		void appDistanceTransform1D();
		void rotateRight();
		void rotateLeft();
		static ImageData* genImage(std::string filepath);
		static std::vector<ImageData*>genImageVector(std::string filepath, int num_colums, int num_rows);
		static void flipImageX(ImageData* img);
		static void flipImageY(ImageData* img);

		static void saveImage_png(ImageData* image, std::string path);

	};


	enum class SP_API TextureType
	{
		flat = GL_TEXTURE_2D,
		cubemap = GL_TEXTURE_CUBE_MAP
	};

	//def: class responsible for handling textures
	class SP_API Texture 
	{
	private:
		int _width;
		int _height;
		TextureType _type;
		uint _texture_id;
		uint _slot;
	public:
		Texture(int width, int height, TextureType type = TextureType::flat);
		~Texture();

		void bind();
		void bind(ShaderProgram* sp, int slot = 0, const char* name = "texture");
		void unbind();

		uint getTextureId() const { return _texture_id; }
		TextureType getTextureType() const { return _type; }
		int getWidth() const { return _width; }
		int getHeight() const { return _height; }
		std::vector<byte> getPixelVector_rgb();

		void setBufferData(void* data, int width, int height, uint storage_type = GL_RGBA, uint data_type = GL_UNSIGNED_BYTE);

		static Texture* genTextureFlat(const char* filepath);
		static Texture* genTextureFlat(ImageData* image);
		static Texture* genTextureCubemap(std::vector<const char*> filepaths);
		static Texture* genTextureCubemap(std::vector<ImageData*> images);
		static Texture* genTextureCubemap(std::string filepath);

		template<typename T>
		static void fillArray2d(T* array, T* data, uint cols, uint dcols, uint x, uint y, uint width, uint height, uint nc = 1, uint dx = 0, uint dy = 0)
		{
			try
			{
				uint stride = nc;
				for (int a = 0; a < height; a++)
				{
					uint offset = ((y + a) * cols + x);
					uint doffset = ((dy + a) * dcols + dx);
					for (int b = 0; b < width; b++)
					{
						for (int i = 0; i < stride; i++) {
							array[(offset + b) * stride + i] = data[(doffset + b) * stride + i];
						}
					}
				}
			}
			catch (...)
			{

			}
		}
	};


	class SP_API SpriteSheet
	{
	private:
		uint _sheet_width, _sheet_height;
		uint _cell_width, _cell_height;
		std::vector<glm::vec4> _crops;
		Texture* _texture;

	public:
		SpriteSheet(std::vector<ImageData*> images);
		SpriteSheet(ImageData* image, std::vector<glm::vec4> crops);
		SpriteSheet(ImageData* image, float cellWidth, float cellHeight);
		~SpriteSheet();

		glm::vec4 getCrop(int i)
		{
			if (i < _crops.size())
				return _crops[i];
			else
				return glm::vec4(0);
		}
		std::vector<glm::vec4> getCrops() const { return _crops; }
		Texture* getTexture() const { return _texture; }
		uint getSheetWidth() const { return _sheet_width; }
		uint getSheetHeight() const { return _sheet_height; }
		uint getCellWidth() const { return _cell_width; }
		uint getCellHeight() const { return _cell_height; }


	};


};