#include "noise.h"
#include <cstdlib>
#include <algorithm>


namespace sp {

Noise::Noise(uint seed)
	:_seed(seed)
{
	srand(_seed);
}

Noise::~Noise()
{
}

float Noise::genRandom()
{
	return (float)rand() / (float)RAND_MAX;
}

std::vector<float> Noise::genRandomArr(int count)
{
	srand(_seed);
	std::vector<float> output = {};
	for (register int i = 0; i < count; i++)
		output.push_back(genRandom());
	return output;
}

std::vector<float> Noise::genPerlin1D(int count, int octaves, float bias, uint seed)
{
	uint prevSeed = _seed;
	if (seed != 0) setSeed(seed);
	auto fSeed = genRandomArr(count);
	setSeed(prevSeed);
	std::vector<float> fOutput;
	fOutput.resize(count, .0f);
	for (int x = 0; x < count; x++)
	{
		float fNoise = 0.0f;
		float fScaleAcc = 0.0f;
		float fScale = 1.0f;

		for (int o = 0; o < octaves; o++)
		{
			int nPitch = count >> o;
			int nSample1 = (x / nPitch) * nPitch;
			int nSample2 = (nSample1 + nPitch) % count;

			float fBlend = (float)(x - nSample1) / (float)nPitch;

			float fSample = (1.0f - fBlend) * fSeed[nSample1] + fBlend * fSeed[nSample2];

			fScaleAcc += fScale;
			fNoise += fSample * fScale;
			fScale = fScale / bias;
		}

		// Scale to seed range
		fOutput[x] = fNoise / fScaleAcc;
	}

	return fOutput;
}

std::vector<float> Noise::genPerlin2D(int width, int height, int octaves, float bias, uint seed)
{
	uint prevSeed = _seed;
	if (seed != 0) setSeed(seed);
	auto fSeed = genRandomArr(width * height);
	setSeed(prevSeed);
	std::vector<float> fOutput;
	fOutput.resize(width * height, .0f);

	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
		{
			float fNoise = 0.0f;
			float fScaleAcc = 0.0f;
			float fScale = 1.0f;

			for (int o = 0; o < octaves; o++)
			{
				int nPitch = width >> o;
				int nSampleX1 = (x / nPitch) * nPitch;
				int nSampleY1 = (y / nPitch) * nPitch;

				int nSampleX2 = (nSampleX1 + nPitch) % width;
				int nSampleY2 = (nSampleY1 + nPitch) % width;

				float fBlendX = (float)(x - nSampleX1) / (float)nPitch;
				float fBlendY = (float)(y - nSampleY1) / (float)nPitch;

				float fSampleT = (1.0f - fBlendX) * fSeed[nSampleY1 * width + nSampleX1] + fBlendX * fSeed[nSampleY1 * width + nSampleX2];
				float fSampleB = (1.0f - fBlendX) * fSeed[nSampleY2 * width + nSampleX1] + fBlendX * fSeed[nSampleY2 * width + nSampleX2];

				fScaleAcc += fScale;
				fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;
				fScale = fScale / bias;
			}

			// Scale to seed range
			fOutput[y * width + x] = fNoise / fScaleAcc;
		}

	return fOutput;

}


}
