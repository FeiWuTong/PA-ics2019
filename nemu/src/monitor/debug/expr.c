#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

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
  {"[1-9]\\d*|0", NUM},		// number
  {"[a-zA-Z_]+\\w*", VAR},	// variable
  {"\\$[eE][a-zA-Z]{2}", REG},	// register
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

static Token tokens[32] __attribute__((used)) = {};
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

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
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

/* pa1-5 test
 * FOR TEST: output the information of token in tokenArray
 */
void tokenOutput() {
	int i;
	for (i = 0; i < nr_token; i++) {
		if (tokens[i].type > 255) {
			printf("Token[%d] - Type[%d] - Str[%s]\n", i, tokens[i].type, tokens[i].str);
		} else {
			printf("Token[%d] - Type[%c]\n", i, tokens[i].type);
		}
	}
}
/* pa1-5 test */

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  tokenOutput();

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
