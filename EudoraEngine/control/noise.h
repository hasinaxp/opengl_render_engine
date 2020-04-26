#pragma once
#include "../api.h"
#include <vector>

namespace sp {

	class SP_API Noise {
	private:
		uint _seed;
	public:
		Noise(uint seed);
		~Noise();

		float genRandom();
		std::vector<float> genRandomArr(int count);
		std::vector<float> genPerlin1D(int count, int octaves, float bias, uint seed = 0);
		std::vector<float> genPerlin2D(int width, int height, int octaves, float bias,  uint seed = 0);


		void setSeed(uint seed) { _seed = seed; }
	};



};