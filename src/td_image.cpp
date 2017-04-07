#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

#include "td_image.h"

namespace td
{
Image::~Image()
{
	if(data)
	{
		free(data);
	}
}

void Image::write(const std::string &path)
{
	std::string ending = path.substr(path.find_last_of('.')+1);
	if(ending == "png")
		write_png(path);
	else if(ending == "tga")
		write_tga(path);
	else if(ending == "bmp")
		write_bmp(path);
}

void Image::write_png(const std::string &path)
{
	stbi_write_png(path.c_str(),w,h,d,data,0);
}

void Image::write_bmp(const std::string &path)
{
	stbi_write_bmp(path.c_str(),w,h,d,data);
}

void Image::write_tga(const std::string &path)
{
	stbi_write_tga(path.c_str(),w,h,d,data);

}

Image::Image(const std::string &path)
{
	data = stbi_load(path.c_str(),&w,&h,&d,0);
}

FloatImage::FloatImage():data(nullptr),w(0),h(0){}

FloatImage::~FloatImage()
{
	if(data)
		free(data);
}

void FloatImage::from_image(const Image &img)
{
	w=img.w;
	h=img.h;

	data=(float*)realloc(data,w*h*4*sizeof(float));
	float s = 1.0f/255.0f;
	for(int y = 0 ; y < h;y++)
		for(int x = 0 ; x<w;x++)
		{
			float* f = at(x,y);
			if(img.d == 1)
			{
				f[0] = f[1] = f[2] = s*img(x,y,0);
				f[3] = 1.0f;
			}
			else if(img.d == 2)
			{
				f[0] = f[1] = f[2] = s*img(x,y,0);
				f[3] = img(x,y,1);
			}
			else
			{
				f[3] = 1.0f;
				for(int c= 0 ; c< img.d;c++)
					f[c] =s*img(x,y,c);
			}
		}
}


void FloatImage::to_image(Image &i)
{
	const auto e = elems();
	i.data=(unsigned char*)realloc(i.data,e);
	i.w=w;
	i.h=h;
	i.d=4;

	for(int j = 0 ; j<e;j++)
	{
		i.data[j] = data[j]*255.0f + 0.5f;
	}
}

void pack_ub(void *dst,const float *src, Format f)
{
	unsigned char* c = (unsigned char*) dst;
	if(f == Format::ALPHA)
	{
		c[0] = src[3] * 255.0f+0.5f;
	}
	else if(f == Format::LUMINANCE)
	{
		c[0] = (0.2126f*src[0] + 0.7152f*src[1] + 0.0722f*src[2])*255.0f + 0.5f;
	}
	else if(f == Format::LUMINANCE_ALPHA)
	{
		c[0] = (0.2126f*src[0] + 0.7152f*src[1] + 0.0722f*src[2])*255.0f + 0.5f;
		c[1] = src[3]*255.0f+0.5f;
	}
	else if(f == Format::RGB)
	{
		for(int i = 0 ; i<3;i++)
		{
			c[i] = src[i]*255.0f+0.5f;
		}
	}
	else if(f == Format::RGBA)
	{
		for(int i = 0 ; i<4;i++)
		{
			c[i] = src[i]*255.0f+0.5f;
		}
	}
}


void unpack_ub(float *dst,const void *src, Format f)
{
	unsigned char* c = (unsigned char*) src;
	float s = 1.0f/255.0f;
	if(f == Format::ALPHA)
	{
		dst[0]=dst[1]=dst[2] = 0.0f;
		dst[3] = s*c[0];
	}
	else if(f == Format::LUMINANCE)
	{
		dst[0]=dst[1]=dst[2] = s*c[0];
		dst[3] = 1.0f;
	}
	else if(f == Format::LUMINANCE_ALPHA)
	{
		dst[0]=dst[1]=dst[2] = s*c[0];
		dst[3] = s*c[1];
	}
	else if(f == Format::RGB)
	{
		for(int i = 0 ; i<3;i++)
		{
			dst[i] = s*c[i];
		}
		dst[3] = 1.0f;
	}
	else if(f == Format::RGBA)
	{
		for(int i = 0 ; i<4;i++)
		{
			dst[i] = s*c[i];
		}
	}
}


void unpack_us_5_6_5(float *dst, const void *src)
{
	const uint16_t& c = *static_cast<const uint16_t*>(src);

	dst[0] = 1.0f/31.0f*((c>>11)&((1<<5)-1));
	dst[1] = 1.0f/63.0f*((c>>5 )&((1<<6)-1));
	dst[2] = 1.0f/31.0f*((c>>0 )&((1<<5)-1));
	dst[3] = 1.0f;
}


void unpack_us_4_4_4_4(float *dst, const void *src)
{
	const uint16_t& c = *static_cast<const uint16_t*>(src);

	dst[0] = 1.0f/15.0f*((c>>12)&((1<<4)-1));
	dst[1] = 1.0f/15.0f*((c>>8 )&((1<<4)-1));
	dst[2] = 1.0f/15.0f*((c>>4 )&((1<<4)-1));
	dst[3] = 1.0f/15.0f*((c>>0 )&((1<<4)-1));
}


void unpack_us_5_5_5_1(float *dst, const void *src)
{
	const uint16_t& c = *static_cast<const uint16_t*>(src);
	dst[0] = 1.0f/31.0f*(float)((c>>11)&((1<<5)-1));
	dst[1] = 1.0f/31.0f*(float)((c>>6 )&((1<<5)-1));
	dst[2] = 1.0f/31.0f*(float)((c>>1 )&((1<<5)-1));
	dst[3] = 1.0f/ 1.0f*(float)((c>>0 )&((1<<2)-1));
}



void FloatImage::from_texture_layer(const TextureLayer &tl)
{
	w = tl.w;
	h = tl.h;
	data = (float*) realloc(data,w*h*4*sizeof(float));


	uint32_t spp = size_per_pixel(tl.frmt,tl.type);
	float* op = data;
	const uint8_t* ip = (const uint8_t*)tl.data;
	for(int p = 0 ; p <w*h;p++)
	{
		if(tl.type == DType::UNSIGNED_SHORT_4_4_4_4)
			unpack_us_4_4_4_4(op,ip);
		else if (tl.type == DType::UNSIGNED_SHORT_5_5_5_1)
			unpack_us_5_5_5_1(op,ip);
		else if (tl.type == DType::UNSIGNED_SHORT_5_6_5)
			unpack_us_5_6_5(op,ip);
		else
			unpack_ub(op,ip,tl.frmt);
		op += 4;
		ip += spp;
	}
}


void pack_us_5_6_5(void *dst, const float *src)
{
	uint16_t& d = *static_cast<uint16_t*>(dst);
	d = 0;

	uint16_t steps[3]={32,64,32};
	uint16_t bits[3]={5,6,5};

	uint16_t shift = 16;
	for(int c= 0 ; c<3;c++)
	{
		shift-=bits[c];
		auto& v = src[c];
		uint16_t interval = (int)(v*(steps[c]-1))+0.5f;
		interval = interval<<shift;
		d |= interval;
	}
}

void pack_us_4_4_4_4(void *dst, const float *src)
{
	uint16_t& d = *static_cast<uint16_t*>(dst);
	d = 0;

	uint16_t steps[]={16,16,16,16};
	uint16_t bits[]={4,4,4,4};

	uint16_t shift = 16;
	for(int c= 0 ; c<4;c++)
	{
		shift-=bits[c];
		auto& v = src[c];
		uint16_t interval = (int)(v*(steps[c]-1))+0.5f;
		interval = interval<<shift;
		d |= interval;

	}
}

void pack_us_5_5_5_1(void *dst, const float *src)
{
	uint16_t& d = *static_cast<uint16_t*>(dst);
	d = 0;

	uint16_t steps[]={32,32,32,2};
	uint16_t bits[]={5,5,5,1};

	uint16_t shift = 16;
	for(int c= 0 ; c<4;c++)
	{
		shift-=bits[c];
		auto& v = src[c];
		uint16_t interval = (int)(v*(steps[c]-1))+0.5f;
		interval = interval<<shift;
		d |= interval;
	}
}

void FloatImage::to_texture_layer(TextureLayer &td, Format f, DType t)
{
		td.w = w;
		td.h = h;
		td.frmt =f;
		td.type = t;
		const uint32_t spp = size_per_pixel(td.frmt,td.type);
		td.data = realloc(td.data,w*h*spp);

		const float* ip = data;
		uint8_t* op = (uint8_t*)td.data;
		for(int p = 0 ; p <w*h;p++)
		{
			if(td.type == DType::UNSIGNED_SHORT_4_4_4_4)
				pack_us_4_4_4_4(op,ip);
			else if (td.type == DType::UNSIGNED_SHORT_5_5_5_1)
				pack_us_5_5_5_1(op,ip);
			else if (td.type == DType::UNSIGNED_SHORT_5_6_5)
				pack_us_5_6_5(op,ip);
			else
				pack_ub(op,ip,td.frmt);
			op += spp;
			ip += 4;
		}
	}


void FloatImage::dither_floyd_steinberg(int *steps)
{
	float step_size[4];
	for(int i = 0 ; i<4;i++)
	{
		step_size[i] = 1.0f/(steps[i]-1);
	}

	for(int y = 0; y<h;y++)
		for(int x = 0; x<w;x++)
		{
			for(int c= 0 ; c<4;c++)
			{
				const auto& v = at(x,y,c);
				uint32_t interval = (int)(v*(steps[c]-1))+0.5f;
				float qe = v-interval*step_size[c];
				if(x<w-1)
					at(x+1,y  ,c) += qe * 7.0f / 16.0f;
				if(x<w-1 && y<h-1 )
					at(x+1,y+1,c) += qe * 1.0f / 16.0f;
				if(x>0 && y<h-1 )
					at(x-1,y+1,c) += qe * 3.0f / 16.0f;
				if(y<h-1 )
					at(x  ,y+1,c) += qe * 5.0f / 16.0f;
			}
		}

}

void FloatImage::quantize(int *steps)
{
	float step_size[4];
	for(int i = 0 ; i<4;i++)
	{
		step_size[i] = 1.0f/(steps[i]-1);
	}

	for(int y = 0; y<h;y++)
		for(int x = 0; x<w;x++)
		{
			for(int c= 0 ; c<4;c++)
			{
				auto& v = at(x,y,c);
				int interval = (int)(v*(steps[c]-1)+0.5f);
				interval = std::min(interval,steps[c]-1);
				v = interval*step_size[c];
			}
		}
}


float *FloatImage::operator()(int x, int y)
{
	return data+ (y*w+x)*4;
}

float &FloatImage::operator()(int x, int y, int c)
{
	return data[(y*w+x)*4+c];
}

float *FloatImage::at(int x, int y)
{
	return data+ (y*w+x)*4;
}

float &FloatImage::at(int x, int y, int c)
{
	return data[(y*w+x)*4+c];
}

int FloatImage::elems() const {return w*h*4;}

std::vector<FloatImage> generate_mip_maps(const FloatImage& img)
{
	std::vector<FloatImage> res;

	// generate a linear version of the image
	float* lin_img = (float*)malloc(img.w*img.h*4*sizeof(float));
	for(int i = 0; i< img.elems();i++)
	{
		lin_img[i] = pow((float)img.data[i],2.2f);
	}

	int curr_w = img.w*2;
	int curr_h = img.h*2;

	float* working_image=(float*)malloc((curr_w/2)*(curr_h/2)*4*sizeof(float));

	do
	{

		curr_w = curr_w/2;
		curr_h = curr_h/2;

		stbir_resize(lin_img,img.w,img.h,0,working_image,curr_w,curr_h,0,
					 STBIR_TYPE_FLOAT,4,3,
					 STBIR_FLAG_ALPHA_USES_COLORSPACE,//flags
					 STBIR_EDGE_CLAMP,//edgemoce hor
					 STBIR_EDGE_CLAMP,//edfemode vert
					 STBIR_FILTER_TRIANGLE ,//fileter hor
					 STBIR_FILTER_TRIANGLE ,//filter vert
					 STBIR_COLORSPACE_LINEAR,//colorspace
					 nullptr//alloc context
					 );


		FloatImage mms;
		res.push_back(mms);
		res.back().w = curr_w;
		res.back().h = curr_h;
		res.back().data = (float*) malloc(curr_w*curr_h*sizeof(float)*4);
		for(int i = 0; i< curr_w*curr_h*4;i++)
		{
			res.back().data[i] =pow(working_image[i],1.0f/2.2f);
		}

	}
	while (curr_w != 1 || curr_h !=1);

	free(working_image);
	return res;
}

}
