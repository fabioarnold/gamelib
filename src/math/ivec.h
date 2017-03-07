union ivec2 {
	struct {int x, y;};
	int e[2];

	inline int& operator[](int index) {
		assert(index >= 0 && index < 2);
		return e[index];
	}
};

inline ivec2 iv2(int x, int y) {
	ivec2 v; v.x = x; v.y = y;
	return v;
}
inline ivec2 iv2(int x) {
	ivec2 v; v.x = x; v.y = x;
	return v;
}
inline ivec2 iv2(vec2 v) {
	ivec2 iv; iv.x = (int)v.x; iv.y = (int)v.y;
	return iv;
}
/* compatibility with vec.h */
inline vec2 v2(ivec2 iv) {
	vec2 v; v.x = (float)iv.x; v.y = (float)iv.y;
	return v;
}

inline ivec2 operator+(ivec2 v0, ivec2 v1) {
	ivec2 result;
	result.x = v0.x + v1.x;
	result.y = v0.y + v1.y;
	return result;
}
inline void operator+=(ivec2 &v0, ivec2 v1) {
	v0.x += v1.x;
	v0.y += v1.y;
}
inline ivec2 operator-(ivec2 v0, ivec2 v1) {
	ivec2 result;
	result.x = v0.x - v1.x;
	result.y = v0.y - v1.y;
	return result;
}
inline ivec2 operator*(int f, ivec2 v) {
	ivec2 result;
	result.x = f * v.x;
	result.y = f * v.y;
	return result;
}
inline ivec2 operator*(ivec2 v, int f) {
	return f*v;
}
inline ivec2 operator/(ivec2 v, int d) {
	ivec2 result;
	result.x = v.x / d;
	result.y = v.y / d;
	return result;
}
