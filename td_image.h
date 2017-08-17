#pragma once
#include <string>
#include <vector>
#include <cstring>
#include "td.h"
namespace td {

/**
 * @brief The Image class is used for basic image IO using common formats.
 * This is done using stb_image so the supported formats are:
 *
 * |Format	|r|w|
 * +--------+-+-+
 * |PNG		|x|x|
 * |JPEG	|x| |
 * |BMP		|x|x|
 * |TGA		|x|x|
 */
class Image
{

public:

	unsigned char* data;
	int w;
	int h;
	int d;

	Image():data(nullptr),w(0),h(0),d(0){}
	/**
	 * @brief Note: ~Image() will free data if data!=nullptr !
	 */
	~Image();

	Image(const Image& o):data(nullptr),w(o.w),h(o.h),d(o.d)
	{
		data = (unsigned char*)realloc(data,w*h*d);
		memcpy(data,o.data,w*h*d);
	}

	unsigned char* operator()(int x, int y)
	{
		return data+ (y*w+x)*d;
	}

	unsigned char& operator()(int x, int y,int c)
	{
		return data[(y*w+x)*d+c];
	}

	unsigned char operator()(int x, int y,int c) const
	{
		return data[(y*w+x)*d+c];
	}

	void write(const std::string& path);
	void write_png(const std::string& path);
	void write_bmp(const std::string& path);
	void write_tga(const std::string& path);

	Image(const std::string& path);
	int elems() const {return w*h*d;}

};

/**
 * @brief The FloatImage class is used for processing the image. In order
 * to simplify the structure, a FloatImage always has 4 channels. Data is stored
 * in floats per image, to avoid any loss in quality.
 */
class FloatImage
{

public:

	float* data;
	int w;
	int h;

	FloatImage();
	/**
	 * @brief Note: ~FloatImage() will free data if data!=nullptr !
	 */
	~FloatImage();

	FloatImage(const FloatImage& o):data(nullptr),w(o.w),h(o.h)
	{
		data = (float*)realloc(data,w*h*4*sizeof(float));
		memcpy(data,o.data,w*h*4*sizeof(float));
	}

	/**
	 * @brief from_image reads data from an Image normalizing the color data
	 * from [0,255] to [0,1].
	 * @param img
	 */
	void from_image(const Image& img);

	/**
	 * @brief from_texture_layer reads data from a TextureLayer normalizing
	 * the color data to [0,1].
	 * @param img
	 */
	void from_texture_layer(const TextureLayer& tl);

	/**
	 * @brief to_image converts the Image to a normal Image converting the data.
	 * from [0,1] to [0,255]
	 * @param i
	 */
	void to_image(Image& i);

	/**
	 * @brief to_texture_layer converts the Image to a TextureLayer - quantizing
	 * and packing the data according to the given type and format.
	 * @param td - target Texture layser
	 * @param f  - target Format
	 * @param t  - target Type
	 */
	void to_texture_layer(TextureLayer& td, Format f, DType t);

	/**
	 * @brief dither_floyd_steinberg applies the Floyd-Steinberg-Dithering for
	 * a hyperthetical quantization of setps[0-3] steps.
	 * Note: There is no quantization, only a dithering for the given
	 * quantiuation is applied.
	 * @param steps - an array of 4 integers refering to the steps per channel.
	 */
	void dither_floyd_steinberg(int* steps);

	/**
	 * @brief quantize Applies a quantization in [0,1] for steps[0-3] steps.
	 * @param steps - an array of 4 integers refering to the steps per channel.
	 */
	void quantize(int* steps);

	/**
	 * @brief operator () access operator for a given pixel (x,y).
	 * @param x
	 * @param y
	 * @return
	 */
	float* operator()(int x, int y);

	/**
	 * @brief operator () access operator for a given channel (c) in a given
	 * pixel (x,y).
	 * @param x
	 * @param y
	 * @param c
	 * @return
	 */
	float& operator()(int x, int y,int c);
	float* at(int x, int y);
	float& at(int x, int y,int c);

	/**
	 * @brief elems returns the number of elements (width*height*channels)
	 * stored in thins image.
	 * @return w*h*4
	 */
	int elems() const;
};

/**
 * @brief generate_mip_maps generate all mip-map-levels for img (including lvl 0!)
 * using stb_image_resize.
 * @param img
 * @return
 */
std::vector<FloatImage> generate_mip_maps(const FloatImage &img);
}
