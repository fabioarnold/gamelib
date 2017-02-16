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

#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))
#define ARRAY_FREE(a) if (!(a)) {delete [] (a); (a) = NULL;}

bool isPowerOfTwo(u32 i) {
	return (i != 0) && !(i & (i-1));
}
