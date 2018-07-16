struct Planed;

struct Plane {
	vec3 normal;
	float d;

	float distanceTo(vec3 p) {
		return dot(normal, p) - d;
	}

	vec3 project(vec3 p) {
		return p - distanceTo(p)*normal;
	}

	// point position in half space { -1: inside, 0: on boundary, 1: outside }
	int classify(vec3 p, float epsilon = 0.0001f) {
		float distance = distanceTo(p);
		if (distance > epsilon) return 1;
		else if (distance < -epsilon) return -1;
		return 0;
	}

	bool intersectsRay(vec3 ro, vec3 rd, float *t, float epsilon = 0.0001f) {
		float denom = dot(normal, rd);
		if (fabsf(denom) > epsilon) { // not parallel
			*t = distanceTo(ro) / denom;
			if (*t >= 0.0f) return true;
		}
		return false;
	}

	Plane() {}
	Plane(Planed pd);
};

// ccw
Plane makePlane(vec3 p0, vec3 p1, vec3 p2) {
	Plane result;
	result.normal = normalize(cross(p1-p0, p2-p0));
	result.d = dot(result.normal, p0);
	return result;
}

struct Planed {
	dvec3 normal;
	double d;

	double distanceTo(dvec3 p) {
		return dot(normal, p) - d;
	}

	dvec3 project(dvec3 p) {
		return p - distanceTo(p)*normal;
	}

	// point position in half space { -1: inside, 0: on boundary, 1: outside }
	int classify(dvec3 p, double epsilon = 0.0001) {
		double distance = distanceTo(p);
		if (distance > epsilon) return 1;
		else if (distance < -epsilon) return -1;
		return 0;
	}

	bool intersectsRay(dvec3 ro, dvec3 rd, double *t, double epsilon = 0.0001) {
		double denom = dot(normal, rd);
		if (fabs(denom) > epsilon) { // not parallel
			*t = distanceTo(ro) / denom;
			if (*t >= 0.0) return true;
		}
		return false;
	}
};

// ccw
Planed makePlane(dvec3 p0, dvec3 p1, dvec3 p2) {
	Planed result;
	result.normal = normalize(cross(p1-p0, p2-p0));
	result.d = dot(result.normal, p0);
	return result;
}

Plane::Plane(Planed pd) {
	normal = v3(pd.normal);
	d = (float)pd.d;
}