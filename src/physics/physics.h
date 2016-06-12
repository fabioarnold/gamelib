struct Physics {
	RigidBody bodies[6];
	int body_count;

	Contact contacts[32];
	int contacts_count;

	void init();
	void simulate(float delta_time);
	int checkCollisions(int body_index);
	void resolveCollisions(int body_index);
};
