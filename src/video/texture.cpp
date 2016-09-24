void imageCopyPixels(u8 *dest, u8 *src, int dest_width, int src_width, int comp, int line_width, int num_lines);
void imageHalve(u8 *pixels, int width, int height, int comp);
void imageFlipHorizontally(u8 *pixels, int width, int height, int comp);
void imageFlipVertically(u8 *pixels, int width, int height, int comp);

void imageCopyPixels(float *dest, float *src, int dest_width, int src_width, int comp, int line_width, int num_lines);
void imageHalve(float *pixels, int width, int height, int comp);
void imageFlipHorizontally(float *pixels, int width, int height, int comp);
void imageFlipVertically(float *pixels, int width, int height, int comp);

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
	if (build_mipmaps) {
		for (int mipmap_level = 1; width > 1 && height > 1; mipmap_level++) {
			imageHalve(pixels, width, height, comp);
			width  /= 2;
			height /= 2;
			glTexImage2D(GL_TEXTURE_2D, mipmap_level, internal_format,
				width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
		}
	}

	return texture;
}

GLuint loadTexture2D(float *pixels, int width, int height, int comp, bool build_mipmaps) {
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

#if 0 // test expand code
	u64 t = SDL_GetPerformanceCounter();
		int expand_level = 4;
		for (int level = 0; level < expand_level; level++) {
			imageReduce(pixels, width, height, comp);
			width  /= 2;
			height /= 2;
		}
		for (int level = 0; level < expand_level; level++) {
			imageExpand(pixels, width, height, comp);
			width  *= 2;
			height *= 2;
		}
	t = SDL_GetPerformanceCounter() - t;
	LOGI("%llu ns", t);
#endif

	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height,
		0, format, GL_FLOAT, pixels);
#ifdef DEBUG
	GLenum gl_error = glGetError();
	if (gl_error) {
		LOGE("gl error: %d", gl_error);
	}
#endif

	// mipmap building
	if (build_mipmaps) {
		for (int mipmap_level = 1; width > 1 && height > 1; mipmap_level++) {
			#if 0
			if (width > 2 && height > 2)
				imageReduce(pixels, width, height, comp);
			else
			#endif
				imageHalve(pixels, width, height, comp);
			width  /= 2;
			height /= 2;
			glTexImage2D(GL_TEXTURE_2D, mipmap_level, internal_format,
				width, height, 0, format, GL_FLOAT, pixels);
		}
	}

	return texture;
}

GLuint loadTexture2DTGA(const char *filepath, bool build_mipmaps, int *out_width, int *out_height);

GLuint loadTexture2D(const char *filepath, bool build_mipmaps, int *out_width, int *out_height) {
	const char *ext = strrchr(filepath, (int)'.');
	if (!ext) {
		LOGE("Image extension not found: %s", filepath);
		return 0;
	}

#ifdef USE_STB_IMAGE
	int comp;
	if (!strncmp(ext, ".hdr", 4)) {
		float *pixels = stbi_loadf(filepath, out_width, out_height, &comp, 0);
		if (pixels) {
			GLuint texture = loadTexture2D(pixels, *out_width, *out_height, comp, build_mipmaps);
			stbi_image_free(pixels);
			return texture;
		} else {
			LOGE("Invalid HDR file: %s", filepath);
		}
	} else {
		u8 *pixels = stbi_load(filepath, out_width, out_height, &comp, 0);
		if (pixels) {
			GLuint texture = loadTexture2D(pixels, *out_width, *out_height, comp, build_mipmaps);
			stbi_image_free(pixels);
			return texture;
		} else {
			LOGE("Unrecognized image format: %s", filepath);
		}
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

GLuint loadTextureCubeCross(u8 *pixels, int width, int height, int comp, bool build_mipmaps) {
	int slice_size = width / 3;
	if (!isPowerOfTwo(slice_size) || (height/4 != slice_size)) {
		LOGE("Incorrect dimensions for cube cross: %dx%d", width, height);
		return 0;
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

	GLuint cubemap;
	glGenTextures(1, &cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#ifndef USE_OPENGLES
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
#endif

	if (build_mipmaps) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	} else {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	u8 *slice_pixels = new u8[slice_size*slice_size*comp];

	int cross_offsets[] = {
		comp * (width * 1*slice_size + 2*slice_size),
		comp * (width * 1*slice_size + 0*slice_size),
		comp * (width * 0*slice_size + 1*slice_size),
		comp * (width * 2*slice_size + 1*slice_size),
		comp * (width * 1*slice_size + 1*slice_size),
		comp * (width * 3*slice_size + 1*slice_size)
	};

	for (int si = 0; si < 6; si++) {
		imageCopyPixels(slice_pixels, pixels + cross_offsets[si],
			slice_size, width, comp, slice_size, slice_size);
		if (si == 5) {
			imageFlipHorizontally(slice_pixels, slice_size, slice_size, comp);
			imageFlipVertically(slice_pixels, slice_size, slice_size, comp);
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+si, 0,
			internal_format, slice_size, slice_size,
			0, format, GL_UNSIGNED_BYTE, slice_pixels);
		if (build_mipmaps) {
			int half_slice_size = slice_size;
			for (int mipmap_level = 1; half_slice_size > 1; mipmap_level++) {
				imageHalve(slice_pixels, half_slice_size, half_slice_size, comp);
				half_slice_size /= 2;
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+si, mipmap_level,
					internal_format, half_slice_size, half_slice_size,
					0, format, GL_UNSIGNED_BYTE, slice_pixels);
			}
		}	
	}

	delete [] slice_pixels;

	return cubemap;
}

GLuint loadTextureCubeCross(float *pixels, int width, int height, int comp, bool build_mipmaps) {
	int slice_size = width / 3;
	if (!isPowerOfTwo(slice_size) || (height/4 != slice_size)) {
		LOGE("Incorrect dimensions for cube cross: %dx%d", width, height);
		return 0;
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

	GLuint cubemap;
	glGenTextures(1, &cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#ifndef USE_OPENGLES
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
#endif

	if (build_mipmaps) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	} else {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	float *slice_pixels = new float[slice_size*slice_size*comp];

	int cross_offsets[] = {
		comp * (width * 1*slice_size + 2*slice_size),
		comp * (width * 1*slice_size + 0*slice_size),
		comp * (width * 0*slice_size + 1*slice_size),
		comp * (width * 2*slice_size + 1*slice_size),
		comp * (width * 1*slice_size + 1*slice_size),
		comp * (width * 3*slice_size + 1*slice_size)
	};

	for (int si = 0; si < 6; si++) {
		imageCopyPixels(slice_pixels, pixels + cross_offsets[si],
			slice_size, width, comp, slice_size, slice_size);
		if (si == 5) {
			imageFlipHorizontally(slice_pixels, slice_size, slice_size, comp);
			imageFlipVertically  (slice_pixels, slice_size, slice_size, comp);
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+si, 0,
			internal_format, slice_size, slice_size,
			0, format, GL_FLOAT, slice_pixels);
		if (build_mipmaps) {
			int half_slice_size = slice_size;
			for (int mipmap_level = 1; half_slice_size > 1; mipmap_level++) {
				imageHalve(slice_pixels, half_slice_size, half_slice_size, comp);
				half_slice_size /= 2;
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+si, mipmap_level,
					internal_format, half_slice_size, half_slice_size,
					0, format, GL_FLOAT, slice_pixels);
			}
		}	
	}

	delete [] slice_pixels;

	return cubemap;
}

GLuint loadTextureCubeCross(const char *filepath, bool build_mipmaps, int *out_size) {
	const char *ext = strrchr(filepath, (int)'.');
	if (!ext) {
		LOGE("Image extension not found: %s", filepath);
		return 0;
	}

#ifdef USE_STB_IMAGE
	int comp;
	if (!strncmp(ext, ".hdr", 4)) {
		int out_width, out_height;
		float *pixels = stbi_loadf(filepath, &out_width, &out_height, &comp, 0);
		if (out_size) *out_size = out_width/3;
		if (pixels) {
			GLuint texture = loadTextureCubeCross(pixels, out_width, out_height, comp, build_mipmaps);
			stbi_image_free(pixels);
			return texture;
		} else {
			LOGE("Unrecognized image format: %s", filepath);
		}
	} else {
		int out_width, out_height;
		u8 *pixels = stbi_load(filepath, &out_width, &out_height, &comp, 0);
		if (out_size) *out_size = out_width/3;
		if (pixels) {
			GLuint texture = loadTextureCubeCross(pixels, out_width, out_height, comp, build_mipmaps);
			stbi_image_free(pixels);
			return texture;
		} else {
			LOGE("Unrecognized image format: %s", filepath);
		}
	}
#else
	assert(!"Not implemented");
#endif

	return 0;
}

/* simplified TGA image loader (loads only a minimal subset) */
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

	//float *pixelsf = imageFloat(pixels, width*height*comp);
	GLuint texture = loadTexture2D(pixels, width, height, comp, build_mipmaps);
	//delete [] pixelsf;

	delete [] data;

	if (out_width) *out_width = width;
	if (out_height) *out_height = height;

	return texture;
}
