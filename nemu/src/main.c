int init_monitor(int, char *[]);
void ui_mainloop(int);

/* Test Part */
#include "monitor/expr.h"
uint32_t expr(char *, bool *);
/* Test Part */

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);

	/* pa1-5 Test Part */
	bool success = true;
	//char *e = "0xc0100000+  ($eax +500)*4 - *( $ebp + 8) + number";
	//char *e = "0x11111111111111111111111111111111111111111111111111";
	char *e = "12315 + 0xa234 *  6- 123";
	printf("Test expression:\n%s <==> %d\n", e, expr(e, &success));
	/* pa1-5 Test Part */

  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
