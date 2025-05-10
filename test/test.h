#pragma once

struct TestResult {
	enum Res {
		PASSED,
		FAILED,
	} res;
	const char* reason;
};

struct Test {
	const char* name;
	const char* description;
	TestResult (*test_function)();
};

inline TestResult _test_success_fn() {
	return {TestResult::PASSED, nullptr};
}
inline struct Test test_success = {
	"Test test success",
	"This is a test to test tests. it should succeed",
	&_test_success_fn,
};

inline TestResult _test_fail_fn() {
	return {TestResult::FAILED, "intentional failure"};
}
inline struct Test test_fail = {
	"Test test fail",
	"This is a test to test tests. it should fail",
	&_test_fail_fn,
};

//#define IMPL_TEST(name, desc, body) 