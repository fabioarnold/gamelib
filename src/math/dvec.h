union dvec2 {
	struct {double x, y;};
	double e[2];

	inline double& operator[](int index) {
		assert(index >= 0 && index < 2);
		return e[index];
	}
};

union dvec3 {
	struct {double x, y, z;};
	struct {double r, g, b;};
	double e[3];

	inline double& operator[](int index) {
		assert(index >= 0 && index < 3);
		return e[index];
	}
};

inline dvec3 dv3(double f) {
	dvec3 v; v.x = f; v.y = f; v.z = f;
	return v;
}
inline dvec3 dv3(double x, double y, double z) {
	dvec3 v; v.x = x; v.y = y; v.z = z;
	return v;
}
/* compatibility with vec.h */
inline vec3 v3(dvec3 dv) {
	vec3 v;
	v.x = (float)dv.x;
	v.y = (float)dv.y;
	v.z = (float)dv.z;
	return v;
}

inline dvec3 operator+(dvec3 v0, dvec3 v1) {
	dvec3 result;
	result.x = v0.x + v1.x;
	result.y = v0.y + v1.y;
	result.z = v0.z + v1.z;
	return result;
}
inline dvec3 operator-(dvec3 v0, dvec3 v1) {
	dvec3 result;
	result.x = v0.x - v1.x;
	result.y = v0.y - v1.y;
	result.z = v0.z - v1.z;
	return result;
}
inline dvec3 operator*(double f, dvec3 v) {
	dvec3 result;
	result.x = f * v.x;
	result.y = f * v.y;
	result.z = f * v.z;
	return result;
}
inline dvec3 operator/(dvec3 v, double d) {
	dvec3 result;
	result.x = v.x / d;
	result.y = v.y / d;
	result.z = v.z / d;
	return result;
}

inline double dot(dvec3 v0, dvec3 v1) {
	return v0.x*v1.x + v0.y*v1.y + v0.z*v1.z;
}

inline dvec3 cross(dvec3 v0, dvec3 v1) {
	dvec3 result;
	result.x = v0.y*v1.z - v0.z*v1.y;
	result.y = v0.z*v1.x - v0.x*v1.z;
	result.z = v0.x*v1.y - v0.y*v1.x;
	return result;
}

inline float lengthSqr(dvec3 v) {
	return dot(v, v);
}

inline float length(dvec3 v) {
	return sqrtf(lengthSqr(v));
}

inline dvec3 normalize(dvec3 v) {
	double len = length(v);
	if (fequal(len, 0.0f)) return dv3(0.0f); // avoid infinity
	return v/len;
}