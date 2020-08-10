#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
  size_t success = 0;
  while (success < len) _putc(*((char *)buf + success++));
  return success;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  //Log("len %d", len);
  int key = read_key();
  bool keydown = false;
  if (key & 0x8000) {
	keydown = true;
	key ^= 0x8000;
  }
  if (key == _KEY_NONE) return snprintf(buf, len, "t %u\n", uptime());
  else return snprintf(buf, len, "%s %s\n", keydown ? "kd" : "ku", keyname[key]);
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  if (len + offset > 128) len = 128 - offset;
  strncpy(buf, dispinfo + offset, len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  int w = screen_width();
  int x = (offset / 4) % w, y = (offset / 4) / w;
  draw_rect((uint32_t *)buf, x, y, len / 4, 1);
  return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  draw_sync();
  return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", screen_width(), screen_height());
}
