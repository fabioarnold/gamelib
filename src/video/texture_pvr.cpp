#ifndef MAKEFOURCC
#define MAKEFOURCC(c0, c1, c2, c3) \
	((u32)(c0) | \
	((u32)(c1) << 8) | \
	((u32)(c2) << 16) | \
	((u32)(c3) << 24))
#endif

#define FOURCC_PVRTEX3 MAKEFOURCC('P', 'V', 'R', 3)

struct PVRTextureHeaderV3 {
	u32 version;
	u32 flags;
	u64 pixel_format;
	u32 color_space;
	u32 channel_type;
	u32 height;
	u32 width;
	u32 depth;
	u32 surfaces_count;
	u32 faces_count;
	u32 mipmap_count;
	u32 metadata_size;
};

enum PVRTPixelFormat {
	PVRTPF_PVRTCI_2bpp_RGB,
	PVRTPF_PVRTCI_2bpp_RGBA,
	PVRTPF_PVRTCI_4bpp_RGB,
	PVRTPF_PVRTCI_4bpp_RGBA,
	PVRTPF_PVRTCII_2bpp,
	PVRTPF_PVRTCII_4bpp
};

/*
 loads some basic PVRTC1 compressed textures
 */
GLuint loadCompressedTexture2D(const char *filepath, int *out_width, int *out_height) {
	static char filepath_pvr[256];
	sprintf(filepath_pvr, "%s.pvr", filepath);

	size_t data_size; // TODO: check we don't read off limits
	u8 *data = readDataFromFile(filepath_pvr, &data_size);
	if (!data) return 0;

	PVRTextureHeaderV3 *header = (PVRTextureHeaderV3*)data;
#ifdef DEBUG
	if (header->version != FOURCC_PVRTEX3) {
		LOGE("magic number mismatch in \"%s\"\n", filepath_pvr);
		delete [] data;
		return 0;
	}
#endif

	GLenum internal_format;
	int bits_per_pixel;
	int block_width, block_height;
	switch (header->pixel_format) {
	case PVRTPF_PVRTCI_2bpp_RGB:
		internal_format = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
		bits_per_pixel = 2;
		block_width = 16;
		block_height = 8;
		break;
	case PVRTPF_PVRTCI_2bpp_RGBA:
		internal_format = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
		bits_per_pixel = 2;
		block_width = 16;
		block_height = 8;
		break;
	case PVRTPF_PVRTCI_4bpp_RGB:
		internal_format = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
		bits_per_pixel = 4;
		block_width = 8;
		block_height = 8;
		break;
	case PVRTPF_PVRTCI_4bpp_RGBA:
		internal_format = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
		bits_per_pixel = 4;
		block_width = 8;
		block_height = 8;
		break;
	default:
		LOGE("unknown pixel format in \"%s\"\n", filepath_pvr);
		delete [] data;
		return 0;
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	setFilterTexture2D(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	setWrapTexture2D(GL_REPEAT, GL_REPEAT);

	int block_size = block_width*block_height*bits_per_pixel/8; // in bytes
	int width = header->width;
	int height = header->height;
	int offset = sizeof(PVRTextureHeaderV3)-4+header->metadata_size;
	for (int level = 0; level < header->mipmap_count; level++) {
		int size = block_size * ((width+block_width-1)/block_width)*((height+block_height-1)/block_height);
		glCompressedTexImage2D(GL_TEXTURE_2D, level, internal_format, width, height, 0, size, data+offset);
		offset += size;
		if (width > 1) width /= 2;
		if (height > 1) height /= 2;
	}
#ifdef DEBUG
	if (offset != data_size) {
		LOGW("read data mismatch in \"%s\"\n", filepath_pvr);
	}
#endif

	delete [] data;

	if (out_width) *out_width = header->width;
	if (out_height) *out_height = header->height;

	return texture;

	return 0;
}
