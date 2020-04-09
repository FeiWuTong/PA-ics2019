#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
# define LIMIT 50000
static char buf[LIMIT];
/* pa1-5 */
static int cur = 0;

static uint32_t choose(uint32_t n) {
	return rand() % n;
}

static void gen_num() {
	char num[4];
	sprintf(num, "%u", choose(100) + 1);
	int i = 0;
	while (num[i] != '\0' && cur < LIMIT - 1) {
		buf[cur++] = num[i++];
	}
}

static void gen(char c) {
	buf[cur++] = c;
}

static void gen_rand_op() {
	if (cur > LIMIT - 3) {
		 return;
	}
	switch (choose(4)) {
		case 0: 
			buf[cur++] = '+'; break;
		case 1:
			buf[cur++] = '-'; break;
		case 2:
			buf[cur++] = '*'; break;
		default:
			buf[cur++] = '/';
	}
}

static inline void gen_rand_expr() {
	if (cur > LIMIT - 3) {
		return;
	}
	switch (choose(3)) {
		case 0: gen_num(); break;
		case 1: gen('('); gen_rand_expr(); gen(')'); break;
		default: gen_rand_expr(); gen_rand_op(); gen_rand_expr();
	}
	int i, j = choose(2);
	for (i = 0; i < j && cur < LIMIT - 1; i++) {
		buf[cur++] = ' ';
	}
	buf[cur] = '\0';
}

static inline void reset_buf() {
	cur = 0;
}
/* pa1-5 */

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
	  /* pa1-5 */
	  reset_buf();
	  /* pa1-5 */
    gen_rand_expr();

	//sprintf(code_buf, code_format, "1/(2/3)");
	//sprintf(code_buf, code_format, "1");
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

	/*
	ret = system("/tmp/.expr");
	if (ret != 0) {
		i--;
		continue;
	}
	*/

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);
	
	if (fgetc(fp) == EOF) {
		pclose(fp);
		i--;
		continue;
	}
    pclose(fp);
    fp = popen("/tmp/.expr", "r");

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
