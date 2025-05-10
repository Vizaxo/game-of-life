#pragma once

#include "test.h"
#include "common.h"
#include "debug.h"

// TODO: move to separate test project

inline void test_print(const char* str) {
	std::cout << str;
}

inline TestResult execute_test(Test test) {
	assert(test.test_function);
	TestResult res = test.test_function();
	char buf[1024];
	switch (res.res) {
	case TestResult::PASSED:
		snprintf(buf, 1024, "PASSED: %s\n", test.name);
		test_print(buf);
		break;
	case TestResult::FAILED:
		snprintf(buf, 1024, "FAILED: %s\n\t%s\n\tREASON: %s\n", test.name, test.description, res.reason);
		test_print(buf);
		break;
	default:
		assert(false);
	}
	return res;
}

inline void execute_tests(std::vector<Test*> &tests) {
	int total_tests = 0;
	int passes = 0;
	for (Test* test: tests) {
		++total_tests;
		reset_test_env();
		if (execute_test(*test).res == TestResult::PASSED)
			++passes;
	}
	char buf[1024];
	if (passes == total_tests) {
		snprintf(buf, 1024, "========\n%d/%d tests passed.\nSUCCESS.\n========\n", passes, total_tests);
		test_print(buf);
		exit(0);
	} else {
		snprintf(buf, 1024, "========\n%d/%d tests passed.\nFAILURE.\n========\n", passes, total_tests);
		test_print(buf);
		exit(1);
	}
}
