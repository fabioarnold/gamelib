// we only compile for platforms where these types hold

typedef signed char 		s8;
typedef signed short 		s16;
typedef signed int 			s32;
typedef signed long long 	s64;

typedef unsigned char 		u8;
typedef unsigned short 		u16;
typedef unsigned int 		u32;
typedef unsigned long long 	u64;

typedef float 				f32;

union IntFloat {
	int   i;
	float f;
};

#define ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]))
#define ARRAY_FREE(a) if (!(a)) {delete [] (a); (a) = NULL;}

bool isPowerOfTwo(u32 i) {
	return (i != 0) && !(i & (i-1));
}

int imin(int i0, int i1) {
	return i0 < i1 ? i0 : i1;
}

int imax(int i0, int i1) {
	return i0 > i1 ? i0 : i1;
}

bool fequal(float f0, float f1, float eps = 1e-6f) {
	return fabsf(f1-f0) < eps;
}
