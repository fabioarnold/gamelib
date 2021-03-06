struct ButtonState {
	// call this at beginning of frame before event handling
	void beginFrame() {_transition_count = 0;}
	// call this during event handling
	void setState(bool down) {
		if (down != _down) {
			_transition_count++;
		}
		_down = down;
	}
	// fake a click
	void simulateClick() {
		if (_down) {
			_transition_count = 1;
		} else {
			_transition_count = 2;
		}
	}

	// true if button is currently down
	bool down() {
		return _down;
	}

	// detects up->down edge
	// true if button was just pressed or was down in between polls
	bool clicked() {
		if (_down) {
			// clicked in this frame?
			return _transition_count > 0;
		} else {
			// button was pressed and released between frames (polls)
			return _transition_count > 1;
		}
	}

	// detects down->up edge
	bool released() {
		if (!_down) {
			// released in this frame?
			return _transition_count > 0;
		} else {
			// button was released and pressed between polling (full transition)
			return _transition_count > 1;
		}
	}

	// detects up->down edge
	// button was pressed down (like clicked but with intention to hold button)
	// ignores clicks in between polls
	bool pressed() {
		return _down && _transition_count > 0;
	}

	// button is held down or was held down between polling
	bool held() {
		return _down || _transition_count > 1;
	}

private:
	bool _down = false; // current state
	int _transition_count = 0; // counts the transitions between up and down
};

struct Mouse {
	void beginFrame() {
		for (size_t i = 0; i < ARRAY_COUNT(buttons); i++) {
			buttons[i].beginFrame();
		}
		old_x = x;
		old_y = y;
		rel_x = 0;
		rel_y = 0;
		wheel_x = 0;
		wheel_y = 0;
	}

	ButtonState& left()   {return buttons[0];}
	ButtonState& right()  {return buttons[1];}
	ButtonState& middle() {return buttons[2];}

	int x = 0, y = 0;
	int old_x = 0, old_y = 0;
	float rel_x = 0, rel_y = 0;
	ButtonState buttons[3];
	int wheel_x = 0, wheel_y = 0;
	bool is_relative = false; // mouse is in relative mode (no absolute movement)
} mouse; // currently only one mouse

struct MultitouchGesture {
	float center_x, center_y; // range [0; 1], last center point
	float delta_angle; // in radians (rotating)
	float delta_distance; // distance pinched (zooming)

	void beginFrame() {
		delta_angle = 0.0f;
		delta_distance = 0.0f;
	}
} multitouch_gesture;

struct Keyboard {
	void beginFrame() {
		for (size_t i = 0; i < ARRAY_COUNT(keys); i++) {
			keys[i].beginFrame();
			if (bindings[i]) bindings[i]->beginFrame();
		}
		text_input[0] = '\0';
	}
	void bind(int key_code, ButtonState *virt_button) {
		assert(key_code >= 0 && key_code < (int)ARRAY_COUNT(keys));
		bindings[key_code] = virt_button;
	}
	// call this on key event
	void onKey(int key_code, bool key_state) {
		if (key_code >= 0 && key_code < (int)ARRAY_COUNT(keys)) {
			keys[key_code].setState(key_state);
			if (bindings[key_code]) bindings[key_code]->setState(key_state);
		}
	}

	const char *getKeyName(int key_code); // to be implemented by platform specific code
	int getKeyCode(const char *name);

	ButtonState keys[512];

	char text_input[32]; // within a frame

private:
	ButtonState *bindings[512];
} keyboard; // only one keyboard

struct Gamepad {
	bool plugged_in = false;
	bool active = false; // any button has been pressed
	const char *name;

	void beginFrame() {
		for (size_t i = 0; i < ARRAY_COUNT(button_bindings); i++) {
			if (button_bindings[i]) button_bindings[i]->beginFrame();
		}
	}

	void bindButton(int button_idx, ButtonState *virt_button) {
		assert(button_idx >= 0 && button_idx < (int)ARRAY_COUNT(button_bindings));
		button_bindings[button_idx] = virt_button;
	}

	// call this on joystick button event
	void onButton(int button_idx, bool button_state) {
		if (button_idx >= 0 && button_idx < (int)ARRAY_COUNT(button_bindings)) {
			if (button_bindings[button_idx]) {
				button_bindings[button_idx]->setState(button_state);
			} else {
				active = true;
			}
		}
	}

	void bindAxis(int axis_idx, float *virt_axis) {
		assert(axis_idx >= 0 && axis_idx < (int)ARRAY_COUNT(axis_bindings));
		axis_bindings[axis_idx] = virt_axis;
	}

	void onAxis(int axis_idx, float axis_value) {
		if (axis_idx >= 0 && axis_idx < (int)ARRAY_COUNT(axis_bindings)) {
			if (axis_bindings[axis_idx]) {
				*axis_bindings[axis_idx] = axis_value;
			}
		}
	}

private:
	float *axis_bindings[8];
	ButtonState *button_bindings[32];
} gamepads[8];

/* gamepad axis helper functions */

// normalized direction of axis, considers dead zone
vec2 getAxisDirection(vec2 axis, float dead_zone = 0.1f) {
	return length(axis) < dead_zone ? v2(0.0f) : normalize(axis);
}

// axis with length in range of 0.0f to 1.0f, considers dead zone
vec2 getAxisScaled(vec2 axis, float dead_zone = 0.1f) {
	float len = length(axis);
	if (len < dead_zone) return v2(0.0f);
	if (len > 1.0f) return axis / len; // normalized
	float scale = (len - dead_zone) / (1.0f - dead_zone);
	return scale * axis;
}

/* general input functions */

void inputInit() {
	memset(&mouse, 0, sizeof(mouse));
	memset(&multitouch_gesture, 0, sizeof(multitouch_gesture));
	memset(&keyboard, 0, sizeof(keyboard));
	memset(&gamepads, 0, sizeof(gamepads));
}

void inputBeginFrame() {
	mouse.beginFrame();
	multitouch_gesture.beginFrame();
	keyboard.beginFrame();
	for (size_t i = 0; i < ARRAY_COUNT(gamepads); i++) {
		gamepads[i].beginFrame();
	}
}
