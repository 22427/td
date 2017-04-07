#pragma once

#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <cstring>
namespace td {

/**
 * @brief The DType enum represents the supported Data types.
 * (Based on the OpenGLES 2.0 spec)
 */
enum class DType : uint32_t
{
	UNSIGNED_SHORT_5_6_5	= 0x8363,
	UNSIGNED_SHORT_4_4_4_4	= 0x8033,
	UNSIGNED_SHORT_5_5_5_1	= 0x8034,
	UNSIGNED_BYTE			= 0x1401,
};


/**
 * @brief The Format enum represents the supported formats.
 * (Based on the OpenGLES 2.0 spec)
 */
enum class Format : uint32_t
{
	ALPHA				=0x1906,
	LUMINANCE			=0x1909,
	LUMINANCE_ALPHA		=0x190A,
	RGB					=0x1907,
	RGBA				=0x1908,
};


/**
 * @brief size_per_pixel gives the number of bytes per pixel for a given
 * format/type combination.
 * @param f - the format.
 * @param t - the type.
 * @return
 */
inline uint32_t size_per_pixel(const Format f, const DType t)
{

	if(t != DType::UNSIGNED_BYTE)
	{
		return 2;
	}
	else
	{
		if(f==Format::LUMINANCE_ALPHA)
			return 2;
		else if(f==Format::RGB)
			return 3;
		else if(f==Format::RGBA)
			return 4;
		else
			return 1;
	}
}


/**
 * @brief The TextureLayer class a texture layer is an actual 2D-bitmap storing
 * widthxheight pixels of a given format in a given type. The lvl represents an
 * aribrary integer which can be used to store the mipmaplevel of this layer.
 */
class TextureLayer
{
public:
	int32_t lvl; // level of this layer
	int32_t w;	 // width
	int32_t h;   // height
	Format frmt; // format
	DType type;  // type
	void* data;  // the actual image data

	TextureLayer(int lvl=0, int w=0, int h=0,
				 Format f=Format::RGBA, DType t = DType::UNSIGNED_BYTE)
		:lvl(lvl),w(w),h(h),frmt(f),type(t),data(nullptr)
	{
	}

	TextureLayer(const TextureLayer& o)
		:lvl(o.lvl),w(o.w),h(o.h),frmt(o.frmt),type(o.type),data(nullptr)
	{
		data=malloc(w*h*size_per_pixel(frmt,type));
		memcpy(data,o.data,w*h*size_per_pixel(frmt,type));
	}
	~TextureLayer()
	{
		if(data)
		free(data);
	}

	void write(std::ostream& f) const
	{
		f.write(reinterpret_cast<const char*>(&lvl),sizeof(lvl));
		f.write(reinterpret_cast<const char*>(&w),sizeof(w));
		f.write(reinterpret_cast<const char*>(&h),sizeof(h));
		f.write(reinterpret_cast<const char*>(&frmt),sizeof(frmt));
		f.write(reinterpret_cast<const char*>(&type),sizeof(type));
		f.write(reinterpret_cast<const char*>(data),w*h*size_per_pixel(frmt,type));
	}
	void read(std::istream& f)
	{
		f.read(reinterpret_cast<char*>(&lvl),sizeof(lvl));
		f.read(reinterpret_cast<char*>(&w),sizeof(w));
		f.read(reinterpret_cast<char*>(&h),sizeof(h));
		f.read(reinterpret_cast<char*>(&frmt),sizeof(frmt));
		f.read(reinterpret_cast<char*>(&type),sizeof(type));
		data = realloc(data,w*h*size_per_pixel(frmt,type));
		f.read(reinterpret_cast<char*>(data),w*h*size_per_pixel(frmt,type));
	}
};

/**
 * @brief The TextureData class Texture data is effectively a number of
 * TextureLayers.
 */
class TextureData
{
public:
	std::vector<TextureLayer> layers;

	void write(std::ostream& f) const
	{
		uint32_t n_layers = layers.size();
		f.write(reinterpret_cast<const char*>(&n_layers),sizeof(n_layers));
		for(const auto& l : layers)
			l.write(f);
	}


	void read(std::istream& f)
	{
		uint32_t n_layers = 0;
		f.read(reinterpret_cast<char*>(&n_layers),sizeof(n_layers));
		layers.resize(n_layers);
		for(auto& l : layers)
			l.read(f);
	}

	void write(const std::string& path)
	{
		std::ofstream f(path,std::ios::binary);
		if(f.is_open())
		{
			write(f);
			f.close();
		}
	}


	void read(const std::string& path)
	{
		std::ifstream f(path,std::ios::binary);
		if(f.is_open())
		{
			read(f);
			f.close();
		}
	}

	auto begin() -> decltype(layers.begin()){return layers.begin();}
	auto end() -> decltype(layers.end()){return layers.end();}


};


inline std::ostream& operator<<(std::ostream& f, const TextureData& td)
{
	td.write(f);
	return f;
}

inline std::istream& operator>>(std::istream& f, TextureData& td)
{
	td.read(f);
	return f;
}
}
