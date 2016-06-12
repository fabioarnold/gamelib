#if 0
// test quaternion
#include <glm/gtc/quaternion.hpp>
glm::quat glm_q0 = glm::quat(1.0f, 2.0f, 3.0f, 4.0f);
glm::quat glm_q1 = glm::quat(5.0f, 6.0f, 7.0f, 8.0f);
glm::quat glm_q2 = glm_q0 * glm_q1;
printf("glm %f %f %f %f\n", glm_q2.w, glm_q2.x, glm_q2.y, glm_q2.z);
glm_q2 = glm::normalize(glm_q2);
printf("glm %f %f %f %f normalized\n", glm_q2.w, glm_q2.x, glm_q2.y, glm_q2.z);
glm::mat3 glm_m = glm::mat3_cast(glm_q2);
printf("glm mat:\n");
for (int row = 0; row < 3; row++) {
	for (int col = 0; col < 3; col++) {
		printf("%f ", glm_m[col][row]);
	}
	printf("\n");
}

quat q0 = quaternion(1.0f, 2.0f, 3.0f, 4.0f);
quat q1 = quaternion(5.0f, 6.0f, 7.0f, 8.0f);
quat q2 = q0 * q1;
printf("res %f %f %f %f\n", q2.w, q2.x, q2.y, q2.z);
q2 = normalize(q2);
printf("nor %f %f %f %f\n", q2.w, q2.x, q2.y, q2.z);
mat3 m = m3(q2);
printf("mat:\n");
printMatrix(m);

// integrate orientation over time
q(t=0): orientation
q(t=dt): orientation


/*
differentiation of quaternions:
https://fgiesen.wordpress.com/2012/08/24/quaternion-differentiation/
*/

q(t) = angular_velocity^t * orientation
spin = quaternion(0.0f, 0.5f*angular_velocity.xyz);
orientation_1 = spin * orientation_0

#endif

// [0, 1]
float randf() {
	return (float)rand() / (float)RAND_MAX;
}

void Physics::init() {
	// init bodies
	body_count = 1;
	for (int i = 0; i < body_count; i++) {
		bodies[i].init(v3(-1.0f + i*2.0f, 0.0f, 0.0f + i*1.0f), v3(0.5f+0.5f*randf(), 0.5f+0.5f*randf(), 0.5f+0.5f*randf()));
	}
}

int Physics::checkCollisions(int body_index) {
	RigidBody *b = bodies+body_index;

	contacts_count = 0;
#if 0
	for (int i = 0; i < body_count; i++) {
		if (i == body_index) continue;
		b->collide(bodies+i);
	}
#endif

	float depth_epsilon = 0.001f;

	vec3 vertices[8];
	b->getVertices(vertices);
	for (int i = 0; i < 8; i++) {
		vec3 u = vertices[i] - b->position;
		vec3 velocity = b->velocity + cross(b->angular_velocity, u);
		if (vertices[i].z < 0.0f-depth_epsilon) {
			return 1; // penetration
		} else if (vertices[i].z < 0.0f+depth_epsilon) {
			if (dot(velocity, v3(0.0f, 0.0f, 1.0f)) < 0.0f) {
				contacts[contacts_count].position = vertices[i];
				contacts[contacts_count].normal = v3(0.0f, 0.0f, 1.0f);
				contacts[contacts_count].penetration = vertices[i].z;
				contacts_count++;
			}
		}
	}
	return 0;
}

void Physics::resolveCollisions(int body_index) {
	RigidBody *b = bodies+body_index;
	float coeff_restitution = 0.2f;
	float coeff_friction = 0.5f;

	for (int ci = 0; ci < 1; ci++) {
		Contact *c = contacts+ci;
		float inv_mass = 1.0f/b->mass;

		vec3 r = c->position - b->position;
		vec3 velocity = b->velocity + cross(b->angular_velocity, r);

		float numerator = -(1.0f + coeff_restitution) * dot(velocity, c->normal);
		float denominator = inv_mass + dot(cross(b->inv_world_inertia_mat * cross(r, c->normal), r), c->normal);
		float ratio = numerator/denominator;
		if (ratio < 0.0f) {LOGW("physics: neg impulse\n"); ratio = 0.0f;}
		vec3 restitution_impulse = ratio * c->normal; // scale more appropriately

		vec3 friction_impulse = v3(0.0f);
		vec3 tangent = velocity - dot(velocity, c->normal)*c->normal;
		float length_squared = dot(tangent, tangent);
		if (length_squared > 0.00001f) {
			tangent = tangent / sqrtf(length_squared);
			numerator = -(coeff_friction) * dot(velocity, tangent);
			denominator = inv_mass + dot(cross(b->inv_world_inertia_mat * cross(r, tangent), r), tangent);
			float ratio = numerator/denominator;
			friction_impulse = ratio * tangent;
		}

		vec3 impulse = restitution_impulse+friction_impulse;

		b->velocity += inv_mass * impulse;
		b->angular_momentum += cross(r, impulse);

		b->angular_velocity = b->inv_world_inertia_mat*b->angular_momentum;
		//b->position.z -= c->penetration;
	}
}

void Physics::simulate(float delta_time) {
	vec3 gravity = v3(0.0f, 0.0f, -10.0f);

	for (int i = 0; i < body_count; i++) {
		RigidBody *b = bodies+i;

		float current_time = 0.0f;
		float target_time = delta_time;

		while (current_time < delta_time) {
			RigidBody backup;
			memcpy(&backup, b, sizeof(RigidBody));

			// compute forces
			b->applyForce(b->position, b->mass*gravity);
			b->force += -0.002f*b->velocity;
			b->torque += -0.001f*b->angular_velocity;

			b->integrate(target_time - current_time);

			// check collisions
			int state = checkCollisions(i);
			// check contacts
			// apply impulses

			if (state == 1) { // penetration > PENETRATION_THRESHOLD
				memcpy(b, &backup, sizeof(RigidBody)); // restore backup
				target_time = 0.5f * (current_time + target_time);
				//printf("%f\n", target_time);
				float epsilon = 0.00001f;
				if (fabs(target_time - current_time) <= epsilon) {
#ifdef DEBUG
					LOGE("physics fail lol\n");
					exit(0);
#else
					// desperation
					resolveCollisions(i);
					current_time = target_time;
					target_time = delta_time;
#endif
				}
				assert(fabs(target_time - current_time) > epsilon);
			} else {
				// resolve collsions
				while (contacts_count) {
					resolveCollisions(i);
					checkCollisions(i);
				}
				//printf("collisions resolved %d\n", state);
				current_time = target_time;
				target_time = delta_time;
			}
		}
	}
}
