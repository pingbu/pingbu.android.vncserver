#include "InputEvent.h"

#pragma once

class KeyEvent: InputEvent {
public:
	KeyEvent(const char* name) :
			InputEvent(name) {
	}

	void injectEvent(int scanCode, bool down) {
		injectInputEvent(EV_KEY, scanCode, down ? 1 : 0);
		injectInputEvent(EV_SYN, SYN_REPORT, 0);
	}
};
