struct Contact {
	vec3 position;
	vec3 normal;
	float penetration;
};

struct RigidBody {
	vec3 size;
	float mass;
	mat3 inv_inertia_mat;

	vec3 position;
	quat q_orientation;

	vec3 velocity; // for center of mass
	vec3 angular_momentum;

	vec3 force;
	vec3 torque;

	bool sleeping;

	// aux
	mat3 orientation;
	mat3 inv_world_inertia_mat;
	vec3 angular_velocity;


	void applyForce(vec3 p, vec3 f);
	void integrate(float delta_time);
	void collide(RigidBody *o);

	void init(vec3 pos, vec3 size);

	Transform getTransform();
	mat4 getModelMatrix();
	void getVertices(vec3 *vertices);
};
