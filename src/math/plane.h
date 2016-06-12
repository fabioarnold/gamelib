struct Plane {
	vec3 normal;
	float d;

	float distanceTo(vec3 p) {
		return dot(normal, p) - d;
	}

	vec3 project(vec3 p) {
		return p - distanceTo(p)*normal;
	}
};

// ccw
Plane makePlane(vec3 p0, vec3 p1, vec3 p2) {
	Plane result;
	result.normal = normalize(cross(p1-p0, p2-p0));
	result.d = dot(result.normal, p0);
	return result;
}
