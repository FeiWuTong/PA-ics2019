#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  assert(0);
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  int n = 0, num, buf_n;
  char buf[10];
  char *s;
  while (*fmt) {
	if (*fmt == '%') {
	  switch (*++fmt) {
		case 'd':
		  num = va_arg(ap, int), buf_n = 0;
		  while (num) {
			buf[buf_n++] = num % 10 + '0';
			num /= 10;
		  }
		  while (buf_n--) out[n++] = buf[buf_n];
		  break;
		case 's':
		  s = va_arg(ap, char*);
		  while (*s) out[n++] = *s++;
		  break;
		default:
		  out[n++] = *fmt;
	  }
	} else {
		out[n++] = *fmt;
	}
	fmt++;
  }
  out[n] = '\0';
  return n;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int n = vsprintf(out, fmt, ap);
  va_end(ap);
  return n;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  assert(0);
  return 0;
}

#endif
