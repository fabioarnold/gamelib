#define LOGE(...) logError(__FILE__,__LINE__,__VA_ARGS__);
#define LOGW(...) logWarning(__FILE__,__LINE__,__VA_ARGS__);
#define LOGI(...) logInfo(__FILE__,__LINE__,__VA_ARGS__);

void logError(const char *fileName, int lineNumber, const char *format, ...);
void logWarning(const char *fileName, int lineNumber, const char *format, ...);
void logInfo(const char *fileName, int lineNumber, const char *format, ...);
