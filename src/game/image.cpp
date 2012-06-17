/*********************************************************
 * image object -- See header file for more information. *
 *********************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <png.h>

#include "image.h"

namespace Loader {

//------------------------------------------------------------------------------

Image::Image(udword w, udword h, ubyte d)
	: width(w), height(h), depth(d)
{
	data = new ubyte[w*h*d];
}

//------------------------------------------------------------------------------

Image::~Image()
{
	if (data)
		delete[] data;
}

//------------------------------------------------------------------------------

unsigned long Image::size()
{
	return (width * height * depth);
}

//------------------------------------------------------------------------------

PngImage::PngImage(const char *filename)
{
	png_byte header[8];
	png_structp png;
	png_infop info;
	
	FILE *fp = fopen(filename, "rb");
	if (!fp)
	{
		//fputs("[PngImage] file not found\n", stderr);
		return;
	}
	
	fread(header, sizeof (char), 8, fp);
	if (png_sig_cmp(header, 0, 8))
	{
		fputs("[PngImage] wrong signature\n", stderr);
		fclose(fp);
		return;
	}
	
	if (!(png = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0)))
	{
		fputs("[PngImage] allocation failed\n", stderr);
		fclose(fp);
		return;
	}
	
	if (!(info = png_create_info_struct(png)))
	{
		fputs("[PngImage] allocation failed\n", stderr);
		png_destroy_read_struct(&png, 0, 0);
		fclose(fp);
		return;
	}
	
	if (setjmp(png_jmpbuf(png)))
	{
		fputs("[PngImage] image info error\n", stderr);
		png_destroy_read_struct(&png, &info, 0);
		fclose(fp);
		return;
	}
	
	png_init_io(png, fp);
	png_set_sig_bytes(png, 8);
	png_read_info(png, info);
	
	png_byte type = png_get_color_type(png, info);
	width = png_get_image_width(png, info);
	height = png_get_image_height(png, info);
	depth = png_get_bit_depth(png, info);
	
	png_set_interlace_handling(png);
	
	if (type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);
	
	if ((type == PNG_COLOR_TYPE_GRAY) && (depth < 8))
		png_set_expand_gray_1_2_4_to_8(png);
	
	if (png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);
	else
		png_set_add_alpha(png, 0xff, PNG_FILLER_AFTER);
	
	if (depth == 16)
		png_set_strip_16(png);
	else if (depth < 8)
		png_set_packing(png);
	
	png_read_update_info(png, info);
	depth = 4;
	
	data = new ubyte[width * height * depth];
	
	if (setjmp(png_jmpbuf(png)))
	{
		fputs("[PngImage] image data error\n", stderr);
		png_destroy_read_struct(&png, &info, 0);
		fclose(fp);
		delete[] data;
		data = NULL;
		return;
	}
	
	png_bytep *rows = new ubyte *[height];
	for (int i = 0, j = height - 1; i < height; ++i, --j)
		rows[i] = &data[j * depth * width];
	
	png_read_image(png, rows);
	
	png_read_end(png, 0);
	delete[] rows;
	png_destroy_read_struct(&png, &info, 0);
	fclose(fp);
}

//------------------------------------------------------------------------------

} // namespace Loader

//..............................................................................
