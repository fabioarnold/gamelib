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
} mouse; // currently only one mouse

// only works with bindings
// bind specific key codes to virtual buttons (binary actions)
struct Keyboard {
	void beginFrame() {
		for (size_t i = 0; i < ARRAY_COUNT(bindings); i++) {
			if (bindings[i]) bindings[i]->beginFrame();
		}
	}
	void bind(int key_code, ButtonState *virt_button) {
		assert(key_code >= 0 && key_code < (int)ARRAY_COUNT(bindings));
		bindings[key_code] = virt_button;
	}
	// call this on key event
	void onKey(int key_code, bool key_state) {
		if (key_code >= 0 && key_code < (int)ARRAY_COUNT(bindings)
		 && bindings[key_code]) {
			bindings[key_code]->setState(key_state);
		}
	}

private:
	ButtonState *bindings[512]; // can be bound to any ButtonState
} keyboard;

#if 0
#define MAX_GAMEPAD_COUNT 4
struct Gamepad {
	bool available; // true when plugged in

	int num_buttons;

	vec2 sticks[2];
	ButtonState buttons[12];
} gamepads[MAX_GAMEPAD_COUNT];
#endif
