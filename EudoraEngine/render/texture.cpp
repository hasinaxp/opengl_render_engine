#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <cstdio>
#include "../deps/stb_image.h"
#include "../deps/stb_image_write.h"
#include "../console.h"
#include <cmath>



namespace sp {



	ImageData::ImageData(byte* data, uint w, uint h, uint nc)
		:width(w),
		height(h),
		num_channel(nc)
	{
		buffer_size = w * h * nc;
		buffer = (byte*)malloc(buffer_size * sizeof(byte));
		memcpy(buffer, data, buffer_size * sizeof(byte));
	}
	ImageData::~ImageData()
	{
		delete buffer;
	}

	void ImageData::applyFilterKernal(float kernal[], int kernalWidth)
	{
		if (kernalWidth % 2 == 0) Console::err("kernal length must be odd");
		int nc = num_channel;
		int offset = (kernalWidth - 1) / 2;
		byte* buff = (byte*)malloc(buffer_size * sizeof(byte));
		memcpy(buff, buffer, buffer_size * sizeof(byte));
		for (int y = 0; y < height - kernalWidth; y++)
			for (int x = 0; x < width - kernalWidth; x++)
			{
				for (int n = 0; n < nc; n++) {

					float total = 0.0f;
					for (int fy = 0; fy < kernalWidth; fy++)
						for (int fx = 0; fx < kernalWidth; fx++)
							total += buffer[((y + fy) * width + x + fx) * nc + n] * kernal[fy * 3 + fx];

					byte pixVal = 0;
					if (total < 0.0f) {
						pixVal = 0;
					}
					else if (total > 255.0f) {
						pixVal = 255;
					}
					else {
						pixVal = total;
					}
					buff[((y + offset) * width + x + offset) * nc + n] = pixVal;
				}
			}

		delete buffer;
		buffer = buff;
	}

	void distrans1d(ImageData &img) {
		byte* buff = (byte*)malloc(img.buffer_size * sizeof(byte));
		int n = img.buffer_size;

		int k = 0;
		int * v = new int[n];
		float* z = new float[n + 1];

		v[0] = 0;
		z[0] = -INFINITY;
		z[1] = INFINITY;

		double s;

		for (int q = 1; q < n; q++)
		{
			while (true)
			{
				s = (((img.buffer[q] + q * q) - (img.buffer[v[k]] + v[k] * v[k])) / (2.0 * q - 2.0 * v[k]));

				if (s <= z[k])
				{
					k--;
				}
				else
				{
					break;
				}
			}

			k++;

			v[k] = q;
			z[k] = s;
			z[k + 1] = INFINITY;
		}

		k = 0;

		for (int q = 0; q < n; q++)
		{
			while (z[k + 1] < q)
			{
				k++;
			}

			buff[q] = ((q - v[k]) * (q - v[k]) + img.buffer[v[k]]);
		}
		delete img.buffer;
		img.buffer = buff;
	}

	void ImageData::appDistanceTransform1D()
	{
		distrans1d(*this);
		rotateRight();
		distrans1d(*this);
		rotateRight();
		rotateRight();
		rotateRight();
		//hue adjustment
		std::vector<byte> maxVals = {};
		std::vector<byte> minVals = {};
		maxVals.resize(num_channel, 0);
		minVals.resize(num_channel, 255);
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				for (int i = 0; i < num_channel; i++) {
					int cx = (int)buffer[(y * width + x) * num_channel + i];
					if (cx >= 255) cx = 255;
					buffer[(y * width + x) * num_channel + i] = (byte)cx;
					if (cx > maxVals[i]) maxVals[i] = cx;
					if (cx < minVals[i]) minVals[i] = cx;
				}
			}
		}
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				for (int i = 0; i < num_channel; i++) {
					int cx = (int)buffer[(y * width + x) * num_channel + i];
					float span = 255.0f / float(maxVals[i] - minVals[i]);
					cx = int(cx * span);
					buffer[(y * width + x) * num_channel + i] = (byte)cx;
				}
			}
		}
	}

	void ImageData::rotateRight() // subject to be modified
	{

		int h = height;
		int w = width;

		byte* buff = (byte*)malloc(buffer_size * sizeof(byte));
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				for( int i = 0; i < num_channel; i++)
				buff[(x * h + h - y - 1) * num_channel + i] = buffer[(y * w + x) *num_channel + i];
			}
		}
		height = w;
		width = h;
		delete buffer;
		buffer = buff;
	}

	void ImageData::rotateLeft()
	{
		int h = height;
		int w = width;

		byte* buff = (byte*)malloc(buffer_size * sizeof(byte));
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				for (int i = 0; i < num_channel; i++)
					buff[(y * w + x) * num_channel + i] = buffer[(x * h + h - y - 1) * num_channel + i];
			}
		}
		height = w;
		width = h;
		delete buffer;
		buffer = buff;
	}
	ImageData* ImageData::genImage(std::string filepath)
	{
		ImageData* img = nullptr;
		int width, height, nrChannels;
		byte* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 4);
		Console::real((float)nrChannels);
		if (data)
		{
			img = new ImageData(data, width, height, 4);
			stbi_image_free(data);
			return img;
		}
		else
		{
			Console::err("failed to load image for Image", filepath);
			return nullptr;
		}
		return nullptr;
	}

	std::vector<ImageData*> ImageData::genImageVector(std::string filepath, int num_colums, int num_rows)
	{
		std::vector<ImageData*> images = {};
		int width, height, nr;
		byte* data = stbi_load(filepath.c_str(), &width, &height, &nr, 4);
		int dw = width / num_colums;
		int dh = height / num_rows;
		if (data)
		{
			std::vector<byte*>buffers = {};
			for (int y = 0; y < num_rows; y++)
			{
				for (int x = 0; x < num_colums; x++)
				{
					byte* buffer = (byte*)malloc(nr * dw * dh * sizeof(byte));
					memset(buffer, 0, nr * dw * dh * sizeof(byte));
					Texture::fillArray2d(buffer, data, dw, width, 0, 0, dw, dh, nr, dw * x, dh * y);
					ImageData* img = new ImageData(buffer, dw, dh, nr);
					images.push_back(img);
					delete buffer;
				}
			}
			stbi_image_free(data);
			return images;
		}
		else
		{
			Console::err("failed to load image for Image", filepath);
			return {};
		}
	}

	void ImageData::flipImageX(ImageData* img)
	{
		byte* temp_buffer = (byte*)malloc(img->buffer_size * sizeof(byte));
		memset(temp_buffer, 0, img->buffer_size * sizeof(byte));
		uint w = img->width, h = img->height, n = img->num_channel;
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				for (int i = 0; i < n; i++)
				{
					temp_buffer[((y * w) + (w - x - 1)) * n + i] = img->buffer[((y * w) + (x)) * n + i];
				}
			}
		}
		delete img->buffer;
		img->buffer = temp_buffer;
	}

	void ImageData::flipImageY(ImageData* img)
	{
		byte* temp_buffer = (byte*)malloc(img->buffer_size * sizeof(byte));
		memset(temp_buffer, 0, img->buffer_size * sizeof(byte));
		uint w = img->width, h = img->height, n = img->num_channel;
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				for (int i = 0; i < n; i++)
				{
					temp_buffer[(((h - y - 1) * w) + x) * n + i] = img->buffer[((y * w) + (x)) * n + i];
				}
			}
		}
		delete img->buffer;
		img->buffer = temp_buffer;
	}


	void ImageData::saveImage_png(ImageData* image, std::string path)
	{
		stbi_write_png(path.c_str(), image->width, image->height, 4, image->buffer, image->width * image->num_channel);
	}


	//texture class members

	Texture::Texture(int width, int height, TextureType type)
		:_width(width),
		_height(height),
		_type(type),
		_texture_id(0),
		_slot(0)
	{

		//preparing new texture with blank image data
		glGenTextures(1, &_texture_id);
		glBindTexture(static_cast<GLenum>(_type), _texture_id);
		if (_type == TextureType::flat)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(static_cast<GLenum>(_type), 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		}
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &_texture_id);
	}

	void Texture::bind()
	{
		glBindTexture(static_cast<GLenum>(_type), _texture_id);

	}

	void Texture::bind(ShaderProgram* sp, int slot, const char* name)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(static_cast<GLenum>(_type), _texture_id);
		uint location = glGetUniformLocation(sp->get_program(), name);
		glUniform1i(location, slot);
		_slot = slot;
	}

	void Texture::unbind()
	{
		glActiveTexture(GL_TEXTURE0 + _slot);
		glBindTexture(static_cast<GLenum>(_type), 0);
	}

	std::vector<byte> Texture::getPixelVector_rgb()
	{
		if (_type == TextureType::flat)
		{
			glBindTexture(static_cast<GLenum>(_type), _texture_id);
			uint num_pixel = _width * _height * 4; //for r g b a
			byte* pixels = new byte[num_pixel];
			glGetTexImage(static_cast<GLenum>(_type), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			std::vector<byte> data(pixels, pixels + num_pixel);
			return data;
		}
		return {};
	}

	void Texture::setBufferData(void* data, int width, int height, uint storage_type, uint data_type)
	{
		if (_type == TextureType::flat)
		{
			glBindTexture(static_cast<GLenum>(_type), _texture_id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexImage2D(static_cast<GLenum>(_type), 0, storage_type, width, height, 0, storage_type, data_type, data);
			_width = width;
			_height = height;
		}
	}

	Texture* Texture::genTextureFlat(const char* filepath)
	{
		Texture* t = nullptr;
		int width, height, nrChannels;
		byte* data = stbi_load(filepath, &width, &height, &nrChannels, 4);
		if (data)
		{
			t = new Texture(width, height);
			t->bind();
			//set mipmap levels
			int highest = width;
			if (height > width) highest = height;
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			if (highest >= 1024)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
			else if (highest >= 512)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);
			else if (highest >= 256)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 2);
			else
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1);
			//load image data
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			//set texture parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			//free the allocated data
			stbi_image_free(data);
			t->unbind();
		}
		else
		{
			Console::err("failed to load image for texture", filepath);
		}
		return t;
	}

	Texture* Texture::genTextureFlat(ImageData* image)
	{
		Texture* t = nullptr;
		if (image->buffer)
		{
			t = new Texture(image->width, image->height);
			t->bind();
			//set mipmap levels
			int highest = image->width;
			if (image->height > image->width) highest = image->height;
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			if (highest >= 1024)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
			else if (highest >= 512)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);
			else if (highest >= 256)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 2);
			else
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1);
			//load image data
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->buffer);
			glGenerateMipmap(GL_TEXTURE_2D);

			//set texture parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			//free the allocated data
			t->unbind();
		}
		else
		{
			Console::err("failed to load s_image for texture.", "nullpointer value.");
		}
		return t;
	}

	///<summary>
	///face order {right, left, top, bottom, front, back}
	///</summary>
	Texture* Texture::genTextureCubemap(std::vector<const char*> filepaths)
	{
		Texture* t = nullptr;
		int width, height, nrChannels;
		byte* data;
		if (filepaths.size() != 6)
		{
			Console::err(std::string("cubemap needs 6 images but given vector has ") + std::to_string(filepaths.size()), filepaths[0]);
			return nullptr;
		}
		for (uint i = 0; i < 6; i++)
		{
			data = stbi_load(filepaths[i], &width, &height, &nrChannels, 4);
			//only for first time
			if (i == 0) {
				t = new Texture(width, height, TextureType::cubemap);
				t->bind();
			}
			if (data) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				Console::err("failed to load image for texture", filepaths[i]);
				return nullptr;
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		t->unbind();
		return t;
	}

	///<summary>
	///face order {right, left, top, bottom, front, back}
	///</summary>
	Texture* Texture::genTextureCubemap(std::vector<ImageData*> images)
	{
		Texture* t = nullptr;
		int width, height, nrChannels;
		byte* data;
		if (images.size() != 6)
		{
			Console::err(std::string("cubemap needs 6 images but given vector has ") + std::to_string(images.size()));
			return nullptr;
		}
		for (uint i = 0; i < 6; i++)
		{
			width = images[i]->width;
			height = images[i]->height;
			nrChannels = images[i]->num_channel;
			data = images[i]->buffer;
			//only for first time
			if (i == 0) {
				t = new Texture(width, height, TextureType::cubemap);
				t->bind();
			}
			if (data) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				Console::err("failed to load image for texture", "Image::buffer is nullptr");
				return nullptr;
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		t->unbind();
		return t;
	}


	Texture* Texture::genTextureCubemap(std::string filepath)
	{
		std::vector<ImageData*> images = ImageData::genImageVector(filepath, 4, 3);
		std::vector<ImageData*> faces = {};
		//face order {right, left, top, bottom, front, back}
		faces.push_back(images[6]);
		faces.push_back(images[4]);
		faces.push_back(images[1]);
		faces.push_back(images[9]);
		faces.push_back(images[5]);
		faces.push_back(images[7]);
		Texture* t = genTextureCubemap(faces);
		faces = {};
		for (ImageData* img : images)
		{
			delete img;
		}
		return t;
	}



	// spritesheet class members 

	SpriteSheet::SpriteSheet(std::vector<ImageData*> images)
	{
		uint block_width = 0;
		uint block_height = 0;
		uint block_channels = 0;
		for (auto i : images)
		{
			if (i->width > block_width)
				block_width = i->width;
			if (i->height > block_height)
				block_height = i->height;
			if (i->num_channel > block_channels)
				block_channels = i->num_channel;
		}
		int nr = int(sqrt(images.size())) + 1;
		_cell_width = block_width;
		_cell_height = block_height;

		_sheet_width = nr * block_width;
		_sheet_height = nr * block_height;

		//create giant sheet
		byte* giant_sheet = (byte*)malloc(nr * block_width * block_channels * nr * block_height * sizeof(byte));
		memset(giant_sheet, 0, nr * block_width * block_channels * nr * block_height * sizeof(byte));

		int n_added = 0;
		while (n_added < images.size())
		{
			uint row = int(n_added / nr);
			uint col = int(n_added % nr);
			//create crop
			float x = float(col) / nr;
			float y = float(row) / nr;
			float w = float(images[n_added]->width) / (nr * block_width);
			float h = float(images[n_added]->height) / (nr * block_height);
			_crops.push_back(glm::vec4(x, y, w, h));
			//paint on the sheet
			if (images[n_added]->buffer_size != 0)
				Texture::fillArray2d<byte>(giant_sheet, images[n_added]->buffer,
					nr * block_width, images[n_added]->width * images[n_added]->num_channel,
					col * block_width, row * block_height, images[n_added]->width, images[n_added]->height, block_channels);
			n_added++;
		}
		_texture = new Texture(nr * block_width, nr * block_height);
		_texture->setBufferData(giant_sheet, nr * block_width, nr * block_height, GL_RED, GL_UNSIGNED_BYTE);

		//delete giant sheet
		delete giant_sheet;
	}
	SpriteSheet::SpriteSheet(ImageData * image, std::vector<glm::vec4> crops)
		:_sheet_width(image->width),
		_sheet_height(image->height),
		_cell_width(0),
		_cell_height(0)
	{
		_texture = Texture::genTextureFlat(image);
		_crops = crops;


	}
	SpriteSheet::SpriteSheet(ImageData* image, float cellWidth, float cellHeight)
		:_sheet_width(image->width),
		_sheet_height(image->height),
		_cell_width(cellWidth * image->width),
		_cell_height(cellHeight * image->height)
	{
		uint numRow = (uint)( 1.0f / cellHeight);
		uint numCol = (uint)(1.0f / cellWidth);
		for (int y = 0; y < numRow; y++) {
			for (int x = 0; x < numCol; x++) {

				_crops.push_back(glm::vec4(x * cellWidth, y *cellHeight, cellWidth, cellHeight));
			}
		}
		_texture = Texture::genTextureFlat(image);
	}

	SpriteSheet::~SpriteSheet()
	{
		delete _texture;
	}

	

	
};