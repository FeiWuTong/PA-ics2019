#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

/* pa1-6 */
WP* new_wp() {
	WP *wp = free_;
	if (wp == NULL) {
		printf("No free watchpoint in pool.\n");
		assert(0);
	}
	free_ = free_->next;
	wp->next = head;
	head = wp;
	return wp;
}

void free_wp(int n) {
	WP *find = head, *pre = NULL;
	while (find != NULL) {
		if (find->NO == n) {
			if (pre != NULL) {
				pre->next = find->next;
			}
			else {
				head = head->next;
			}
			find->next = free_;
			free_ = find;
			printf("Watchpoint %d has been freed.\n", n);
			return;
		}
		pre = find;
		find = find->next;
	}
	printf("Watchpoint %d not used.\n", n);
}

bool check_wps() {
	WP *wp = head;
	bool trigger = false;
	while (wp != NULL) {
		bool success = true;
		uint32_t new_result = expr(wp->expr, &success);
		if (!success) {
			trigger = true;
			printf("Watchpoint %d with EXPR %s (Result %u) change, but evaluation fail.\n", wp->NO, wp->expr, wp->result);
		}
		else {
			if (new_result != wp->result) {
				trigger = true;
				printf("Watchpoint %d | %s\n", wp->NO, wp->expr);
				printf("Old result: %u (0x%x)\n", wp->result, wp->result);
				printf("New result: %u (0x%x)\n", new_result, new_result);
				wp->result = new_result;
			}
		}
		wp = wp->next;
	}
	return trigger;
}

void info_wps() {
	if (head == NULL) {
		printf("No used watchpoint.\n");
		return;
	}
	printf("NO\tEXPR\t\tResult\n");
	WP *wp = head;
	while (wp != NULL) {
		printf("%d\t%s\t\t%u(0x%x)\n", wp->NO, wp->expr, wp->result, wp->result);
		wp = wp->next;
	}
}
/* pa1-6 */
