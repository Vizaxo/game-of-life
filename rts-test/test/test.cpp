#include "test.h"

int fatal_log_hit = 0;
bool expecting_fatal_log_hit = false;

void reset_test_env() {
	fatal_log_hit = 0;
	expecting_fatal_log_hit = false;
}
