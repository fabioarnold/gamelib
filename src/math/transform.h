// standard srt transform
struct Transform {
	vec3 translation;
	quat rotation;
	vec3 scale;

	static Transform identity();
	mat4 getMatrix();
};
