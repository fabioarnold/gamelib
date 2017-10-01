void imageCopyPixels(u8 *dest, u8 *src, int dest_width, int src_width, int comp, int line_width, int num_lines);
void imageHalve(u8 *pixels, int width, int height, int comp);
void imageHalveHorizontally(u8 *pixels, int width, int height, int comp);
void imageHalveVertically(u8 *pixels, int width, int height, int comp);
void imageFlipHorizontally(u8 *pixels, int width, int height, int comp);
void imageFlipVertically(u8 *pixels, int width, int height, int comp);
void imageSwapChannelsRB(u8 *pixels, int pixel_count, int comp);

float *imageFloat(u8 *pixels, int comp_count/*=width*height*comp*/);

void imageCopyPixels(float *dest, float *src, int dest_width, int src_width, int comp, int line_width, int num_lines);
void imageHalve(float *pixels, int width, int height, int comp);
void imageHalveHorizontally(float *pixels, int width, int height, int comp);
void imageHalveVertically(float *pixels, int width, int height, int comp);
void imageFlipHorizontally(float *pixels, int width, int height, int comp);
void imageFlipVertically(float *pixels, int width, int height, int comp);
void imageTranspose(float *pixels, int width, int height, int comp);
