// TODO: use modulo for arbitrary values
// wraps value to [-pi, pi]
float wrapMPi(float angle) {
	if (angle >  (float)M_PI) return angle - 2.0f * (float)M_PI;
	if (angle < -(float)M_PI) return angle + 2.0f * (float)M_PI;
	return angle;
}

float mixAngles(float v0, float v1, float a) {
	float delta = wrapMPi(v1 - v0);
	return wrapMPi(v0 + a*delta);
}
