#include "stdafx.h"

#include <cstdint>
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>

#include "IUnityRenderingExtensions.h"

#define TAG "ASCIIART"
#include "Log.hpp"

namespace __asciiart_detail_
{
	// Old school plasma effect
	uint32_t Plasma(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t frame)
	{
		auto px = (float)x / width;
		auto py = (float)y / height;
		auto time = frame / 60.0f;

		auto l = std::sinf(px * std::sinf(time * 1.3f) + std::sinf(py * 4 + time) * std::sinf(time));

		auto r = (uint32_t)(std::sinf(l * 6) * 127 + 127);
		auto g = (uint32_t)(std::sinf(l * 7) * 127 + 127);
		auto b = (uint32_t)(std::sinf(l * 10) * 127 + 127);

		return r + (g << 8) + (b << 16) + 0xff000000;
	}

	class PerlinNoise {
		// The permutation vector
		std::vector<int> p;
	public:
		// Initialize with the reference values for the permutation vector
		PerlinNoise();
		// Generate a new permutation vector based on the value of seed
		PerlinNoise(unsigned int seed);
		// Get a noise value, for 2D images z can have any value
		double noise(double x, double y, double z);
	private:
		double fade(double t);
		double lerp(double t, double a, double b);
		double grad(int hash, double x, double y, double z);
	};

	// Initialize with the reference values for the permutation vector
	PerlinNoise::PerlinNoise() {

		// Initialize the permutation vector with the reference values
		p = {
			151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
			8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
			35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,
			134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
			55,46,245,40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,187,208, 89,
			18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,
			250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
			189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167,
			43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,
			97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,
			107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
			138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };
		// Duplicate the permutation vector
		p.insert(p.end(), p.begin(), p.end());
	}

	// Generate a new permutation vector based on the value of seed
	PerlinNoise::PerlinNoise(unsigned int seed) {
		p.resize(256);

		// Fill p with values from 0 to 255
		std::iota(p.begin(), p.end(), 0);

		// Initialize a random engine with seed
		std::default_random_engine engine(seed);

		// Suffle  using the above random engine
		std::shuffle(p.begin(), p.end(), engine);

		// Duplicate the permutation vector
		p.insert(p.end(), p.begin(), p.end());
	}

	double PerlinNoise::noise(double x, double y, double z) {
		// Find the unit cube that contains the point
		int X = (int)floor(x) & 255;
		int Y = (int)floor(y) & 255;
		int Z = (int)floor(z) & 255;

		// Find relative x, y,z of point in cube
		x -= floor(x);
		y -= floor(y);
		z -= floor(z);

		// Compute fade curves for each of x, y, z
		double u = fade(x);
		double v = fade(y);
		double w = fade(z);

		// Hash coordinates of the 8 cube corners
		int A = p[X] + Y;
		int AA = p[A] + Z;
		int AB = p[A + 1] + Z;
		int B = p[X + 1] + Y;
		int BA = p[B] + Z;
		int BB = p[B + 1] + Z;

		// Add blended results from 8 corners of cube
		double res = lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z), grad(p[BA], x - 1, y, z)), lerp(u, grad(p[AB], x, y - 1, z), grad(p[BB], x - 1, y - 1, z))), lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1), grad(p[BA + 1], x - 1, y, z - 1)), lerp(u, grad(p[AB + 1], x, y - 1, z - 1), grad(p[BB + 1], x - 1, y - 1, z - 1))));
		return (res + 1.0) / 2.0;
	}

	double PerlinNoise::fade(double t) {
		return t * t * t * (t * (t * 6 - 15) + 10);
	}

	double PerlinNoise::lerp(double t, double a, double b) {
		return a + t * (b - a);
	}

	double PerlinNoise::grad(int hash, double x, double y, double z) {
		int h = hash & 15;
		// Convert lower 4 bits of hash into 12 gradient directions
		double u = h < 8 ? x : y,
			v = h < 4 ? y : h == 12 || h == 14 ? x : z;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}

	void copy_u32(uint32_t* src, int srcStep, int width, int height, uint32_t* dst, int dstStep) {
		for (int y = 0; y < height; ++y) {
			memcpy(dst, src, width * sizeof(uint32_t));

			src = (uint32_t*)((uint8_t*)src + srcStep);
			dst = (uint32_t*)((uint8_t*)dst + dstStep);
		}
	}

	void copy_u8(uint8_t* src, int srcStep, int width, int height, uint8_t* dst, int dstStep) {
		for (int y = 0; y < height; ++y) {
			memcpy(dst, src, width);

			src += srcStep;
			dst += dstStep;
		}
	}

	struct PerlinNoiseTexture {
		int mIndex;

		uint32_t* mImage; // BGRA
		int mImageStep;
		int mWidth;
		int mHeight;

		enum {
			STATE_READY,
			STATE_UPDATING,
		} mState;

		PerlinNoise* mNoise;
		double mScale;
		double mZ;

		void Init() {
			mImage = NULL;
			mImageStep = 0;
			mWidth = 0;
			mHeight = 0;
			mState = STATE_READY;

			mNoise = NULL;
			mScale = 1;
			mZ = 0.6;
		}

		void Uninit() {
			if (mState != STATE_READY) {
				LOGE("%s(%d): unexpected value, mState = %d", __FUNCTION__, __LINE__, mState);
			}

			if (mImage) {
				delete[] mImage;
			}

			if (mNoise) {
				delete mNoise;
			}
		}

		void SetZ(double z) {
			mZ = z;
		}
		void SetScale(double scale) {
			mScale = scale;
		}

		void SetSeed(uint32_t seed) {
			if (mNoise) delete mNoise;

			mNoise = new PerlinNoise(seed);
		}

		void Begin(int width, int height, void** texData) {
			if (mState != STATE_READY) {
				LOGE("%s(%d): unexpected value, mState = %d", __FUNCTION__, __LINE__, mState);
				return;
			}

			if (mImage) {
				// reallocate memory if size is different
				if (mWidth != width || mHeight != height) {
					delete[] mImage;

					mWidth = width;
					mHeight = height;
					mImage = new uint32_t[width * height];
					mImageStep = width * sizeof(uint32_t);
				}
			}
			else {
				mWidth = width;
				mHeight = height;
				mImage = new uint32_t[width * height];
				mImageStep = width * sizeof(uint32_t);
			}

			for (auto y = 0u; y < height; y++) {
				for (auto x = 0u; x < width; x++) {
					uint32_t& pix = mImage[y * width + x];

					double dX = (double)x * mScale / (double)width;
					double dY = (double)y * mScale / (double)height;

					double n = mNoise->noise(dX, dY, mZ);

					int gray = (int)floor(n * 255);
					pix = (gray + (gray << 8) + (gray << 16) + 0xff000000);
				}
			}

			mState = STATE_UPDATING;
			*texData = mImage;
		}

		void End() {
			if (mState != STATE_UPDATING) {
				LOGE("%s(%d): unexpected value, mState = %d", __FUNCTION__, __LINE__, mState);
				return;
			}

			mState = STATE_READY;
		}

		static void TextureUpdateCallbackV2(int eventID, void* data);
	};

	struct DigitTexture {
		int mIndex;

		uint32_t* mImage; // BGRA
		int mImageStep;
		int mWidth;
		int mHeight;

		enum {
			STATE_READY,
			STATE_UPDATING,
		} mState;

		uint32_t* mCharsImage;
		int mCharsImageStep;
		int mCharsImageWidth;
		int mCharsImageHeight;
		int mCharsCount;
		int mCharWidth;
		int mCharHeight;

		void Init() {
			mImage = NULL;
			mImageStep = 0;
			mWidth = 0;
			mHeight = 0;
			mState = STATE_READY;

			mCharsImage = NULL;
			mCharsImageStep = 0;
			mCharsImageWidth = 0;
			mCharsImageHeight = 0;
			mCharsCount = 0;
			mCharWidth = 0;
			mCharHeight = 0;
		}

		void Uninit() {
			if (mState != STATE_READY) {
				LOGE("%s(%d): unexpected value, mState = %d", __FUNCTION__, __LINE__, mState);
			}

			if (mImage) {
				delete[] mImage;
			}
		}

		void SetCharsImage(uint32_t* ptr, int width, int height, int charWidth) {
			mCharsImage = ptr;
			mCharsImageStep = width * sizeof(uint32_t);
			mCharsImageWidth = width;
			mCharsImageHeight = height;
			mCharWidth = charWidth;
			mCharHeight = height;
			mCharsCount = width / charWidth;
		}

		void Begin(int width, int height, void** texData);

		void End() {
			if (mState != STATE_UPDATING) {
				LOGE("%s(%d): unexpected value, mState = %d", __FUNCTION__, __LINE__, mState);
				return;
			}

			mState = STATE_READY;
		}

		static void TextureUpdateCallbackV2(int eventID, void* data);
	};

	PerlinNoiseTexture g_PerlinNoiseTextures[8];
	DigitTexture g_DigitTextures[8];

	void PerlinNoiseTexture::TextureUpdateCallbackV2(int eventID, void* data) {
		auto nEvent = static_cast<UnityRenderingExtEventType>(eventID);

		switch (nEvent) {
		case kUnityRenderingExtEventUpdateTextureBegin:
		{
			auto pData = reinterpret_cast<UnityRenderingExtTextureUpdateParams*>(data);
			auto gid = pData->userData;

			g_PerlinNoiseTextures[gid % _countof(g_PerlinNoiseTextures)].Begin(pData->width, pData->height, &pData->texData);
		}
		break;

		case kUnityRenderingExtEventUpdateTextureEnd:
		{
			auto pData = reinterpret_cast<UnityRenderingExtTextureUpdateParams*>(data);
			auto gid = pData->userData;

			g_PerlinNoiseTextures[gid % _countof(g_PerlinNoiseTextures)].End();
		}
		break;

		case kUnityRenderingExtEventUpdateTextureBeginV2:
		{
			auto pData = reinterpret_cast<UnityRenderingExtTextureUpdateParamsV2*>(data);
			auto gid = pData->userData;

			g_PerlinNoiseTextures[gid % _countof(g_PerlinNoiseTextures)].Begin(pData->width, pData->height, &pData->texData);
		}
		break;

		case kUnityRenderingExtEventUpdateTextureEndV2:
		{
			auto pData = reinterpret_cast<UnityRenderingExtTextureUpdateParamsV2*>(data);
			auto gid = pData->userData;

			g_PerlinNoiseTextures[gid % _countof(g_PerlinNoiseTextures)].End();
		}
		break;

		default:
			LOGE("%s(%d): unexpected value, nEvent=%d", __FUNCTION__, __LINE__, nEvent);
			break;
		}
	}

	void DigitTexture::TextureUpdateCallbackV2(int eventID, void* data) {
		auto nEvent = static_cast<UnityRenderingExtEventType>(eventID);

		switch (nEvent) {
		case kUnityRenderingExtEventUpdateTextureBegin:
		{
			auto pData = reinterpret_cast<UnityRenderingExtTextureUpdateParams*>(data);
			auto gid = pData->userData;

			g_DigitTextures[gid % _countof(g_DigitTextures)].Begin(pData->width, pData->height, &pData->texData);
		}
		break;

		case kUnityRenderingExtEventUpdateTextureEnd:
		{
			auto pData = reinterpret_cast<UnityRenderingExtTextureUpdateParams*>(data);
			auto gid = pData->userData;

			g_DigitTextures[gid % _countof(g_DigitTextures)].End();
		}
		break;

		case kUnityRenderingExtEventUpdateTextureBeginV2:
		{
			auto pData = reinterpret_cast<UnityRenderingExtTextureUpdateParamsV2*>(data);
			auto gid = pData->userData;

			g_DigitTextures[gid % _countof(g_DigitTextures)].Begin(pData->width, pData->height, &pData->texData);
		}
		break;

		case kUnityRenderingExtEventUpdateTextureEndV2:
		{
			auto pData = reinterpret_cast<UnityRenderingExtTextureUpdateParamsV2*>(data);
			auto gid = pData->userData;

			g_DigitTextures[gid % _countof(g_DigitTextures)].End();
		}
		break;

		default:
			LOGE("%s(%d): unexpected value, nEvent=%d", __FUNCTION__, __LINE__, nEvent);
			break;
		}
	}

	void DigitTexture::Begin(int width, int height, void** texData) {
		if (mState != STATE_READY) {
			LOGE("%s(%d): unexpected value, mState = %d", __FUNCTION__, __LINE__, mState);
			return;
		}

		PerlinNoiseTexture& perlinNoiseTexture = g_PerlinNoiseTextures[mIndex];

		if (width != perlinNoiseTexture.mWidth * mCharWidth ||
			height != perlinNoiseTexture.mHeight * mCharHeight) {
			LOGE("%s(%d): unexpected value, width = %d, height = %d", __FUNCTION__, __LINE__, width, height);
			return;
		}

		if (mImage) {
			// reallocate memory if size is different
			if (mWidth != width || mHeight != height) {
				delete[] mImage;

				mWidth = width;
				mHeight = height;
				mImage = new uint32_t[width * height];
				mImageStep = width * sizeof(uint32_t);
			}
		}
		else {
			mWidth = width;
			mHeight = height;
			mImage = new uint32_t[width * height];
			mImageStep = width * sizeof(uint32_t);
		}

		for (auto y = 0u; y < height; y += mCharHeight) {
			for (auto x = 0u; x < width; x += mCharWidth) {
				uint32_t& pix = mImage[y * width + x];
				uint32_t& perlinNoisePix = perlinNoiseTexture.mImage[(y / mCharHeight) * (width / mCharWidth) + (x / mCharWidth)];

				int char_index = (perlinNoisePix & 0xFF) * mCharsCount / 255;
				uint32_t* pChar = (uint32_t*)((uint8_t*)mCharsImage + char_index * mCharWidth);

				copy_u32(pChar, mCharsImageStep, mCharWidth, mCharHeight, &pix, mImageStep);
			}
		}

		mState = STATE_UPDATING;
		*texData = mImage;
	}
}

using namespace __asciiart_detail_;

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API AsciiArt_Init() {
	for (int i = 0; i < _countof(g_PerlinNoiseTextures); ++i) {
		g_PerlinNoiseTextures[i].mIndex = i;
		g_PerlinNoiseTextures[i].Init();
	}

	for (int i = 0; i < _countof(g_DigitTextures); ++i) {
		g_DigitTextures[i].mIndex = i;
		g_DigitTextures[i].Init();
	}
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API AsciiArt_Uninit() {
	for (int i = 0; i < _countof(g_PerlinNoiseTextures); ++i) {
		g_PerlinNoiseTextures[i].Uninit();
	}

	for (int i = 0; i < _countof(g_DigitTextures); ++i) {
		g_DigitTextures[i].Uninit();
	}
}

extern "C" UnityRenderingEventAndData UNITY_INTERFACE_EXPORT AsciiArt_PerlinNoiseV2()
{
	return PerlinNoiseTexture::TextureUpdateCallbackV2;
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API AsciiArt_PerlinNoise_SetNoiseSeed(int gid, uint32_t seed) {
	g_PerlinNoiseTextures[gid % _countof(g_PerlinNoiseTextures)].SetSeed(seed);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API AsciiArt_PerlinNoise_SetZ(int gid, double z) {
	g_PerlinNoiseTextures[gid % _countof(g_PerlinNoiseTextures)].SetZ(z);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API AsciiArt_PerlinNoise_SetScale(int gid, double scale) {
	g_PerlinNoiseTextures[gid % _countof(g_PerlinNoiseTextures)].SetScale(scale);
}

extern "C" UnityRenderingEventAndData UNITY_INTERFACE_EXPORT AsciiArt_DigitV2()
{
	return DigitTexture::TextureUpdateCallbackV2;
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API AsciiArt_Digit_SetCharsImage(int gid, uint32_t* ptr, int width, int height, int charWidth) {
	g_DigitTextures[gid % _countof(g_DigitTextures)].SetCharsImage(ptr, width, height, charWidth);
}
