#define LOG_TAG  "VncServer"

#include <jni.h>
#include <android/log.h>
#include <stdarg.h>

#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <sys/stat.h>
#include <sys/sysmacros.h>

#include <assert.h>
#include <errno.h>

#include "rfb/rfb.h"
#include "rfb/keysym.h"

#include "LocalScreen.h"
#include "KeyEvent.h"
#include "TouchEvent.h"

/* Android already has 5900 bound natively. */
#define VNC_PORT 5901

static void android_rfbLog(const char *format, ...) {
	va_list args;
	va_start(args, format);
	__android_log_vprint(ANDROID_LOG_DEBUG, LOG_TAG, format, args);
	va_end(args);
}

static void android_rfbErr(const char *format, ...) {
	va_list args;
	va_start(args, format);
	__android_log_vprint(ANDROID_LOG_ERROR, LOG_TAG, format, args);
	va_end(args);
}

class VncServer {
	LocalScreen mScreen;
	TouchEvent mTouchEvent;
	KeyEvent mKeyEvent, mKeyEvent2;
	char* mVncBuf;
	rfbScreenInfoPtr mVncScr;

	void update_screen(void) {
		int min_i, min_j, max_i, max_j;
		min_i = min_j = 9999;
		max_i = max_j = -1;

		// support only RGBA or RGBX
		const unsigned* f = (const unsigned*) mScreen.updateFrame();
		if (f == NULL)
			return;

		unsigned* r = (unsigned*) mVncBuf;

		for (int y = 0; y < mScreen.getHeight(); y++) {
			for (int x = 0; x < mScreen.getWidth(); ++x) {
				unsigned int pixel = *f++;

				if (pixel != *r) {
					*r = pixel;

					if (x < min_i)
						min_i = x;
					else {
						if (x > max_i)
							max_i = x;

						if (y > max_j)
							max_j = y;
						else if (y < min_j)
							min_j = y;
					}
				}

				r++;
			}
		}

		if (min_i < 9999) {
			if (max_i < 0)
				max_i = min_i;

			if (max_j < 0)
				max_j = min_j;

			++max_i;
			++max_j;

			rfbMarkRectAsModified(mVncScr, min_i, min_j, max_i, max_j);

			rfbProcessEvents(mVncScr, 10000);
		}
	}

	static void keyevent(rfbBool down, rfbKeySym key, rfbClientPtr cl) {
		VncServer* thiz = (VncServer*) cl->screen->screenData;
		switch ((int) key) {
		case 0xFF1B:
			thiz->mKeyEvent.injectEvent(KEY_BACK, down);
			break;
		case 0xFF50:
			thiz->mKeyEvent.injectEvent(KEY_HOMEPAGE, down);
			break;
		case 0xFFC7:
			thiz->mKeyEvent.injectEvent(KEY_MENU, down);
			break;
		case 0xFF57:
			thiz->mKeyEvent2.injectEvent(KEY_POWER, down);
			break;
		}
	}

	static void ptrevent(int buttonMask, int x, int y, rfbClientPtr cl) {
		VncServer* thiz = (VncServer*) cl->screen->screenData;
		thiz->mTouchEvent.injectEvent(x, y, (buttonMask & 1) != 0);
	}

public:
	VncServer() :
			mTouchEvent("/dev/input/event2"), mKeyEvent("/dev/input/event3"), mKeyEvent2(
					"/dev/input/event6") {
		mVncBuf = new char[mScreen.getWidth() * mScreen.getHeight()
				* mScreen.getBytesPerPixel()];
		assert(mVncBuf != NULL);

		/* TODO: This assumes scrinfo.bits_per_pixel is 16. */
		const char* argv[] = { "" };
		int argc = sizeof(argv) / sizeof(*argv);
		mVncScr = rfbGetScreen(&argc, (char**) argv, mScreen.getWidth(),
				mScreen.getHeight(), 8, 4, 4);
		assert(mVncScr != NULL);

		mVncScr->desktopName = "Android";
		mVncScr->frameBuffer = mVncBuf;
		mVncScr->alwaysShared = TRUE;
		mVncScr->httpDir = NULL;
		mVncScr->port = VNC_PORT;

		mVncScr->kbdAddEvent = keyevent;
		mVncScr->ptrAddEvent = ptrevent;
		mVncScr->screenData = this;

		rfbInitServer(mVncScr);

		/* Mark as dirty since we haven't sent any updates at all yet. */
		rfbMarkRectAsModified(mVncScr, 0, 0, mScreen.getWidth(),
				mScreen.getHeight());
	}

	void run() {
		android_rfbLog("Running loop...");
		for (;;) {
			while (mVncScr->clientHead == NULL)
				rfbProcessEvents(mVncScr, 100000);

			rfbProcessEvents(mVncScr, 100000);
			update_screen();
		}
	}
};

extern "C" void JNICALL Java_pingbu_android_vncserver_VncService_runServer(JNIEnv* env, jclass* clazz) {
	rfbLog = android_rfbLog;
	rfbErr = android_rfbErr;
	VncServer().run();
}

int main(int argc, char **argv) {
	VncServer().run();
}
