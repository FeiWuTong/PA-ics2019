#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  va_list ap;
  char out[5096];
  va_start(ap, fmt);
  int n = vsprintf(out, fmt, ap), i;
  va_end(ap);
  for (i = 0; i < n; i++) _putc(out[i]);
  return n;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  int num, buf_n, temp, width = 0, format_flag = 0, neg = 0;
  unsigned u;
  char buf[32];
  char *s, *outptr = out;

  while (*fmt) {
	if (format_flag) {
	  switch (*fmt) {
		case 'd':
		  num = va_arg(ap, int), buf_n = 0;
		  if (num <= 0) {
			if (num == 0) {
			  while (--width > 0) *outptr++ = ' ';
			  *outptr++ = '0';
			} else {
			  num = -num;
			  neg = 1;
			}
		  }
		  while (num) {
		    buf[buf_n++] = num % 10 + '0';
		    num /= 10;
		  }
		  width -= buf_n + neg;
		  while (width-- > 0) *outptr++ = ' ';
		  if (neg) {
			*outptr++ = '-';
			neg = 0;
		  }
		  while (buf_n--) *outptr++ = buf[buf_n];
		  format_flag = 0;
		  break;
		case 'u':
		  u = va_arg(ap, unsigned), buf_n = 0;
		  if (u == 0) {
			while (--width > 0) *outptr++ = ' ';
			*outptr++ = '0';
		  }
		  while (u) {
			buf[buf_n++] = u % 10 + '0';
			u /= 10;
		  }
		  width -= buf_n;
		  while (width-- > 0) *outptr++ = ' ';
		  while (buf_n--) *outptr++ = buf[buf_n];
		  format_flag = 0;
		  break;
		case 's':
		  s = va_arg(ap, char*);
		  width -= strlen(s);
		  while (width-- > 0) *outptr++ = ' ';
		  while (*s) *outptr++ = *s++;
		  format_flag = 0;
		  break;
		case 'x':
		case 'p':
		  if (*fmt == 'p') {
			*outptr++ = '0';
			*outptr++ = 'x';
		  }
		  num = va_arg(ap, unsigned), buf_n = 0;
		  if (num == 0) {
			while (--width > 0) *outptr++ = ' ';
			*outptr++ = '0';
		  }
		  while (num) {
			temp = num % 16;
			buf[buf_n++] = temp >= 10 ? (temp - 10) + 'a' : temp + '0';
			num /= 16;
		  }
		  width -= buf_n;
		  while (width-- > 0) *outptr++ = ' ';
		  while (buf_n--) *outptr++ = buf[buf_n];
		  format_flag = 0;
		  break;
		default:
		  if (*fmt >= '0' && *fmt <= '9') width = 10 * width + *fmt - '0';
		  else *outptr++ = *fmt;
	  }
	} else if (*fmt == '%') {
	  format_flag = 1;
  	  width = 0;
	} else {
	  *outptr++ = *fmt;
	}
	fmt++;
  }
  *outptr = '\0';
  return outptr - out;
}

/*
int vsnprintf(char *out, size_t size, const char *fmt, va_list ap) {
  if (size == 0) return 0;
  else size--;

  int num, buf_n, temp, width = 0, format_flag = 0, neg = 0;
  unsigned u;
  char buf[32];
  char *s, *outptr = out;

  while (*fmt) {
	if (size == 0) break;
	if (format_flag) {
	  switch (*fmt) {
		case 'd':
		  num = va_arg(ap, int), buf_n = 0;
		  if (num <= 0) {
			if (num == 0) {
			  if (width > size) {
				width = size;
				size = 0;
			  } else {
				if (width > 0) size -= width;
				else size--;
			  }
			  while (--width > 0) *outptr++ = ' ';
			  *outptr++ = '0';
			} else {
			  num = -num;
			  neg = 1;
			}
		  }
		  while (num) {
		    buf[buf_n++] = num % 10 + '0';
		    num /= 10;
		  }
		  width -= buf_n + neg;
		  if (width > size) {
			width = size;
			size = 0;
		  } else {
			if (width > 0) size -= width;
		  }
		  while (width-- > 0) *outptr++ = ' ';
		  if (size == 0) break;
		  if (neg) {
			*outptr++ = '-';
			size--;
			neg = 0;
		  }
		  if (buf_n > size) {
			buf_n = size;
			size = 0;
		  } else {
			size -= buf_n;
		  }
		  while (buf_n--) *outptr++ = buf[buf_n];
		  format_flag = 0;
		  break;
		case 'u':
		  u = va_arg(ap, unsigned), buf_n = 0;
		  if (u == 0) {
			if (width > size) {
			  width = size;
			  size = 0;
			} else {
			  if (width > 0) size -= width;
			  else size--;
			}
			while (--width > 0) *outptr++ = ' ';
			*outptr++ = '0';
		  }
		  while (u) {
			buf[buf_n++] = u % 10 + '0';
			u /= 10;
		  }
		  width -= buf_n;
		  if (width > size) {
			width = size;
			size = 0;
		  } else {
			if (width > 0) size -= width;
		  }
		  while (width-- > 0) *outptr++ = ' ';
		  if (buf_n > size) {
			buf_n = size;
			size = 0;
		  } else {
			size -= buf_n;
		  }
		  while (buf_n--) *outptr++ = buf[buf_n];
		  format_flag = 0;
		  break;
		case 's':
		  s = va_arg(ap, char*);
		  buf_n = strlen(s);
		  width -= buf_n;
		  if (width > size) {
			width = size;
			size = 0;
		  } else {
			if (width > 0) size -= width;
		  }
		  while (width-- > 0) *outptr++ = ' ';
		  if (buf_n > size) {
			buf_n = size;
			size = 0;
		  } else {
			size -= buf_n;
		  }
		  while (buf_n--) *outptr++ = *s++;
		  format_flag = 0;
		  break;
		case 'x':
		case 'p':
		  if (*fmt == 'p') {
			if (size <= 2) {
			  size = 0;
			  break;
			}
			size -= 2;
			*outptr++ = '0';
			*outptr++ = 'x';
		  }
		  num = va_arg(ap, unsigned), buf_n = 0;
		  if (num == 0) {
			if (width > size) {
			  width = size;
			  size = 0;
			} else {
			  if (width > 0) size -= width;
			  else size--;
			}
			while (--width > 0) *outptr++ = ' ';
			*outptr++ = '0';
		  }
		  while (num) {
			temp = num % 16;
			buf[buf_n++] = temp >= 10 ? (temp - 10) + 'a' : temp + '0';
			num /= 16;
		  }
		  width -= buf_n;
		  if (width > size) {
			width = size;
			size = 0;
		  } else {
			if (width > 0) size -= width;
		  }
		  while (width-- > 0) *outptr++ = ' ';
		  if (buf_n > size) {
			buf_n = size;
			size = 0;
		  } else {
			size -= buf_n;
		  }
		  while (buf_n--) *outptr++ = buf[buf_n];
		  format_flag = 0;
		  break;
		default:
		  if (*fmt >= '0' && *fmt <= '9') {
			width = 10 * width + *fmt - '0';
		  } else {
			*outptr++ = *fmt;
			size--;
		  }
	  }
	} else if (*fmt == '%') {
	  format_flag = 1;
  	  width = 0;
	} else {
	  *outptr++ = *fmt;
	  size--;
	}
	fmt++;
  }
  *outptr = '\0';
  return outptr - out;
}
*/

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int n = vsprintf(out, fmt, ap);
  va_end(ap);
  return n;
}

/*
int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret_n = vsnprintf(out, n, fmt, ap);
  va_end(ap);
  return ret_n;
}
*/

// not std, fake snprintf
int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret_n = vsprintf(out, fmt, ap);
  va_end(ap);
  if (ret_n >= n) {
	*(out + n - 1) = '\0';
	return n;
  }
  return ret_n;
}

#endif
