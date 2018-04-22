#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/input.h>

#pragma once

class InputEvent {
	int fd;

protected:
	InputEvent(const char* name) {
		fd = open(name, O_RDWR);
		if (fd == -1)
			fprintf(stderr, "cannot open touch device %s\n", name);
	}

	virtual ~InputEvent() {
		if (fd != -1)
			close(fd);
	}

	void injectInputEvent(int type, int code, int value) {
		if (fd != -1) {
			printf("injectInputEvent %04x %04x %08x\n", type, code, value);
			struct input_event ev = { 0 };
			gettimeofday(&ev.time, 0);
			ev.type = type;
			ev.code = code;
			ev.value = value;
			if (write(fd, &ev, sizeof(ev)) < 0)
				printf("write event failed, %s\n", strerror(errno));
		}
	}
};
