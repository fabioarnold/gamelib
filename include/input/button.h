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
