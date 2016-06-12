void RigidBody::init(vec3 p, vec3 s) {
	size = s;
	mass = 1.0f;

	position = p;
	velocity = v3(0.0f);
	orientation = rotationMatrix(normalize(v3(1.0f)), 0.25f * (float)M_PI);
	q_orientation = quaternion(normalize(v3(1.0f)), 0.25f * (float)M_PI);
	orientation = m3(1.0f);
	q_orientation = quaternion(1.0f, 0.0f, 0.0f, 0.0f);

	angular_momentum = v3(0.0f);
	angular_velocity = v3(0.0f);

	float x2 = s.x * s.x;
	float y2 = s.y * s.y;
	float z2 = s.z * s.z;

	inv_inertia_mat = m3(0.0f);
	inv_inertia_mat[0] = 3.0f / (mass*(y2+z2));
	inv_inertia_mat[4] = 3.0f / (mass*(x2+z2));
	inv_inertia_mat[8] = 3.0f / (mass*(x2+y2));
}

mat4 RigidBody::getModelMatrix() {
	//return translationMatrix(position)*m4(orientation*scaleMatrix(size));
	return m4(position, q_orientation, size);
}

Transform RigidBody::getTransform() {
	Transform result;
	result.translation = position;
	result.rotation = q_orientation;
	result.scale = size;
	return result;
}

void RigidBody::getVertices(vec3 *vertices) {
	mat4 m = getModelMatrix();
	for (int i = 0; i < 8; i++) {
		vec3 v = v3((i&4)?1.0f:-1.0f, (i&2)?1.0f:-1.0f, (i&1)?1.0f:-1.0f);
		vertices[i] = v3(m*v4(v, 1.0f));
	}
}

void RigidBody::applyForce(vec3 p, vec3 f) {
	float inv_mass = 1.0f/mass;
	force += inv_mass * f;
	torque += cross(p-position, inv_mass * f);
}

// uses simple explicit euler integration
void RigidBody::integrate(float delta_time) {
	//printf("integrate %f\n", delta_time);
	float inv_mass = 1.0f/mass;

	position += delta_time * velocity;
	#if 0
	mat3 skew_mat = transpose(skewSymmetric(angular_velocity));
	orientation = orientation + delta_time * (skew_mat*orientation);
	orientation = orthoNormalize(orientation);
	#endif

	// integrate quaternion orientation
	quat spin = 0.5f * quaternion(0.0f, angular_velocity.x, angular_velocity.y, angular_velocity.z) * q_orientation;
	q_orientation += delta_time * spin;
	q_orientation = normalize(q_orientation);

	velocity += /*0.5f */ (delta_time*inv_mass) * force;
	angular_momentum += /*0.5f */ delta_time * torque;

	// compute auxiliaries
	#if 1
	orientation = m3(q_orientation);
	inv_world_inertia_mat = orientation * inv_inertia_mat * transpose(orientation);
	angular_velocity = inv_world_inertia_mat * angular_momentum;
	#else
	inv_world_inertia_mat = inv_inertia_mat;
	#endif

	angular_velocity = inv_world_inertia_mat * angular_momentum;

	force = v3(0.0f);
	torque = v3(0.0f);
}

void RigidBody::collide(RigidBody *o) {
	// TODO: implement
}
