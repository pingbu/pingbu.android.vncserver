#define LOG_TAG  "VncServer"

#include <android/log.h>

#include <gui/ISurfaceComposer.h>
#include <gui/SurfaceComposerClient.h>

#pragma once

using namespace android;

class LocalScreen {
	sp<IBinder> display;
	ScreenshotClient screenshot;
	int width, height, format;

public:
	LocalScreen() {
		display = SurfaceComposerClient::getBuiltInDisplay(
				ISurfaceComposer::eDisplayIdMain);
		assert(display != NULL);

		int r = screenshot.update(display);
		assert(r != NO_ERROR);

		width = (int) screenshot.getWidth();
		height = (int) screenshot.getHeight();
		format = (int) screenshot.getFormat();

		__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,
				"size=%ux%u, format=%u\n", width, height, format);
	}

	int getWidth() const {
		return width;
	}
	int getHeight() const {
		return height;
	}
	PixelFormat getFormat() const {
		return (PixelFormat) format;
	}
	int getBytesPerPixel() const {
		if (format == PIXEL_FORMAT_RGB_565 || format == PIXEL_FORMAT_RGBA_5551
				|| format == PIXEL_FORMAT_RGBA_4444)
			return 2;
		else if (format == PIXEL_FORMAT_RGB_888)
			return 3;
		else
			return 4;
	}

	const void* updateFrame() {
		if (screenshot.update(display) == NO_ERROR)
			return screenshot.getPixels();
		else
			return NULL;
	}
};
