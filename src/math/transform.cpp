Transform Transform::identity() {
	Transform result;
	result.translation = v3(0.0f);
	result.rotation = quaternion(1.0f, 0.0f, 0.0f, 0.0f);
	result.scale = v3(1.0f);
	return result;
}

mat4 Transform::getMatrix() {
	return m4(translation, rotation, scale);
}
