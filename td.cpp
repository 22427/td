

#include <cstdio>
#include <fstream>


#include "td_image.h"
#include "td.h"
#include <iostream>
using namespace td;

struct cmd_data
{
	cmd_data()
	{
		input_image = "";
		output_image = "result.td";
		output_format = Format::RGB;
		output_data_type = DType::UNSIGNED_BYTE;
		disable_dither = false;
		generate_mip_maps = false;
	}
	std::string input_image;
	std::string output_image;

	Format output_format;
	DType output_data_type;
	bool disable_dither;
	bool generate_mip_maps;
};


bool print_help(const std::string& msg="")
{
	cmd_data cd;
	if(!msg.empty())
		fprintf(stderr,"%s\n",msg.c_str());

	fprintf(stderr,"-i <f>    Set input file <f>.               | %s\n",cd.input_image.c_str());
	fprintf(stderr,"-o <f>    Set output file <f>.              | %s\n",cd.input_image.c_str());

	fprintf(stderr,"-f <frmt> Set output format to <frmt>.      | %s\n","RGB");
	fprintf(stderr,"\tOne of: ALPHA, LUMINANCE, LUMINANCE_ALPHA, RGB, RGBA\n");
	fprintf(stderr,"-dt <dt>  Set output data type to <dT>.     | %s\n","UNSIGNED_BYTE");
	fprintf(stderr,"\tOne of: UNSIGNED_BYTE, UNSIGNED_SHORT_4_4_4_4,\n\t       UNSIGNED_SHORT_5_5_5_1, UNSIGNED_SHORT_5_6_5\n");
	fprintf(stderr,"-mm       Genreate MipMaps.                 | %s\n","false");
	fprintf(stderr,"-dd       Disable dithering on quantization | %s\n","false");


	return false;
}
bool parse_cmd(int argc, char** argv, cmd_data& cd)
{
	for(int i  =1 ; i < argc;)
	{
		auto c = std::string(argv[i++]);
		if(c == "-i")
		{
			cd.input_image=argv[i++];
		}
		if(c == "-o")
		{
			cd.output_image=argv[i++];
		}
		if(c == "-mm")
		{
			cd.generate_mip_maps = true;
		}
		if(c == "-dd")
		{
			cd.disable_dither = true;
		}
		if(c == "-h")
		{
			return print_help();
		}
		if(c == "-f")
		{
#define stformat(x) if(std::string(argv[i]) == #x ) cd.output_format = Format:: x
			stformat(ALPHA);
			stformat(LUMINANCE);
			stformat(LUMINANCE_ALPHA);
			stformat(RGB);
			stformat(RGBA);
#undef stformat
			i++;
		}
		if(c == "-dt")
		{
			std::string t(argv[i]);
			if(t == "UNSIGNED_BYTE" ) cd.output_data_type = DType::UNSIGNED_BYTE;
			if(t == "UNSIGNED_SHORT_4_4_4_4" )
			{
				cd.output_data_type = DType::UNSIGNED_SHORT_4_4_4_4;
				cd.output_format = Format::RGBA;
			}
			if(t == "UNSIGNED_SHORT_5_5_5_1" )
			{
				cd.output_data_type = DType::UNSIGNED_SHORT_5_5_5_1;
				cd.output_format = Format::RGBA;
			}
			if(t == "UNSIGNED_SHORT_5_6_5" )
			{
				cd.output_data_type = DType::UNSIGNED_SHORT_5_6_5;
				cd.output_format = Format::RGB;
			}
		}
	}
	if(cd.input_image.empty())
		return print_help("You need to specify an input image");
	return true;
};


int main(int argc, char** argv)
{

	cmd_data cd;
	if(!parse_cmd(argc,argv,cd))
		return -1;

	int b[4] = {8,8,8,8};
	int steps[4];

	if(cd.output_data_type == DType::UNSIGNED_SHORT_4_4_4_4)
	{
		b[0]=b[1]=b[2]=b[3] = 4;
	}
	else if(cd.output_data_type == DType::UNSIGNED_SHORT_5_5_5_1)
	{
		b[0]=b[1]=b[2]= 5;
		b[3] = 1;
	}
	else if (cd.output_data_type == DType::UNSIGNED_SHORT_5_6_5)
	{
		b[0] = b[2] =5;
		b[1] = 6;
		b[3] = 0;
	}

	for(int i = 0 ; i< 4;i++)
	{
		steps[i] = (1<<b[i]);
	}

	TextureData td;
	FloatImage f;


	if(cd.input_image.substr(cd.input_image.find_last_of('.')+1) == "td")
	{
		Image i;
		std::string out_ending = cd.output_image.substr(cd.output_image.find_last_of('.'));
		std::string out_name = cd.output_image.substr(0,cd.output_image.find_last_of('.'));
		td.read(cd.input_image);
		int q= 0 ;
		for(const auto& tl: td.layers)
		{
			f.from_texture_layer(tl);
			f.to_image(i);
			i.write(out_name+"_"+std::to_string(q)+out_ending);
			q++;
		}
		return  0;
	}


	Image i(cd.input_image);

	f.from_image(i);

	std::vector<FloatImage> layers;
	if(cd.generate_mip_maps)
		layers = generate_mip_maps(f);
	else
		layers.push_back(f);


	int lvl = 0;
	for(auto& r : layers)
	{
		if(!cd.disable_dither)
			r.dither_floyd_steinberg(steps);

		td.layers.push_back(TextureLayer(lvl));
		r.to_texture_layer(td.layers.back(),
						   cd.output_format,
						   cd.output_data_type);

		lvl++;
	}

	td.write(cd.output_image);

	return 0;

}

