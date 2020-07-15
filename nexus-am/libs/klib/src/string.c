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
  assert(dst != NULL && src != NULL);
  int i, n2 = strlen(src);
  if (dst <= src) {
	for (i = 0; i <= n2; i++) dst[i] = src[i];
  } else {
	for (i = n2; i >= 0; i--) dst[i] = src[i];
  }
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  assert(0);
  return NULL;
}

char* strcat(char* dst, const char* src) {
  assert(dst != NULL && src != NULL);
  int n1 = strlen(dst);
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
	*(int *)(v + i) = c;
  }
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  assert(0);
  return NULL;
}

int memcmp(const void* s1, const void* s2, size_t n){
  if (s1 == NULL || s2 == NULL) return s1 != NULL ? 1 : (s2 == NULL ? 0 : -1);
  int i;
  for (i = 0; i < n; i++) {
	if (*(int *)(s1 + i) != *(int *)(s2 + i)) return *(int *)s1 - *(int *)s2;
  }
  return 0;
}

#endif
