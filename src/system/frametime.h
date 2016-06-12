struct FrameTime {
	u64 last_ticks;
	u64 ticks_counter;
	u64 one_second;
	int frame_counter;
	int frames_per_second;
	double frame_time;
	double smoothed_frame_time;

	void init();
	void update();
	void drawInfo();
} frametime;
