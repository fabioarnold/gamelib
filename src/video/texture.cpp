void setFilterTexture2D(GLint min_filter, GLint mag_filter) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
}

void setWrapTexture2D(GLint wrap_s, GLint wrap_t) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
}

GLuint loadTexture2D(u8 *pixels, int width, int height, int comp, bool build_mipmaps) {
	if (build_mipmaps) {
		if (!(isPowerOfTwo(width) && isPowerOfTwo(height))) {
			LOGW("Trying to build mipmap of non power of two image");
			build_mipmaps = false;
		}
	}

	GLint internal_format;
	GLenum format;
	switch (comp) {
		case 1: format = internal_format = GL_ALPHA; break;
		case 2: format = internal_format = GL_LUMINANCE_ALPHA; break;
		case 3: format = internal_format = GL_RGB; break;
		case 4: format = internal_format = GL_RGBA; break;
		default: LOGE("Invalid bit depth"); return 0;
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	if (build_mipmaps) {
		setFilterTexture2D(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	} else {
		setFilterTexture2D(GL_LINEAR, GL_LINEAR);
	}
	setWrapTexture2D(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height,
		0, format, GL_UNSIGNED_BYTE, pixels);
#ifdef DEBUG
	GLenum gl_error = glGetError();
	if (gl_error) {
		LOGE("gl error: %d", gl_error);
	}
#endif

	// mipmap building
	if (build_mipmaps && width > 1 && height > 1) {
		// test for power of two
		u8 *buffer = new u8[width * height * comp / 4];
		u8 *pixels2 = buffer;

		int mipmap_level = 0;
		do {
			width  /= 2;
			height /= 2;
			mipmap_level++;

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					u8 *pixel2 = pixels2+((y*width + x) * comp);
					for (int ci = 0; ci < comp; ci++) {
						u16 c = pixels[((2*y+0)*2*width + (2*x+0))*comp + ci]
							  + pixels[((2*y+0)*2*width + (2*x+1))*comp + ci]
							  + pixels[((2*y+1)*2*width + (2*x+0))*comp + ci]
							  + pixels[((2*y+1)*2*width + (2*x+1))*comp + ci];
						pixel2[ci] = (u8)(c/4);
					}
				}
			}
			glTexImage2D(GL_TEXTURE_2D, mipmap_level, internal_format,
				width, height, 0, format, GL_UNSIGNED_BYTE, pixels2);

			// swap
			u8 *tmp = pixels;
			pixels = pixels2;
			pixels2 = tmp;
		} while (width > 1 && height > 1);

		delete [] buffer;
	}

	return texture;
}

GLuint loadTexture2DTGA(const char *filepath, bool build_mipmaps, int *out_width, int *out_height);

GLuint loadTexture2D(const char *filepath, bool build_mipmaps, int *out_width, int *out_height){
	const char *ext = strrchr(filepath, (int)'.');
	if (!ext) {
		LOGE("Image extension not found: %s", filepath);
		return 0;
	}

#ifdef USE_STB_IMAGE
	int comp;
	u8 *pixels = stbi_load(filepath, out_width, out_height, &comp, 0);
	if (pixels) {
		GLuint texture = loadTexture2D(pixels, *out_width, *out_height, comp, build_mipmaps);
		stbi_image_free(pixels);
		return texture;
	} else {
		LOGE("Unrecognized image format: %s", filepath);
	}
#else
	if (!strncmp(ext, ".tga", 4)) {
		return loadTexture2DTGA(filepath, build_mipmaps, out_width, out_height);
	} else {
		LOGE("Unrecognized image format: %s", filepath);
	}
#endif
	return 0;
}

// helper function for tga
void imageSwapChannelsRB(u8 *pixels, int pixel_count, int comp) {
	while (pixel_count-->0) {
		u8 blue = pixels[0];
		pixels[0] = pixels[2];
		pixels[2] = blue;
		pixels += comp;
	}
}

/* simplified tga image loader */
enum TargaColorType {
	TARGA_COLOR_TYPE_TRUECOLOR, // 0
	TARGA_COLOR_TYPE_INDEXED // 1
};

enum TargaImageType {
	TARGA_IMAGE_TYPE_NONE, // 0
	TARGA_IMAGE_TYPE_INDEXED, // 1
	TARGA_IMAGE_TYPE_TRUECOLOR, // 2
	TARGA_IMAGE_TYPE_GREYSCALE, // 3
	TARGA_IMAGE_TYPE_RLE_BIT = 0x08 // runlength encoded
};

#pragma pack(push, 1) // tightly pack this struct
struct TargaHeader {
	u8 offset; // offset + 18 == start of palette/image data
	u8 color_type;
	u8 image_type;

	u16 palette_start;
	u16 palette_length; // number of entries
	u8 palette_depth; // bits per color entry

	u16 origin_x;
	u16 origin_y;
	u16 width;
	u16 height;

	u8 depth;
	u8 flags;
}; // 18 byte
#pragma pack(pop)

#define TARGA_ORIGIN_TOP_FLAG (0x1<<5)

GLuint loadTexture2DTGA(const char* filepath, bool build_mipmaps, int *out_width, int *out_height) {
	u8 *data = readDataFromFile(filepath, NULL);
	if (!data) return 0;

	TargaHeader *header = (TargaHeader*)data;
#ifdef DEBUG
	if (header->offset) {
		LOGE("TGA image \"%s\" has metadata. This is currently unsupported.", filepath);
		delete [] data;
		return 0;
	}
	if (header->color_type != TARGA_COLOR_TYPE_TRUECOLOR
	 || header->image_type != TARGA_IMAGE_TYPE_TRUECOLOR) {
		LOGE("TGA image \"%s\" isn't truecolor or might be rle.", filepath);
		delete [] data;
		return 0;
	}
#if 0
	if (header->origin_x != 0 || header->origin_y != header->height) {
		LOGW("TGA image \"%s\" is unexpectedly offset: %d %d", filepath, header->origin_x, header->origin_y);
	}
#endif
	if (!(header->flags & TARGA_ORIGIN_TOP_FLAG)) {
		LOGW("TGA image \"%s\" has its origin in the bottom left corner: flags=0x%X", filepath, header->flags);
	}
#endif

	u8 *pixels = data+sizeof(TargaHeader);
	int width = header->width;
	int height = header->height;
	int comp = header->depth/8;
	// targa has pixels stored in the order BGRA but gles needs RGBA
	if (comp >= 3) imageSwapChannelsRB(pixels, width*height, comp);

	GLuint texture = loadTexture2D(pixels, width, height, comp, build_mipmaps);

	delete [] data;

	if (out_width) *out_width = width;
	if (out_height) *out_height = height;

	return texture;
}
