/*
 the file extension is implementation defined (dds or pvr)
 so just pass in the texture basename without extension
 */
GLuint loadCompressedTexture2D(const char *filepath, int *out_width, int *out_height);

GLuint loadTexture2D(u8 *pixels, int width, int height, int comp, bool build_mipmaps);
/* loads uncompressed targa images */
GLuint loadTexture2D(const char *filepath, bool build_mipmaps, int *out_width, int *out_height);

// short hands for glTexParameteri
void setFilterTexture2D(GLint min_filter, GLint mag_filter);
void setWrapTexture2D(GLint wrap_s, GLint wrap_t);
