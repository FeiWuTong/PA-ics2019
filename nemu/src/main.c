int init_monitor(int, char *[]);
void ui_mainloop(int);

/* Test Part */
#include "monitor/expr.h"
/* Test Part */

/* pa1-5 Test Part */
// Remember adjust the SIZE of tokens in expr.c
void test_expr() {
	FILE *fp = fopen("./tools/gen-expr/input", "r");
	assert(fp != NULL);
	char exp[50000];
	uint32_t result = 0;
	uint32_t cal = 0;
	bool success = true;
	int total = 0;
	int right = 0;
	while (fscanf(fp, "%u", &result) != EOF) {
		fgets(exp, 50000, fp);
		if (exp[strlen(exp) - 1] == '\n') {
			exp[strlen(exp) - 1] = '\0';
		}
		total++;
		cal = expr(exp, &success);
		if (success) {
			right++;
			printf("Real: %u | Calc: %u | Judge: %s\n", result, cal, result == cal ? "Right" : "Fall");
		}
		else {
			printf("[%u] Wrong at: %s\n", result, exp);
		}
		success = true;
	}
	printf("Total: %d | Right: %d | Wrong: %d\n", total, right, total - right);
}
/* pa1-5 Test Part */

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);

	/* pa1-5 Test Part */
	//bool success = true;
	//char *e = "0xc0100000+  ($eax +500)*4 - *( $ebp + 8) + number";
	//char *e = "0x11111111111111111111111111111111111111111111111111";
	//char *e = "12315 + 0xa234 *  6- 123";
	//char *e = "123 / ( 456 /(789))";
	//printf("Test expression:\n%s <==> %d\n", e, expr(e, &success));
	//test_expr();
	/* pa1-5 Test Part */

  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
