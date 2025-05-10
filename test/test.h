#pragma once

#include <vector>

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
	inline static TestAllocator _ta_##fn(&_ts_##fn);

TestResult test_fail() {
	return {TestResult::FAILED, "intentional"};
}
//DEFTEST("Intentional failure", "Designed to fail intentionally", test_fail);

TestResult test_success() {
	return {TestResult::PASSED};
}
DEFTEST("Intentional success", "Designed to succeed intentionally", test_success);