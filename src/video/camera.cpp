void Camera::setPerspectiveProjection() {
	float top = tanf(0.5f * field_of_view) * near_plane;
	float right = top * aspect_ratio;
	proj_mat = makeFrustum(-right, right, -top, top, near_plane, far_plane);
}

void Camera::setOrthographicProjection(float width, float height) {
	float right = 0.5f * width, top = 0.5f * height;
	proj_mat = makeOrtho(-right, right, -top, top, near_plane, far_plane);
}

void Camera::updateRotationMatrix() {
	//mat3 z_up  = rotationMatrix(v3(1.0f, 0.0f, 0.0f), -0.5f * (float)M_PI);
	mat3 rot_x = rotationMatrix(v3(1.0f, 0.0f, 0.0f), euler_angles.x);
	mat3 rot_y = rotationMatrix(v3(0.0f, 0.0f, 1.0f), euler_angles.y);
	mat3 rot_z = rotationMatrix(v3(0.0f, 1.0f, 0.0f), euler_angles.z);
	rot_mat = /*z_up **/ rot_z * rot_x * rot_y;
}

vec3 Camera::makeViewRay(int screen_x, int screen_y, VideoMode video) {
	float tfov = tanf(0.5f * field_of_view);
	return transpose(m3(view_mat)) * v3(
		(2.0f * (float)screen_x / (float)video.width - 1.0f) * aspect_ratio * tfov,
		(1.0f - 2.0f * (float)screen_y / (float)video.height) * tfov,
		-1.0f
	);
}

vec2 Camera::getProjectionExtents() {
	vec2 extents;
	extents.y = near_plane * tanf(0.5f * field_of_view);
	extents.x = extents.y * aspect_ratio;
	return extents;
}
