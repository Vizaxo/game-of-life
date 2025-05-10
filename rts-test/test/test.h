#pragma once

#define _TEST

#include <vector>

struct TestResult {
	enum Res {
		PASSED,
		FAILED,
	} res;
	const char* reason;
};

#define test_fail() return {TestResult::FAILED, "test_fail() called"};

struct Test {
	const char* name;
	const char* description;
	TestResult (*test_function)();
};

inline std::vector<Test*> tests = {};
class TestAllocator {
public:
	TestAllocator(Test* test) {
		tests.push_back(test);
	}
};

#define DEFTEST(name, desc, fn)\
	inline struct Test _ts_##fn = {\
		#name,\
		#desc,\
		&fn,\
	};\
	inline TestAllocator _ta_##fn(&_ts_##fn);

#define assert_equal(a, b) do{\
	if (!(a == b)) {\
		return {TestResult::FAILED, #a##" does not equal "## #b##", but it should"};\
	}\
}while(0);

#define assert_not_equal(a, b) do{\
	if (!(a != b)) {\
		return {TestResult::FAILED, #a##" equals "## #b##", but it shouldn't"};\
	}\
}while(0);

#define assert_greater(a, b) do{\
	if (!(a > b)) {\
		return {TestResult::FAILED, #a##" is not greater than "## #b};\
	}\
}while(0);

extern int fatal_log_hit;
extern bool expecting_fatal_log_hit;
#define assert_fatal_log_hit() do{\
	assert_greater(fatal_log_hit, 0);\
	expecting_fatal_log_hit = true;\
}while(0);

#define assert_fatal_log_not_hit() assert_equal(fatal_log_hit, 0);
void reset_test_env();

#define test_return() do{\
	if (!expecting_fatal_log_hit) {\
		assert_fatal_log_not_hit();\
	}\
	return {TestResult::PASSED, nullptr};\
}while(0);


// EXAMPLE
inline TestResult test_success() {
	test_return();
}
DEFTEST("Intentional success", "Designed to succeed intentionally", test_success);

