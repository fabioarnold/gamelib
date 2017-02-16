struct Mouse {
	void beginFrame() {
		for (size_t i = 0; i < ARRAY_COUNT(buttons); i++) {
			buttons[i].beginFrame();
		}
		rel_x = x - old_x;
		rel_y = y - old_y;
		old_x = x;
		old_y = y;
	}

	ButtonState& left()   {return buttons[0];}
	ButtonState& right()  {return buttons[1];}
	ButtonState& middle() {return buttons[2];}

	int x, y;
	int rel_x, rel_y;
	int old_x, old_y;
	ButtonState buttons[3];
	int wheel_x, wheel_y;
};
