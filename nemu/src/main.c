int init_monitor(int, char *[]);
void ui_mainloop(int);

/* Test Part */
#include "monitor/expr.h"
uint32_t expr(char *, bool *);
/* Test Part */

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);

	/* Test Part */
	bool success;
	char *e = "0xc0100000+  ($eax +5)*4 - *( $ebp + 8) + number";
	expr(e, &success);
	/* Test Part */

  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
