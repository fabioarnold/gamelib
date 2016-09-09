#ifndef MAKEFOURCC
#define MAKEFOURCC(c) \
	((u32)(c[0]) | \
	((u32)(c[1]) << 8) | \
	((u32)(c[2]) << 16) | \
	((u32)(c[3]) << 24))
#endif

#define FOURCC_DDS  MAKEFOURCC("DDS ")
#define FOURCC_DXT1 MAKEFOURCC("DXT1")
#define FOURCC_DXT3 MAKEFOURCC("DXT3")
#define FOURCC_DXT5 MAKEFOURCC("DXT5")

struct DDSColorKey {
	u32 low_value;
	u32 high_value;
};

struct DDSPixelFormat {
	u32 size;
	u32 flags;
	u32 fourcc;
	u32 depth; // bits per pixel
	u32 red_mask;
	u32 green_mask;
	u32 blue_mask;
	u32 alpha_mask;
};

struct DDSHeader {
	u32 fourcc;
	u32 size;
	u32 flags;
	u32 height;
	u32 width;
	u32 pitch; // linear size
	u32 depth;
	u32 mipmap_count;
	u32 alpha_depth;
	u32 reserved;
	u32 surface;

	DDSColorKey dst_overlay;
	DDSColorKey dst_blit;
	DDSColorKey src_overlay;
	DDSColorKey src_blit;

	DDSPixelFormat pf;
	u32 caps[4];

	u32 texture_stage;
};

GLuint loadCompressedTexture2D(const char *filepath, int *out_width, int *out_height) {
	static char filepath_dds[256];
	sprintf(filepath_dds, "%s.dds", filepath);

	size_t data_size; // TODO: check we don't read off limits
	u8 *data = readDataFromFile(filepath_dds, &data_size);
	if (!data) return 0;

	DDSHeader *header = (DDSHeader*)data;
#ifdef DEBUG
	if (header->fourcc != FOURCC_DDS) {
		LOGE("magic number mismatch in \"%s\"\n", filepath_dds);
		delete [] data;
		return 0;
	}
#endif

	GLenum internal_format;
	if (header->pf.fourcc == FOURCC_DXT1) {
		internal_format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
	} else if (header->pf.fourcc == FOURCC_DXT3) {
		internal_format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
	} else if (header->pf.fourcc == FOURCC_DXT5) {
		internal_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
	} else {
		LOGE("unknown pixel format in \"%s\"\n", filepath_dds);
		delete [] data;
		return 0;
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	setFilterTexture2D(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	setWrapTexture2D(GL_REPEAT, GL_REPEAT);

	int block_size = (internal_format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	int width = header->width;
	int height = header->height;
	int offset = sizeof(DDSHeader);
	for (u32 level = 0; level < header->mipmap_count; level++) {
		int size = block_size * ((width+3)/4)*((height+3)/4);
		glCompressedTexImage2D(GL_TEXTURE_2D, level, internal_format, width, height, 0, size, data+offset);
		offset += size;
		if (width > 1) width /= 2;
		if (height > 1) height /= 2;
	}

	delete [] data;

	if (out_width) *out_width = header->width;
	if (out_height) *out_height = header->height;

	return texture;
}
