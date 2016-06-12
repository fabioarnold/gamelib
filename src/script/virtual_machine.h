struct VirtualMachine {
public:
	VirtualMachine();
	void reset(); // reset to initial execution state

	// also resets execution state
	void setProgram(ByteBuffer *program);
	void tick(float delta_time);

	void pause();
	void resume();

	void execute();

	bool executionStopped(); // true when program has ended

private:
	ByteBuffer *current_program;

	bool paused;
	int instruction_pointer;

	float sleep_time;
};
