static void getTimeStamp(char *buffer, size_t size) {
	time_t rawtime;
	time(&rawtime);
	strftime(buffer, size, "%F %T", localtime(&rawtime));
}

static void logToFile(FILE *file, const char *type,
	const char *fileName, int lineNumber, const char *message) {
	char timestamp[32];
	getTimeStamp(timestamp, sizeof(timestamp));
	fprintf(file, "[%s] %s:%d %s: %s\n", timestamp, fileName, lineNumber, type, message);
}

void logError(const char *fileName, int lineNumber, const char *format, ...) {
	// SDL_ShowSimpleMessageBox

	char buffer[1024];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);
	logToFile(stderr, "ERROR", fileName, lineNumber, buffer);
}

void logWarning(const char *fileName, int lineNumber, const char *format, ...) {
	char buffer[1024];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);
	logToFile(stderr, "WARNING", fileName, lineNumber, buffer);
}

void logInfo(const char *fileName, int lineNumber, const char *format, ...) {
	char buffer[1024];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);
	logToFile(stdout, "INFO", fileName, lineNumber, buffer);
}
