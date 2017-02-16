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

float mixAngles(float v0, float v1, float a) {
	float delta = wrapMPi(v1 - v0);
	return wrapMPi(v0 + a*delta);
}
