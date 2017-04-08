struct Camera {
	float aspect_ratio;
	float field_of_view;
	mat4 proj_mat;

	vec3 location;
	vec3 euler_angles;

	mat3 rot_mat;
	mat4 view_mat;

	mat4 view_proj_mat;

	void setPerspectiveProjection(float near_plane, float far_plane);
	void setOrthographicProjection(VideoMode video, float near_plane, float far_plane);
	void updateRotationMatrix(); // euler to rot_mat
	void updateViewMatrix() { view_mat = m4(rot_mat) * translationMatrix(-location); }
	void updateViewProjectionMatrix() { view_proj_mat = proj_mat * view_mat; }

	vec3 makeViewRay(int screen_x, int screen_y, VideoMode video);

	mat3 getInverseRotationMatrix() { return transpose(rot_mat); }
	mat4 getInverseViewMatrix() { return translationMatrix(location) * m4(getInverseRotationMatrix()); }
};
