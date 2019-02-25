#ifndef __LOG_HPP_6675602F_B79E_4488_988F_4F67E5416E1F__
#define __LOG_HPP_6675602F_B79E_4488_988F_4F67E5416E1F__

#include <stdio.h>
#include <WinBase.h>
#include <stdarg.h>

#define LOGD(...) ::__log_print__("DEBUG[" TAG "] ", __VA_ARGS__)
#define LOGI(...) ::__log_print__("INFO[" TAG "] ", __VA_ARGS__)
#define LOGE(...) ::__log_print__("ERROR[" TAG "] ", __VA_ARGS__)

#define TRACED(...) ::__trace_print__(__FILE__, __LINE__, "DEBUG[" TAG "] ", __VA_ARGS__)
#define TRACEI(...) ::__trace_print__(__FILE__, __LINE__, "INFO[" TAG "] ", __VA_ARGS__)
#define TRACEE(...) ::__trace_print__(__FILE__, __LINE__, "ERROR[" TAG "] ", __VA_ARGS__)

static void __log_print__(const char* tag, const char* fmt, ...) {
	char buffer[2048];

	char* ptr = buffer;
	int buffer_size = sizeof(buffer);
    int chars = sprintf_s(ptr, buffer_size, tag);
	ptr += chars;
	buffer_size -= chars;

	if(buffer_size > 0) {
		va_list marker;
		va_start(marker, fmt);

		chars = vsprintf_s(ptr, buffer_size, fmt, marker);
		ptr += chars;
		buffer_size -= chars;

		va_end(marker);
	}

	if(buffer_size > 0) {
		chars = sprintf_s(ptr, buffer_size, "\n");
		ptr += chars;
		buffer_size -= chars;
	}

	OutputDebugStringA(buffer);
}

static void __trace_print__(const char* file, int line, const char* tag, const char* fmt, ...) {
	char buffer[2048];

	char* ptr = buffer;
	int buffer_size = sizeof(buffer);
    int chars = sprintf_s(ptr, buffer_size, "%s(%d): %s", file, line, tag);
	ptr += chars;
	buffer_size -= chars;

	if(buffer_size > 0) {
		va_list marker;
		va_start(marker, fmt);

		chars = vsprintf_s(ptr, buffer_size, fmt, marker);
		ptr += chars;
		buffer_size -= chars;

		va_end(marker);
	}

	if(buffer_size > 0) {
		chars = sprintf_s(ptr, buffer_size, "\n");
		ptr += chars;
		buffer_size -= chars;
	}

	OutputDebugStringA(buffer);
}

#endif // __LOG_HPP_6675602F_B79E_4488_988F_4F67E5416E1F__