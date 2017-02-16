#include "video/camera.h"

void Camera::updateProjectionMatrix() {
	float near = 0.1f;
	float far = 100.0f;
	float top = tanf(0.5f * field_of_view) * near;
	float right = top * aspect_ratio;
	proj_mat = makeFrustum(-right, right, -top, top, near, far);
}

void Camera::updateViewMatrix() {
	mat3 z_up = rotationMatrix(v3(1.0f, 0.0f, 0.0f), -0.5f * (float)M_PI);
	mat3 rot_x = rotationMatrix(v3(1.0f, 0.0f, 0.0f), euler_angles.x);
	mat3 rot_y = rotationMatrix(v3(0.0f, 0.0f, 1.0f), euler_angles.y);
	mat3 rot_z = rotationMatrix(v3(0.0f, 1.0f, 0.0f), euler_angles.z);
	mat3 rot = z_up * rot_z * rot_x * rot_y;
	view_mat = m4(rot) * translationMatrix(-location);
}

mat4 Camera::makeInverseViewMatrix() {
	mat3 z_up = rotationMatrix(v3(1.0f, 0.0f, 0.0f), 0.5f * (float)M_PI);
	mat3 rot_x = rotationMatrix(v3(1.0f, 0.0f, 0.0f), -euler_angles.x);
	mat3 rot_y = rotationMatrix(v3(0.0f, 0.0f, 1.0f), -euler_angles.y);
	mat3 rot_z = rotationMatrix(v3(0.0f, 1.0f, 0.0f), -euler_angles.z);
	mat3 rot = rot_y * rot_x * rot_z * z_up;
	return translationMatrix(location) * m4(rot);
}

vec3 Camera::makeViewRay(int screen_x, int screen_y, VideoMode video) {
	float tfov = tanf(0.5f * field_of_view);
	return transpose(m3(view_mat)) * v3(
		(2.0f * (float)screen_x / (float)video.width - 1.0f) * aspect_ratio * tfov,
		(1.0f - 2.0f * (float)screen_y / (float)video.height) * tfov,
		-1.0f
	);
}
