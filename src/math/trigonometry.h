// TODO: use modulo for arbitrary values
// wraps value to [-pi, pi]
float wrapMPi(float angle) {
	if (angle >  (float)M_PI) return angle - 2.0f * (float)M_PI;
	if (angle < -(float)M_PI) return angle + 2.0f * (float)M_PI;
	return angle;
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
