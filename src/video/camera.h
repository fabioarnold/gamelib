struct Camera {
	float aspect_ratio;
	float field_of_view;
	mat4 proj_mat;

	vec3 location;
	vec3 euler_angles;
	mat4 view_mat;

	void updateProjectionMatrix();
	void updateViewMatrix();

	vec3 makeViewRay(int screen_x, int screen_y, VideoMode video);
};
