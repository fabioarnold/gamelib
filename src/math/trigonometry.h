// wraps value to [-pi, pi]
float wrapMPi(float angle) {
	static const float pi = (float)M_PI;
	if (angle > 0.0f) {
		return fmodf(angle + pi, 2.0f * pi) - pi;
	} else {
		return fmodf(angle - pi, 2.0f * pi) + pi;
	}
}

 // [-pi, pi]
float angleFromDir(vec2 dir) {
	return atan2f(dir.y, dir.x);
}

vec2 dirFromAngle(float angle) {
	return v2(cosf(angle), sinf(angle));
}

float mixAngles(float a0, float a1, float alpha) {
	float delta = wrapMPi(a1 - a0);
	return wrapMPi(a0 + alpha*delta);
}

bool areAnglesEqual(float a0, float a1, float epsilon) {
	float delta = wrapMPi(a1 - a0);
	return fabsf(delta) < epsilon;
}
