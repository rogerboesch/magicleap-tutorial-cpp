#include <ml_logging.h>

// Tag used for logging
const char APP_TAG[] = "MAIN";

// Main
int main() {
	MLLoggingEnableLogLevel(MLLogLevel_Debug);
	ML_LOG_TAG(Debug, APP_TAG, "Hello World");

	return 0;
}
