#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* pa1-4 */
void isa_reg_display();
/* pa1-4 */

/* pa1-5 */
WP* new_wp();
void free_wp();
void info_wps();
/* pa1-5 */

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

/* pa1-4 */
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);
/* pa1-4 */

/* pa1-5 */
static int cmd_p(char *args);
/* pa1-5 */

/* pa1-6 */
static int cmd_w(char *args);
static int cmd_d(char *args);
/* pa1-6 */

static int cmd_detach(char *args);
static int cmd_attach(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  { "si", "Single step execution of N instructions then suspend, usage: si [N], default N = 1", cmd_si },
  { "info", "Usage: info [rw], [r] means print the register, [w] means print the watchpoint", cmd_info },
  { "x", "Scan memory from specific address, usage: x [N] [EXPR], where N represents output N-4bytes successively, and EXPR represents expression whose value will be starting address of the scan", cmd_x },
  { "p", "Evaluate the expression, usage: p [EXPR]", cmd_p},
  { "w", "Set watchpoint for expression, usage: w [EXPR]", cmd_w},
  { "d", "Delete watchpoint N, usage: d [N]", cmd_d},
  { "detach", "detach from DiffTest mode", cmd_detach},
  { "attach", "attach to DiffTest mode", cmd_attach},

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

/* pa1-4 */
static int cmd_si(char *args) {
	uint64_t step = 1;
	if (args) {
		unsigned long long temp = strtoull(args, NULL, 10);
		if (temp == 0) {
			printf("si command should be 'si [N]', in which N is an unsigned long long number that bigger than 0\n");
			return 0;
		}
		step = (uint64_t)temp;
	}
	cpu_exec(step);
	return 0;
}

static int cmd_info(char *args) {
	char *subcmd = strtok(NULL, " ");
	if (subcmd == NULL) {
		printf("Usage: info [rw]\nTry 'help info' for more information.\n");
	}
	else if (strcmp(subcmd, "r") == 0) {
		isa_reg_display();
	}
	else if (strcmp(subcmd, "w") == 0) {
		/* pa1-6 */
		info_wps();
		/* pa1-6 */
	}
	else {
		printf("Usage: info [rw]\nTry 'help info' for more information.\n");
	}
	return 0;
}

static int cmd_x(char *args) {
	char *arg = strtok(NULL, " ");
	if (arg == NULL) {
		printf("Usage: x [N] [EXPR]\nTry 'help x' for more information.\n");
		return 0;
	}
	int n = atoi(arg);
	if (n <= 0) {
		printf("N must be an integer and bigger than 0\n");
		return 0;
	}
	/* temp use hexadecimal only instead of expression */
	arg = strtok(NULL, " ");
	if (arg == NULL) {
		printf("Usage: x [N] [EXPR]\nTry 'help x' for more information.\n");
		return 0;
	}
	//vaddr_t expr = strtoul(arg, NULL, 16);
	/* pa1-5 */
	bool success = true;
	vaddr_t addr = expr(arg, &success);
	if (!success) {
		printf("Expression wrong.\n");
		return 0;
	}
	/* pa1-5 */

	int i;
	while (n--) {
		printf("  %x:   ", addr);
		for (i = 0; i < 4; i++) {
			printf("%02x ", vaddr_read(addr++, 1));
		}
		printf("\n");
	}
	return 0;
}
/* pa1-4 */

/* pa1-5 */
static int cmd_p(char *args) {
	char *arg = strtok(NULL, "");
	if (arg == NULL) {
		printf("Usage: p [EXPR]\n");
		return 0;
	}
	bool success = true;
	uint32_t result = expr(arg, &success);
	if (success) {
		printf("Expression result: %u\n", result);
	}
	else {
		printf("Expression error.\n");
	}
	return 0;
}
/* pa1-5 */

/* pa1-6 */
static int cmd_w(char *args) {
	char *arg = strtok(NULL, "");
	if (arg == NULL) {
		printf("Usage: w [EXPR]\n");
		return 0;
	}
	bool success = true;
	uint32_t result = expr(arg, &success);
	if (!success) {
		printf("Set watchpoint failed: expression wrong.\n");
		return 0;
	}
	WP *wp = new_wp();
	strcpy(wp->expr, arg);
	wp->result = result;
	return 0;
}

static int cmd_d(char *args) {
	char *arg = strtok(NULL, "");
	if (arg == NULL) {
		printf("Usage: d [N], N is watchpoint NO\n");
		return 0;
	}
	free_wp(atoi(arg));
	return 0;
}
/* pa1-6 */

// ===== PA3 ======
extern void difftest_detach();
extern void difftest_attach();

static int cmd_detach(char *args) {
	difftest_detach();
	printf("Finish detaching.\n");
	return 0;
}

static int cmd_attach(char *args) {
	printf("Attaching...\n");
	difftest_attach();
	printf("Finish attaching.\n");
	return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
