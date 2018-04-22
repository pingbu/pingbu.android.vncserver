#include "InputEvent.h"

#pragma once

class TouchEvent: InputEvent {
	bool mTouchDown;
	int mCursorX, mCursorY, mTrackId;

public:
	TouchEvent(const char* name) :
			InputEvent(name) {
		mTouchDown = false;
		mCursorX = mCursorY = -1;
		mTrackId = 1;
	}

	void injectEvent(int x, int y, bool down) {
		bool hasEvent = false;
		if (down) {
			if (down != mTouchDown)
				InputEvent::injectInputEvent(EV_ABS, ABS_MT_TRACKING_ID,
						mTrackId++);
			if (x != mCursorX) {
				mCursorX = x;
				InputEvent::injectInputEvent(EV_ABS, ABS_MT_POSITION_X, x);
				hasEvent = true;
			}
			if (y != mCursorY) {
				mCursorY = y;
				InputEvent::injectInputEvent(EV_ABS, ABS_MT_POSITION_Y, y);
				hasEvent = true;
			}
			if (down != mTouchDown)
				InputEvent::injectInputEvent(EV_KEY, BTN_TOUCH, 1);
		} else {
			if (down != mTouchDown) {
				InputEvent::injectInputEvent(EV_ABS, ABS_MT_TRACKING_ID, -1);
				mCursorX = mCursorY = -1;
				InputEvent::injectInputEvent(EV_KEY, BTN_TOUCH, 0);
			}
		}
		if (down != mTouchDown) {
			mTouchDown = down;
			hasEvent = true;
		}
		if (hasEvent)
			InputEvent::injectInputEvent(EV_SYN, SYN_REPORT, 0);
	}
};
