void FrameTime::init() {
	ticks_counter = 0;
	frame_counter = 0;
	frames_per_second = 0;
	frame_time = 0.0;

	one_second = SDL_GetPerformanceFrequency();
	last_ticks = SDL_GetPerformanceCounter();
	smoothed_frame_time = 1.0 / 60.0;
}

void FrameTime::update() {
	frame_counter++;

	u64 ticks = SDL_GetPerformanceCounter();
	u64 frame_ticks = ticks - last_ticks;
	last_ticks = ticks;

	frame_time = (double)frame_ticks / (double)one_second;
	smoothed_frame_time += 0.5 * (frame_time - smoothed_frame_time);

	ticks_counter += frame_ticks;
	if (ticks_counter >= one_second) {
		frames_per_second = frame_counter;
		frame_counter = 0;
		ticks_counter = 0;
	}
}
