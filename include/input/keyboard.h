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
};
