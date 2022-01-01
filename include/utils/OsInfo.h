#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define OS_WINDOWS
#elif __linux__
#define OS_LINUX
#else
#error "Unsupported"
#endif
