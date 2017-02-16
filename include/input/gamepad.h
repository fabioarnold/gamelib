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
};

/* axis helper functions */

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
