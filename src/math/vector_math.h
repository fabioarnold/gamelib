/*
A small collection of simple template-free vector math functions
with the goal of reasonably low compile times

Matrix storage order is column-major:
0 4 8 C
1 5 9 D
2 6 A E
3 7 B F

Reasons for using this order:
- OpenGL and glm use this convention
- There is no need to transpose matrices before using them as uniform
- matrix-vector multiplication uses less operations
Contra:
- human-readable access pattern is inefficient (row-major)
*/

union vec2 {
	struct {float x, y;};
	float e[2];

	inline float& operator[](int index) {
		assert(index >= 0 && index < 2);
		return e[index];
	}
};

union vec3 {
	struct {float x, y, z;};
	struct {float r, g, b;};
	float e[3];

	inline float& operator[](int index) {
		assert(index >= 0 && index < 3);
		return e[index];
	}
};

union vec4 {
	struct {float x, y, z, w;};
	struct {float r, g, b, a;};
	float e[4];

	inline float& operator[](int index) {
		assert(index >= 0 && index < 4);
		return e[index];
	}
};

typedef vec4 quat;

union mat2 {
	float e[4];
	vec2 cols[2];

	inline float& operator[](int index) {
		assert(index >= 0 && index < 4);
		return e[index];
	}
};

union mat3 {
	float e[9];
	vec3 cols[3];

	inline float& operator[](int index) {
		assert(index >= 0 && index < 9);
		return e[index];
	}
};

union mat4 {
	float e[16];
	vec4 cols[4];

	inline float& operator[](int index) {
		assert(index >= 0 && index < 16);
		return e[index];
	}
};

inline vec2 v2(float x, float y) {
	vec2 v; v.x = x; v.y = y;
	return v;
}
inline vec2 v2(float x) {
	vec2 v; v.x = x; v.y = x;
	return v;
}
inline vec2 v2(vec3 v) {
	vec2 r; r.x = v.x; r.y = v.y;
	return r;
}


inline vec3 v3(float f) {
	vec3 v; v.x = f; v.y = f; v.z = f;
	return v;
}
inline vec3 v3(float x, float y, float z) {
	vec3 v; v.x = x; v.y = y; v.z = z;
	return v;
}
inline vec3 v3(vec2 v) {
	vec3 result;
	result.x = v.x;
	result.y = v.y;
	result.z = 0.0f;
	return result;
}
inline vec3 v3(vec2 v, float z) {
	vec3 result;
	result.x = v.x;
	result.y = v.y;
	result.z = z;
	return result;
}
inline vec3 v3(vec4 v) {
	vec3 result;
	result.x = v.x;
	result.y = v.y;
	result.z = v.z;
	return result;
}

inline vec4 v4(float f) {
	vec4 v; v.x = f; v.y = f; v.z = f; v.w = f;
	return v;
}
inline vec4 v4(float x, float y, float z, float w) {
	vec4 v; v.x = x; v.y = y; v.z = z; v.w = w;
	return v;
}
inline vec4 v4(vec3 v, float w) {
	vec4 result;
	result.x = v.x;
	result.y = v.y;
	result.z = v.z;
	result.w = w;
	return result;
}

mat3 m3(float f) {
	mat3 result;
	result.cols[0] = v3(f, 0.0f, 0.0f);
	result.cols[1] = v3(0.0f, f, 0.0f);
	result.cols[2] = v3(0.0f, 0.0f, f);
	return result;
}
mat3 m3(mat4 m) {
	mat3 result;
	result.cols[0] = v3(m.cols[0][0], m.cols[0][1], m.cols[0][2]);
	result.cols[1] = v3(m.cols[1][0], m.cols[1][1], m.cols[1][2]);
	result.cols[2] = v3(m.cols[2][0], m.cols[2][1], m.cols[2][2]);
	return result;
}
mat3 m3(quat q) {
	float qxx = q.x*q.x, qyy = q.y*q.y, qzz = q.z*q.z;
	float qxz = q.x*q.z, qxy = q.x*q.y, qyz = q.y*q.z;
	float qwx = q.w*q.x, qwy = q.w*q.y, qwz = q.w*q.z;
	mat3 result;
	result.cols[0] = v3(1.0f - 2.0f*(qyy+qzz), 2.0f*(qxy+qwz), 2.0f*(qxz-qwy));
	result.cols[1] = v3(2.0f*(qxy-qwz), 1.0f - 2.0f*(qxx+qzz), 2.0f*(qyz+qwx));
	result.cols[2] = v3(2.0f*(qxz+qwy), 2.0f*(qyz-qwx), 1.0f - 2.0f*(qxx+qyy));
	return result;
}

mat4 m4(float f) {
	mat4 result;
	result.cols[0] = v4(f, 0.0f, 0.0f, 0.0f);
	result.cols[1] = v4(0.0f, f, 0.0f, 0.0f);
	result.cols[2] = v4(0.0f, 0.0f, f, 0.0f);
	result.cols[3] = v4(0.0f, 0.0f, 0.0f, f);
	return result;
}
mat4 m4(mat3 m) {
	mat4 result;
	result.cols[0] = v4(m.cols[0][0], m.cols[0][1], m.cols[0][2], 0.0f);
	result.cols[1] = v4(m.cols[1][0], m.cols[1][1], m.cols[1][2], 0.0f);
	result.cols[2] = v4(m.cols[2][0], m.cols[2][1], m.cols[2][2], 0.0f);
	result.cols[3] = v4(0.0f, 0.0f, 0.0f, 1.0f);
	return result;
}

inline vec2 operator+(vec2 v0, vec2 v1) {
	vec2 result;
	result.x = v0.x + v1.x;
	result.y = v0.y + v1.y;
	return result;
}
void operator+=(vec2 &v0, vec2 v1) {
	v0.x += v1.x;
	v0.y += v1.y;
}
inline vec2 operator-(vec2 v0, vec2 v1) {
	vec2 result;
	result.x = v0.x - v1.x;
	result.y = v0.y - v1.y;
	return result;
}
void operator-=(vec2 &v0, vec2 v1) {
	v0.x -= v1.x;
	v0.y -= v1.y;
}
inline vec2 operator*(float f, vec2 v) {
	vec2 result;
	result.x = f * v.x;
	result.y = f * v.y;
	return result;
}
inline vec2 operator*(vec2 v, float f) {
	return f*v;
}
void operator*=(vec2 &v, float f) {
	v.x *= f;
	v.y *= f;
}
inline vec2 operator/(vec2 v, float d) {
	vec2 result;
	result.x = v.x / d;
	result.y = v.y / d;
	return result;
}

inline vec2 operator-(vec2 v) {
	vec2 result;
	result.x = -v.x;
	result.y = -v.y;
	return result;
}

inline vec3 operator-(vec3 v) {
	vec3 result;
	result.x = -v.x;
	result.y = -v.y;
	result.z = -v.z;
	return result;
}

inline vec3 operator+(vec3 v0, vec3 v1) {
	vec3 result;
	result.x = v0.x + v1.x;
	result.y = v0.y + v1.y;
	result.z = v0.z + v1.z;
	return result;
}

inline vec3 operator-(vec3 v0, vec3 v1) {
	vec3 result;
	result.x = v0.x - v1.x;
	result.y = v0.y - v1.y;
	result.z = v0.z - v1.z;
	return result;
}

inline void operator+=(vec3 &v0, vec3 v1) {
	v0.x += v1.x;
	v0.y += v1.y;
	v0.z += v1.z;
}

inline void operator-=(vec3 &v0, vec3 v1) {
	v0.x -= v1.x;
	v0.y -= v1.y;
	v0.z -= v1.z;
}

inline vec4 operator+(vec4 v0, vec4 v1) {
	vec4 result;
	result.x = v0.x + v1.x;
	result.y = v0.y + v1.y;
	result.z = v0.z + v1.z;
	result.w = v0.w + v1.w;
	return result;
}

inline void operator+=(vec4 &v0, vec4 v1) {
	v0.x += v1.x;
	v0.y += v1.y;
	v0.z += v1.z;
	v0.w += v1.w;
}

inline void operator-=(vec4 &v0, vec4 v1) {
	v0.x -= v1.x;
	v0.y -= v1.y;
	v0.z -= v1.z;
	v0.w -= v1.w;
}

inline vec3 operator*(float f, vec3 v) {
	vec3 result;
	result.x = f * v.x;
	result.y = f * v.y;
	result.z = f * v.z;
	return result;
}
inline vec3 operator*(vec3 v, float f) {
	return f*v;
}
void operator*=(vec3 &v, float f) {
	v.x *= f;
	v.y *= f;
	v.z *= f;
}
inline vec3 operator/(vec3 v, float d) {
	vec3 result;
	result.x = v.x / d;
	result.y = v.y / d;
	result.z = v.z / d;
	return result;
}

inline vec4 operator*(float f, vec4 v) {
	vec4 result;
	result.x = f * v.x;
	result.y = f * v.y;
	result.z = f * v.z;
	result.w = f * v.w;
	return result;
}
inline vec4 operator*(vec4 v, float f) {
	return f*v;
}
void operator*=(vec4 &v, float f) {
	v.x *= f;
	v.y *= f;
	v.z *= f;
	v.w *= f;
}
inline vec4 operator/(vec4 v, float d) {
	vec4 result;
	result.x = v.x / d;
	result.y = v.y / d;
	result.z = v.z / d;
	result.w = v.w / d;
	return result;
}

inline float dot(vec2 v0, vec2 v1) {
	return v0.x*v1.x + v0.y*v1.y;
}

inline float dot(vec3 v0, vec3 v1) {
	return v0.x*v1.x + v0.y*v1.y + v0.z*v1.z;
}

inline float dot(vec4 v0, vec4 v1) {
	return v0.x*v1.x + v0.y*v1.y + v0.z*v1.z + v0.w*v1.w;
}

inline vec3 cross(vec3 v0, vec3 v1) {
	vec3 result;
	result.x = v0.y*v1.z - v0.z*v1.y;
	result.y = v0.z*v1.x - v0.x*v1.z;
	result.z = v0.x*v1.y - v0.y*v1.x;
	return result;
}

inline float lengthSqr(vec2 v) {
	return dot(v, v);
}

inline float lengthSqr(vec3 v) {
	return dot(v, v);
}

// from https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Reciprocal_of_the_square_root
// 16.01.2016
// returns huge number if x == 0.0f
float rsqrt(float x) {
	float y = x * 0.5f;
	int i = *reinterpret_cast<int *>(&x);
	i = 0x5f3759df - (i >> 1);
	float r = *reinterpret_cast<float *>(&i);
	r = r * (1.5f - r * r * y); // repeat this line to improve precision (Newton's method)
	return r;
}

#if 1
inline float length(vec2 v) {
	return sqrtf(lengthSqr(v));
}

inline float length(vec3 v) {
	return sqrtf(lengthSqr(v));
}
#else
inline float length(vec3 v) {
	return 1.0f/rsqrt(lengthSqr(v));
}
#endif

#if 1
inline vec2 normalize(vec2 v) {
	float len = length(v);
	if (fequal(len, 0.0f)) return v2(0.0f); // avoid infinity
	return v/len;
}

inline vec3 normalize(vec3 v) {
	float len = length(v);
	if (fequal(len, 0.0f)) return v3(0.0f); // avoid infinity
	return v/len;
}
#else
inline vec3 normalize(vec3 v) {
	return v*rsqrt(lengthSqr(v));
}
#endif

#if 1
inline float length(quat q) {
	return sqrtf(dot(q, q));
}
#else
inline float length(quat q) {
	return 1.0f/rsqrt(dot(q, q));
}
#endif

#if 1
inline quat normalize(quat q) {
	return q/length(q);
}
#else
inline quat normalize(quat q) {
	return q*rsqrt(dot(q, q));
}
#endif

vec2 operator*(mat2 m, vec2 v) {
	return m.cols[0]*v[0] + m.cols[1]*v[1];
}

mat3 transpose(mat3 m) {
	mat3 result;
	// TODO: unroll loop
	for (int i = 0; i < 3; i++) {
		result.cols[0][i] = m.cols[i][0];
		result.cols[1][i] = m.cols[i][1];
		result.cols[2][i] = m.cols[i][2];
	}
	return result;
}

mat3 operator+(mat3 m0, mat3 m1) {
	mat3 result;
	result.cols[0] = m0.cols[0]+m1.cols[0];
	result.cols[1] = m0.cols[1]+m1.cols[1];
	result.cols[2] = m0.cols[2]+m1.cols[2];
	return result;
}

mat3 operator*(float f, mat3 m) {
	mat3 result;
	result.cols[0] = f*m.cols[0];
	result.cols[1] = f*m.cols[1];
	result.cols[2] = f*m.cols[2];
	return result;
}
mat3 operator*(mat3 m, float f) {
	return f*m;
}

vec3 operator*(mat3 m, vec3 v) {
	return m.cols[0]*v[0] + m.cols[1]*v[1] + m.cols[2]*v[2];
}

mat3 operator*(mat3 m0, mat3 m1) {
	mat3 result;
	// TODO: unroll loop
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 3; col++) {
			vec3 m0row = v3(m0.cols[0][row], m0.cols[1][row], m0.cols[2][row]);
			result.cols[col][row] = dot(m0row, m1.cols[col]);
		}
	}
	return result;
}

vec4 operator*(mat4 m, vec4 v) {
	return m.cols[0]*v[0] + m.cols[1]*v[1] + m.cols[2]*v[2] + m.cols[3]*v[3];
}

mat4 operator*(mat4 m0, mat4 m1) {
	mat4 result;
	// TODO: unroll loop
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			vec4 m0row = v4(m0.cols[0][row], m0.cols[1][row], m0.cols[2][row], m0.cols[3][row]);
			result.cols[col][row] = dot(m0row, m1.cols[col]);
		}
	}
	return result;
}

mat3 orthoNormalize(mat3 m) {
	mat3 result = m;
	result.cols[0] = normalize(m.cols[0]);

	float d0 = dot(result.cols[0], result.cols[1]);
	result.cols[1] -= result.cols[0] * d0;
	result.cols[1] = normalize(result.cols[1]);

	float d1 = dot(result.cols[1], result.cols[2]);
	d0 = dot(result.cols[0], result.cols[2]);
	result.cols[2] -= result.cols[0] * d0 + result.cols[1] * d1;
	result.cols[2] = normalize(result.cols[2]);

	return result;
}

#undef near
#undef far
mat4 makeFrustum(float left, float right, float bottom, float top, float near, float far) {
	mat4 result;
	result.cols[0] = v4(2.0f * near / (right-left), 0.0f, 0.0f, 0.0f);
	result.cols[1] = v4(0.0f, 2.0f * near / (top-bottom), 0.0f, 0.0f);
	result.cols[2] = v4((right+left) / (right - left), (top+bottom) / (top-bottom), -(far+near) / (far-near), -1.0f);
	result.cols[3] = v4(0.0f, 0.0f, -(2.0f * far * near) / (far-near), 0.0f);
	return result;
}

mat4 makeOrtho(float left, float right, float bottom, float top, float near, float far) {
	mat4 result;
	result.cols[0] = v4(2.0f / (right-left), 0.0f, 0.0f, 0.0f);
	result.cols[1] = v4(0.0f, 2.0f / (top-bottom), 0.0f, 0.0f);
	result.cols[2] = v4(0.0f, 0.0f, -2.0f / (far-near), 0.0f);
	result.cols[3] = v4(-(right+left)/(right-left), -(top+bottom)/(top-bottom), (far+near)/(far-near), 1.0f);
	return result;
}

mat3 rotationMatrix(vec3 axis, float angle) {
	float c = cosf(angle);
	float s = sinf(angle);
	vec3 tmp = (1.0f-c) * axis;

	mat3 result;
	result.cols[0] = v3(c + tmp[0]*axis[0], tmp[0]*axis[1] + s*axis[2], tmp[0]*axis[2] - s*axis[1]);
	result.cols[1] = v3(tmp[1]*axis[0] - s*axis[2], c + tmp[1]*axis[1], tmp[1]*axis[2] + s*axis[0]);
	result.cols[2] = v3(tmp[2]*axis[0] + s*axis[1], tmp[2]*axis[1] - s*axis[0], c + tmp[2]*axis[2]);
	return result;
}

mat3 scaleMatrix(vec3 scale) {
	mat3 result;
	result.cols[0] = v3(scale.x, 0.0f, 0.0f);
	result.cols[1] = v3(0.0f, scale.y, 0.0f);
	result.cols[2] = v3(0.0f, 0.0f, scale.z);
	return result;
}

mat4 translationMatrix(vec3 v) {
	mat4 result;
	result.cols[0] = v4(1.0f, 0.0f, 0.0f, 0.0f);
	result.cols[1] = v4(0.0f, 1.0f, 0.0f, 0.0f);
	result.cols[2] = v4(0.0f, 0.0f, 1.0f, 0.0f);
	result.cols[3] = v4(v.x, v.y, v.z, 1.0f);
	return result;
}

mat3 skewSymmetric(vec3 v) {
	mat3 result;
	result.cols[0] = v3(0.0f, -v.z, v.y);
	result.cols[1] = v3(v.z, 0.0f, -v.x);
	result.cols[2] = v3(-v.y, v.x, 0.0f);
	return result;
}

void printMatrix(mat3 m) {
#ifdef DEBUG
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 3; col++) {
			printf("%.3f ", (double)m.cols[col][row]);
		}
		puts("");
	}
#endif
}
void printMatrix(mat4 m) {
#ifdef DEBUG
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			printf("%.3f ", (double)m.cols[col][row]);
		}
		puts("");
	}
#endif
}

/*
quaternion stuff!!
ii=jj=kk=ijk = -1
jk = i
ki = j
ij = k
kj = -i
ik = -j
ji = -k

mult derivation:
q0 * q1 = (q0.w1 + q0.xi + q0.yj + q0.zk) * (q1.w1 + q1.xi + q1.yj + q1.zk)

  q0.w*q1.w*11 + q0.w*q1.x*1i + q0.w*q1.y*1j + q0.w*q1.z*1k
+ q0.x*q1.w*i1 + q0.x*q1.x*ii + q0.x*q1.y*ij + q0.x*q1.z*ik
+ q0.y*q1.w*j1 + q0.y*q1.x*ji + q0.y*q1.y*jj + q0.y*q1.z*jk
+ q0.z*q1.w*k1 + q0.z*q1.x*ki + q0.z*q1.y*kj + q0.z*q1.z*kk

1  i  j  k
i -1  k -j
j -k -1  i
k  j -i -1

mat3 -> quat
For a pure rotation, that is where:

the matrix is orthogonal
the matrix is special orthogonal which gives additional condition: det(matrix)= +1
Then the matrix can be converted to a quaternion using this basic form:

qw= √(1 + m00 + m11 + m22) /2
qx = (m21 - m12)/( 4 *qw)
qy = (m02 - m20)/( 4 *qw)
qz = (m10 - m01)/( 4 *qw)
*/

// TODO: think of a better (shorter) name
quat quaternion() { // identity
	quat result;
	result.x = 0.0f;
	result.y = 0.0f;
	result.z = 0.0f;
	result.w = 1.0f;
	return result;
}
quat quaternion(float w, float x, float y, float z) {
	quat result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.w = w;
	return result;
}
quat quaternion(vec3 axis, float angle) {
	float c = cosf(0.5f*angle);
	float s = sinf(0.5f*angle);
	return quaternion(c, s*axis.x, s*axis.y, s*axis.z);
}
quat quaternion(vec3 point) {
	return quaternion(0.0f, point.x, point.y, point.z);
}

inline quat conjugate(quat q) {
	return quaternion(q.w, -q.x, -q.y, -q.z);
}

inline vec3 real(quat q) {
	return v3(q.x, q.y, q.z);
}

inline quat operator*(quat q0, quat q1) {
	quat result;
	result.w = q0.w*q1.w - q0.x*q1.x - q0.y*q1.y - q0.z*q1.z;
	result.x = q0.w*q1.x + q0.x*q1.w + q0.y*q1.z - q0.z*q1.y;
	result.y = q0.w*q1.y - q0.x*q1.z + q0.y*q1.w + q0.z*q1.x;
	result.z = q0.w*q1.z + q0.x*q1.y - q0.y*q1.x + q0.z*q1.w;
	return result;
}

inline vec3 operator*(quat q, vec3 v) {
	return real(q * quaternion(v) * conjugate(q));
}

mat4 m4(vec3 translation, quat rotation, vec3 scale) {
	// TODO: optimize
	mat4 result;
	result = m4(m3(rotation));
	result.cols[0] *= scale.x;
	result.cols[1] *= scale.y;
	result.cols[2] *= scale.z;
	result.cols[3] = v4(translation.x, translation.y, translation.z, 1.0);
	return result;
}

quat rotateTo(vec3 dir, vec3 target_dir) {
	vec3 axis = normalize(cross(dir, target_dir));
	if (std::isnan(axis.x)) return quaternion();
	float angle = acosf(dot(dir, target_dir));
	return quaternion(axis, angle);
}

vec2 reflect(vec2 v, vec2 n) {
	return v - 2.0f * dot(n, v) * n;
}

vec3 reflect(vec3 v, vec3 n) {
	return v - 2.0f * dot(n, v) * n;
}

float mix(float v0, float v1, float a) {
	return (1.0f-a)*v0 + a*v1;
}

vec2 mix(vec2 v0, vec2 v1, float a) {
	return (1.0f-a)*v0 + a*v1;
}

vec3 mix(vec3 v0, vec3 v1, float a) {
	return (1.0f-a)*v0 + a*v1;
}

vec4 mix(vec4 v0, vec4 v1, float a) {
	return (1.0f-a)*v0 + a*v1;
}