#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

uint32_t isa_reg_str2val(const char *, bool *);

enum {
  TK_NOTYPE = 256, TK_EQ,

  /* TODO: Add more token types */
	  /* PA1-5 */
	  TK_ADD = '+',
	  TK_SUB = '-',
	  TK_MUL = '*',
	  TK_DIV = '/',
	  TK_LP = '(',
	  TK_RP = ')',
	  HEX = 258,
	  DEREF,
	  NUM,
	  VAR,
	  REG,
	  TK_NEQ,
	  TK_NOT,
	  TK_AND,
	  TK_OR
	  /* PA1-5 */
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", TK_ADD},		// plus
  {"==", TK_EQ},        // equal
  
  /* PA1-5 */
  {"\\-", TK_SUB},		// sub
  {"\\*", '*'},			// multiply or dereference
  {"/", TK_DIV},		// divide
  {"0[xX][0-9a-fA-F]+", HEX},	// hex
  {"\\(", TK_LP},		// left parenthesis
  {"\\)", TK_RP},		// right parenthesis
  {"[1-9][0-9]*|0", NUM},		// number
  {"[a-zA-Z_]\\w*", VAR},	// variable
  {"\\$[eE][a-zA-Z]{2}|\\$pc", REG},	// register
  {"!=", TK_NEQ},		// not equal
  {"!", TK_NOT},		// not
  {"&&", TK_AND},		// and
  {"\\|\\|", TK_OR},	// or
  /* PA1-5 */

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

// TOKENS_SIZE default 32, test 500
#define TOKENS_SIZE 32

static Token tokens[TOKENS_SIZE] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

		/*
        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
		*/
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

		/* pa1-5 */
        switch (rules[i].token_type) {
			case TK_NOTYPE:
				break;
			case HEX:
			case NUM:
			case VAR:
			case REG:
				assert(substr_len < 32);
				memset(tokens[nr_token].str, '\0', sizeof(tokens[nr_token].str));
				strncpy(tokens[nr_token].str, substr_start, substr_len);
			default:
				tokens[nr_token].type = rules[i].token_type;
				nr_token++;
        }
		/* pa1-5 */

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

/* pa1-5 test (for make_token)
 * FOR TEST: output the information of token in tokenArray
 */
void tokenOutput(int p, int q) {
	int i;
	if (p < 0 || q > nr_token) {
		printf("left boarder or right boarder wrong. \n");
		return;
	}
	for (i = p; i < q; i++) {
		if (tokens[i].type > 255) {
			printf("Token[%d] - Type[%d] - Str[%s]\n", i, tokens[i].type, tokens[i].str);
		} else {
			printf("Token[%d] - Type[%c]\n", i, tokens[i].type);
		}
	}
}
/* pa1-5 test */

/* pa1-5 */
static bool check_parentheses(int p, int q, bool *success) {
	/*
	tokenOutput(p, q);
	printf("=========\n");
	*/
	int i, st = 0;
	bool bnf = true;
	if (tokens[p].type != TK_LP || tokens[q].type != TK_RP) {
		bnf = false;
	}
	for (i = p + 1; i < q; i++) {
		if (tokens[i].type == TK_LP) {
			st++;
		}
		else if (tokens[i].type == TK_RP) {
			st--;
			if (st < 0) {
				bnf = false;
			}
		}
	}
	if (tokens[p].type == TK_LP) {
		st++;
	}
	if (tokens[q].type == TK_RP) {
		st--;
	}
	if (st != 0) {
		*success = false;
		printf("incompatible Parentheses, stack %d\n", st);
		return false;
	}
	return bnf;
}

static bool is_op(int p) {
	if (tokens[p].type == HEX || tokens[p].type == NUM || tokens[p].type == VAR || tokens[p].type == REG || tokens[p].type == TK_LP || tokens[p].type == TK_RP) {
		return false;
	}
	return true;
}

static int op_priority(int p) {
	int ret;
	switch (tokens[p].type) {
		case TK_OR:
		case TK_AND:
			ret = 0;
			break;
		case TK_EQ:
		case TK_NEQ:
			ret = 1;
			break;
		case TK_ADD:
		case TK_SUB:
			ret = 2;
			break;
		case TK_MUL:
		case TK_DIV:
			ret = 3;
			break;
		case DEREF:
		case TK_NOT:
			ret = 4;
			break;
		default:
			ret = 5;
	}
	return ret;
}

static int main_op_position(int p, int q) {
	int i;
	int main_pos = p;
	for (i = p; i < q; i++) {
		if (is_op(i)) {
			if (op_priority(i) <= op_priority(main_pos)) {
				main_pos = i;
			}
		}
		else if(tokens[i].type == TK_LP) {
			int st = 1;
			while (st != 0 && ++i < q) {
				if (tokens[i].type == TK_LP) {
					st++;
				}
				else if (tokens[i].type == TK_RP) {
					st--;
				}
			}
		}
	}
	return main_pos;
}

/* Evaluate the expression
 * p -> token start, q -> token end, success -> process status
 */
static uint32_t eval(int p, int q, bool *success) {
	if (!(*success)) {
		printf("Invalid expression, Abort at [%d, %d].\n", p, q);
		return 0;
	}
	if (p > q) {
		/* Bad expression */
		printf("Bad expression! [%d, %d]\n", p, q);
		*success = false;
		return 0;
	}
	else if (p == q) {
		/* single token */
		switch (tokens[p].type) {
			case NUM:
			case HEX:
				return strtoul(tokens[p].str, NULL, 0);
			case VAR:
				TODO();
				return 0;
			case REG:
				return isa_reg_str2val(tokens[p].str, success);
			default:
				printf("Invalid expression, token[%d] is redundant.\n", p);
				*success = false;
				return 0;
		}
	}
	else if (check_parentheses(p, q, success)) {
		/* The expression is surrounded by a matched () */
		return eval(p + 1, q - 1, success);
	}
	else {
		if (!(*success)) {
			return 0;
		}
		int op_pos = main_op_position(p, q);
		//printf("op_pos = %d | p = %d | q = %d\n", op_pos, p, q);
		uint32_t val1 = 0;
		if (tokens[op_pos].type != DEREF && tokens[op_pos].type != TK_NOT) {
			val1 = eval(p, op_pos - 1, success);
		}
		if (!(*success)) {
			return 0;
		}
		uint32_t val2 = eval(op_pos + 1, q, success);
		if (!(*success)) {
			return 0;
		}
		switch (tokens[op_pos].type) {
			case TK_ADD:
				return val1 + val2;
			case TK_SUB:
				if (val1 < val2) {
					printf("Negative result occurs in calculation (%d - %d), abort.\n", val1, val2);
					*success = false;
					return 0;
				}
				return val1 - val2;
			case TK_MUL:
				return val1 * val2;
			case TK_DIV:
				if (val2 == 0) {
					printf("Cannot divide 0 (%d / %d)\n", val1, val2);
					*success = 0;
					return 0;
				}
				return val1 / val2;
			case TK_EQ:
				return val1 == val2;
			case TK_NEQ:
				return val1 != val2;
			case TK_AND:
				return val1 && val2;
			case TK_OR:
				return val1 || val2;
			case TK_NOT:
				return !val2;
			case DEREF:
				return vaddr_read(val2, 1);
			default:
				printf("main operator goes wrong: %d\n", tokens[op_pos].type);
				*success = false;
				return 0;
		}
	}
}

static inline void check_deref() {
	int i;
	for (i = 0; i < nr_token; i++) {
		if (tokens[i].type == '*' && (i == 0 || is_op(i - 1))) {
			tokens[i].type = DEREF;
		}
	}
}
/* pa1-5 */

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* pa1-5 test */
  //tokenOutput(0, nr_token);
  /* pa1-5 test */

  /* TODO: Insert codes to evaluate the expression. */
  /* pa1-5 */
  check_deref();

  return eval(0, nr_token-1, success);
  /* pa1-5 */
}
