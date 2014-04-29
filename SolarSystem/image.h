#ifndef IMAGE_H
#define IMAGE_H

#include <fstream>
#include <string>
#include <cassert>
#include <unordered_map>
#include <utility>
#include <vector>
#include <iostream>

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include "auto_array.h"

using namespace std;

namespace util {

class image {
public:
	image(char* ps, int w, int h): 
		pixels(ps), width(w), height(h) {
	}

	~image() {
		cout << "release pixels data\n";
		delete[] pixels;
	}

	/* An array of the form (R1, G1, B1, R2, G2, B2, ...) indicating the
	 * color of each pixel in image.  Color components range from 0 to 255.
	 * The array starts the bottom-left pixel, then moves right to the end
	 * of the row, then moves up to the next column, and so on.  This is the
	 * format in which OpenGL likes images.
	 */
	char* pixels;
	int width;
	int height;
};

namespace {
	int to_int(const char* bytes) {
		return (int)(((unsigned char)bytes[3] << 24) | 
					 ((unsigned char)bytes[2] << 16) |
					 ((unsigned char)bytes[1] << 8) |
					  (unsigned char)bytes[0]);
	}

	/**
	 * Converts a two-character array to a short, using little-endian form
	 */
	short to_short(const char* bytes) {
		return (short)(((unsigned char)bytes[1] << 8) | (unsigned char)bytes[0]);
	}

	/**
	 * Reads the next four bytes as an integer, using little-endian form
	 */
	int read_int(ifstream &input) {
		char buffer[4];
		input.read(buffer, 4);
		return to_int(buffer);
	}

	/** 
	 * Reads the next two bytes as a short, using little-endian form
	 */
	short read_short(ifstream &input) {
		char buffer[2];
		input.read(buffer, 2);
		return to_short(buffer);
	}
}

image* load_bmp(const char* filename) {
	ifstream input;
	input.open(filename, ifstream::binary);
	assert(!input.fail() || !"Could not find file");
	char buffer[2];
	input.read(buffer, 2);
	assert(buffer[0] == 'B' && buffer[1] == 'M' || !"Not a bitmap file");
	input.ignore(8);
	int data_offset = read_int(input);

	// read the header
	int header_size = read_int(input);
	int width;
	int height;
	switch (header_size) {
		case 40:
			//V3
			width = read_int(input);
			height = read_int(input);
			input.ignore(2);
			assert(read_short(input) == 24 || !"Image is not 24 bits per pixel");
			assert(read_short(input) == 0 || !"Image is compressed");
			break;

		case 12:
			//OS/2 V1
			width = read_short(input);
			height = read_short(input);
			input.ignore(2);
			assert(read_short(input) == 24 || !"Image is not 24 bits per pixel");
			break;

		case 64:
			//OS/2 V2
			assert(!"Can't load OS/2 V2 bitmaps");
			break;

		case 108:
			//Windows V4
			assert(!"Can't load Windows V4 bitmaps");
			break;

		case 124:
			//Windows V5
			assert(!"Can't load Windows V5 bitmaps");
			break;

		default:
			assert(!"Unknown bitmap format");
	}

	// read the data
	int bytes_per_row = ((width * 3 + 3) / 4) * 4 - (width * 3 % 4);
	int size = bytes_per_row * height;
	auto_array<char> pixels(new char[size]);
	input.seekg(data_offset, ios_base::beg);
	input.read(pixels.get(), size);

	// get the data into the right format
	auto_array<char> pixels2(new char[width * height * 3]);
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			for(int c = 0; c < 3; c++) {
				pixels2[3 * (width * y + x) + c] = pixels[bytes_per_row * y + 3 * x + (2 - c)];
			}
		}
	}

	// release resources
	input.close();
	return new image(pixels2.release(), width, height);
}

/**
 * Returns an array indicating pixel data for an RGBA image that is the same as
 * image, but with an alpha channel indicated by the gray scale image alpha_channel
 */
char* add_alpha_channel(image* img, image* alpha_channel) {
	char* pixels = new char[img->width * img->height * 4];
	for (int y = 0; y < img->height; y++) {
		for (int x = 0; x < img->width; x++) {
			for (int j = 0; j < 3; j++) {
				pixels[4 * (y * img->width + x) + j] = img->pixels[3 * (y * img->width + x) + j];
			}
			pixels[4 * (y * img->width + x) + 3] = alpha_channel->pixels[3 * (y * img->width + x)];
		}
	}
	return pixels;
}

unsigned load_alpha_texture(image* img, image* alpha_channel) {
	char* pixels = add_alpha_channel(img, alpha_channel);
	unsigned texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA,
		img->width, img->height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		pixels);
	delete pixels;
	return texture_id;
}

unsigned load_texture_from_image(image* img) {
	unsigned texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		img->width, img->height,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		img->pixels);
	return texture_id;
}

unordered_map<string, unsigned> load_multiple_textures_from_image(vector<image*> images, const vector<string> &filename) {
	unordered_map<string, unsigned> hm;
	unsigned no_textures = filename.size();
	unsigned *texture_ids = new unsigned[no_textures];
	// generate textures for all bmp files
	glGenTextures(no_textures, texture_ids);
	for (unsigned i = 0; i < no_textures; ++i) {
		glBindTexture(GL_TEXTURE_2D, texture_ids[i]);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGB,
			images[i]->width, images[i]->height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			images[i]->pixels);
		// insert into hash-map for later look up
		hm.insert(std::make_pair(filename[i], texture_ids[i]));
	}
	return hm;
}




} // end namespace 
#endif