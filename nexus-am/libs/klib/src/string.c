#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t n = 0;
  if (s != NULL) {
	  while (s[n] != '\0') n++;
  }
  return n;
}

char *strcpy(char* dst,const char* src) {
  size_t i;
  for (i = 0; src[i] != '\0'; i++) dst[i] = src[i];
  dst[i] = '\0';
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  size_t i;
  for (i = 0; i < n && src[i] != '\0'; i++) dst[i] = src[i];
  for (; i < n; i++) dst[i] = '\0';
  return dst;
}

char* strcat(char* dst, const char* src) {
  assert(dst != NULL && src != NULL);
  size_t n1 = strlen(dst);
  while (*src != '\0') dst[n1++] = *(src++);
  dst[n1] = '\0';
  return dst;
}

int strcmp(const char* s1, const char* s2) {
  if (s1 == NULL || s2 == NULL) return s1 != NULL ? 1 : (s2 == NULL ? 0 : -1);
  while (*s1 != '\0' && *s1 == *s2) s1++, s2++;
  return *s1 - *s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  assert(0);
  return 0;
}

void* memset(void* v,int c,size_t n) {
  int i;
  for (i = 0; i < n; i++) {
	*(char *)(v + i) = c;
  }
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  assert(out != NULL && in != NULL);
  int i;
  if (out <= in) {
	for (i = 0; i < n; i++) *((char *)out + i) = *((char *)in + i);
  } else {
	for (i = n - 1; i >= 0; i--) *((char *)out + i) = *((char *)in + i);
  }
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  if (s1 == NULL || s2 == NULL) return s1 != NULL ? 1 : (s2 == NULL ? 0 : -1);
  int i;
  for (i = 0; i < n; i++) {
	if (*((char *)s1 + i) != *((char *)s2 + i)) return *((char *)s1 + i) - *((char *)s2 + i);
  }
  return 0;
}

#endif
