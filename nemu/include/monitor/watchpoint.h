#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  /* pa1-6 */
  char expr[300];
  uint32_t result;
  /* pa1-6 */

} WP;

#endif
