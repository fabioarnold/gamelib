#define COORDS(X,Y) (((Y)*width + (X))*comp)

void imageCopyPixels(u8 *dst, u8 *src, int dst_width, int src_width, int comp, int line_width, int num_lines) {
	while (num_lines-->0) {
		memcpy(dst, src, (size_t)(line_width * comp) * sizeof(u8));
		dst += dst_width * comp;
		src += src_width * comp;
	}
}

void imageHalve(u8 *pixels, int width, int height, int comp) {
	assert(width > 1 && height > 1);
	int half_width  = width  / 2;
	int half_height = height / 2;
	for (int y = 0; y < half_height; y++) {
		for (int x = 0; x < half_width; x++) {
			u8 *dst_pixel = pixels+((y*half_width + x) * comp);
			for (int ci = 0; ci < comp; ci++) {
				u16 c = (u16)(pixels[COORDS(2*x+0, 2*y+0) + ci]
							+ pixels[COORDS(2*x+1, 2*y+0) + ci]
							+ pixels[COORDS(2*x+0, 2*y+1) + ci]
							+ pixels[COORDS(2*x+1, 2*y+1) + ci]);
				dst_pixel[ci] = (u8)(c/4);
			}
		}
	}
}

void imageHalveHorizontally(u8 *pixels, int width, int height, int comp) {
	assert(width > 1);
	int half_width = width  / 2;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < half_width; x++) {
			u8 *dst_pixel = pixels+((y*half_width + x) * comp);
			for (int ci = 0; ci < comp; ci++) {
				u16 c = (u16)(pixels[COORDS(2*x+0, 2*y) + ci]
							+ pixels[COORDS(2*x+1, 2*y) + ci]);
				dst_pixel[ci] = (u8)(c/2);
			}
		}
	}
}

void imageHalveVertically(u8 *pixels, int width, int height, int comp) {
	assert(height > 1);
	int half_height = height / 2;
	for (int y = 0; y < half_height; y++) {
		for (int x = 0; x < width; x++) {
			u8 *dst_pixel = pixels+((y*width + x) * comp);
			for (int ci = 0; ci < comp; ci++) {
				u16 c = (u16)(pixels[COORDS(2*x, 2*y+0) + ci]
							+ pixels[COORDS(2*x, 2*y+1) + ci]);
				dst_pixel[ci] = (u8)(c/2);
			}
		}
	}
}

void imageFlipHorizontally(u8 *pixels, int width, int height, int comp) {
	u8 *buffer = new u8[comp];
	for (int y = 0; y < height; y++) {
		int x1 = 0, x2 = width-1;
		while (x1 < x2) {
			memcpy(buffer, pixels + (y * width + x1) * comp, (size_t)comp * sizeof(u8));
			memcpy(pixels + (y * width + x1) * comp, pixels + (y * width + x2) * comp, (size_t)comp * sizeof(float));
			memcpy(pixels + (y * width + x2) * comp, buffer, (size_t)comp * sizeof(u8));
			x1++;
			x2--;
		}
	}
	delete [] buffer;
}

void imageFlipVertically(u8 *pixels, int width, int height, int comp) {
	const size_t line_size = (size_t)(width * comp) * sizeof(u8);
	u8 *buffer = new u8[width * comp];
	int y1 = 0, y2 = height - 1;
	while (y1 < y2) {
		memcpy(buffer,                     pixels + y1 * width * comp, line_size);
		memcpy(pixels + y1 * width * comp, pixels + y2 * width * comp, line_size);
		memcpy(pixels + y2 * width * comp, buffer,                     line_size);
		y1++;
		y2--;
	}
	delete [] buffer;
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

float *imageFloat(u8 *pixels, int comp_count) {
	float *pixelsf = new float[comp_count];

	while (comp_count-->0) {
		pixelsf[comp_count] = ((float)pixels[comp_count]) / 255.0f;
	}

	return pixelsf;
}

static inline void imageSwapPixel(float *dst, float *src, int comp) {
	assert(comp > 0 && comp <= 4);
	const size_t pixel_size = (size_t)comp * sizeof(float);
	float buffer[comp];
	memcpy(buffer, dst, pixel_size);
	memcpy(dst,    src, pixel_size);
	memcpy(src, buffer, pixel_size);
}

// square
void imageTranspose(float *pixels, int width, int comp) {
	for (int y = 1; y < width; y++) {
		for (int x = 0; x < y; x++) {
			imageSwapPixel(pixels + COORDS(x,y), pixels + COORDS(y,x), comp);
		}
	}
}

void imageTranspose(float *dst, float *src, int width, int height, int comp) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			float *dst_pixel = dst + (x*height + y)*comp;
			float *src_pixel = src + COORDS(x,y);
			for (int c = 0; c < comp; c++) {
				dst_pixel[c] = src_pixel[c];
			}
		}
	}
}

void imageCopyPixels(float *dst, float *src, int dst_width, int src_width, int comp, int line_width, int num_lines) {
	const size_t line_size = (size_t)(line_width * comp) * sizeof(float);
	while (num_lines-- > 0) {
		memcpy(dst, src, line_size);
		dst += dst_width * comp;
		src += src_width * comp;
	}
}

void imageHalve(float *pixels, int width, int height, int comp) {
	assert(width > 1 && height > 1 && (width%2) == 0 && (height%2) == 0);
	int half_width  = width  / 2;
	int half_height = height / 2;
	for (int y = 0; y < half_height; y++) {
		for (int x = 0; x < half_width; x++) {
			float *dst_pixel = pixels+((y*half_width + x)*comp);
			for (int ci = 0; ci < comp; ci++) {
				float c = pixels[COORDS(2*x+0,2*y+0) + ci]
						+ pixels[COORDS(2*x+1,2*y+0) + ci]
						+ pixels[COORDS(2*x+0,2*y+1) + ci]
						+ pixels[COORDS(2*x+1,2*y+1) + ci];
				dst_pixel[ci] = c/4.0f;
			}
		}
	}
}void imageHalveHorizontally(float *pixels, int width, int height, int comp) {
	assert(width > 1);
	int half_width = width  / 2;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < half_width; x++) {
			float *dst_pixel = pixels+((y*half_width + x) * comp);
			for (int ci = 0; ci < comp; ci++) {
				float c = pixels[COORDS(2*x+0, 2*y) + ci]
						+ pixels[COORDS(2*x+1, 2*y) + ci];
				dst_pixel[ci] = (float)(c/2);
			}
		}
	}
}

void imageHalveVertically(float *pixels, int width, int height, int comp) {
	assert(height > 1);
	int half_height = height / 2;
	for (int y = 0; y < half_height; y++) {
		for (int x = 0; x < width; x++) {
			float *dst_pixel = pixels+((y*width + x) * comp);
			for (int ci = 0; ci < comp; ci++) {
				float c = pixels[COORDS(2*x, 2*y+0) + ci]
						+ pixels[COORDS(2*x, 2*y+1) + ci];
				dst_pixel[ci] = (float)(c/2);
			}
		}
	}
}

// apply 5x5 gauss while halving
void imageReduce(float *pixels, int width, int height, int comp) {
	assert(width > 2 && height > 2 && (width%2) == 0 && (height%2) == 0);

	const float ka = 2.0f/5.0f; // gauss
	const float kb = 1.0f/4.0f;
	const float kc = kb - ka/2.0f;

	int half_width  = width  / 2;
	int half_height = height / 2;
	float *buffer = new float[half_width*height*comp];

	for (int i = 0; i < 2; i++) {
		float *src = pixels;
		float *dst = buffer;
		for (int y = 0; y < height; y++) {
			// clamp border
			int x = 0;
			float *dst_pixel = dst+((y*half_width + x)*comp);
			for (int ci = 0; ci < comp; ci++) {
				float c = kc * src[COORDS(2*x+0,y) + ci]
						+ kb * src[COORDS(2*x+0,y) + ci]
						+ ka * src[COORDS(2*x+0,y) + ci]
						+ kb * src[COORDS(2*x+1,y) + ci]
						+ kc * src[COORDS(2*x+2,y) + ci];
				dst_pixel[ci] = c;
			}

			// image
			for (x = 1; x < half_width-1; x++) {
				dst_pixel = dst+((y*half_width + x)*comp);
				for (int ci = 0; ci < comp; ci++) {
					float c = kc * src[COORDS(2*x-2,y) + ci]
							+ kb * src[COORDS(2*x-1,y) + ci]
							+ ka * src[COORDS(2*x+0,y) + ci]
							+ kb * src[COORDS(2*x+1,y) + ci]
							+ kc * src[COORDS(2*x+2,y) + ci];
					dst_pixel[ci] = c;
				}
			}

			// clamp border
			x = half_width-1;
			dst_pixel = dst+((y*half_width + x)*comp);
			for (int ci = 0; ci < comp; ci++) {
					float c = kc * src[COORDS(2*x-2,y) + ci]
							+ kb * src[COORDS(2*x-1,y) + ci]
							+ ka * src[COORDS(2*x+0,y) + ci]
							+ kb * src[COORDS(2*x+0,y) + ci]
							+ kc * src[COORDS(2*x+0,y) + ci];
				dst_pixel[ci] = c;
			}
		}
		src = buffer;
		dst = pixels;
		imageTranspose(dst, src, half_width, i==0?height:half_height, comp);
	}

	delete [] buffer;
}

// complementary to imageReduce
void imageExpand(float *pixels, int width, int height, int comp) {
	const float ka = 2.0f/5.0f; // gauss
	const float kb = 1.0f/4.0f;
	const float kc = kb - ka/2.0f;

	int double_width  = 2 * width;
	int double_height = 2 * height;
	float *buffer = new float[double_width*double_height*comp];

	for (int i = 0; i < 2; i++) {
		float *src = pixels;
		float *dst = buffer;
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				float *src_pixel_m1 = src+COORDS(x-1 >= 0    ? x-1 : x,y); // clamp
				float *src_pixel_p0 = src+COORDS(x+0,y);
				float *src_pixel_p1 = src+COORDS(x+1 < width ? x+1 : x,y); // clamp
				float *dst_pixel0 = dst+((y*double_width + 2*x+0)*comp);
				float *dst_pixel1 = dst+((y*double_width + 2*x+1)*comp);
				for (int ci = 0; ci < comp; ci++) {
					dst_pixel0[ci] = 2.0f * (
						  kc * src_pixel_m1[ci]
						+ ka * src_pixel_p0[ci]
						+ kc * src_pixel_p1[ci]);
					dst_pixel1[ci] = 2.0f * (
						  kb * src_pixel_p0[ci]
						+ kb * src_pixel_p1[ci]);
				}
			}
		}
		src = buffer;
		dst = pixels;
		imageTranspose(dst, src, double_width, height, comp);
		height = double_height;
	}

	delete [] buffer;
}

void imageFlipHorizontally(float *pixels, int width, int height, int comp) {
	for (int y = 0; y < height; y++) {
		int x1 = 0, x2 = width-1;
		while (x1 < x2) {
			imageSwapPixel(pixels + COORDS(x1,y), pixels + COORDS(x2,y), comp);
			x1++;
			x2--;
		}
	}
}

void imageFlipVertically(float *pixels, int width, int height, int comp) {
	float *line = new float[width * comp];
	size_t line_size = (size_t)(width * comp) * sizeof(float);
	int y1 = 0, y2 = height - 1;
	while (y1 < y2) {
		memcpy(line,                       pixels + y1 * width * comp, line_size);
		memcpy(pixels + y1 * width * comp, pixels + y2 * width * comp, line_size);
		memcpy(pixels + y2 * width * comp, line,                       line_size);
		y1++;
		y2--;
	}
	delete [] line;
}

#undef COORDS
