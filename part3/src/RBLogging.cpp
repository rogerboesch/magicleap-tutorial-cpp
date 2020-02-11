
#include "platform_includes.h"
#include <stdio.h>
#include <stdarg.h>

extern "C" {
	void console_print(char* message);
}

static RBLogLevel _logLevel = Default;

bool RBLoggingLogLevelIsEnabled(RBLogLevel level) {
	if (level >= _logLevel) {
		return true;
	}
	
	return false;
}

void RBLoggingLogVargs(RBLogLevel level, const char *tag, const char *fmt, ...) {
	char buffer[256];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, 255, fmt, args);
	va_end (args);

	char message[300];

	switch (level) {
	case Debug:
		sprintf(message, "%s", buffer);
		break;
	case Warn:
		sprintf(message, "WRN: %s", buffer);
		break;
	case Error:
		sprintf(message, "ERR: %s", buffer);
		break;

	default:
		sprintf(message, "%s", buffer);
		break;
	}

	printf("%s\n", message);
	console_print(message);
}
