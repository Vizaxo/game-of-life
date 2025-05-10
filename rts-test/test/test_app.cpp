#include "test.h"

#include "app.h"

// https://devblogs.microsoft.com/oldnewthing/20041025-00/?p=37483
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

App app;

TestResult test_app_init() {
	HRESULT hr = app.init(HINST_THISCOMPONENT, 640, 480);
	assert_equal(hr, S_OK, "%x");
	test_return();
}
DEFTEST("App initialises", "Creating and initialising an app completes successfully", test_app_init);

TestResult test_app_update() {
	HRESULT hr = app.update(1.f / 60.f);
	assert_equal(hr, S_OK, "%x");
	test_return();
}
DEFTEST("App update", "App updates without error", test_app_update);

TestResult test_app_render() {
	HRESULT hr = app.render();
	assert_equal(hr, S_OK, "%x");
	test_return();
}
DEFTEST("App render", "App renders a frame without error", test_app_render);

TestResult test_app_cleanup() {
	HRESULT hr = app.cleanup();
	assert_equal(hr, S_OK, "%x");
	test_return();
}
DEFTEST("App cleanup", "App cleans up without error", test_app_cleanup);

TestResult test_app_quit() {
	HRESULT hr = app.quit();
	assert_equal(hr, S_OK, "%x");
	test_return();
}
DEFTEST("App quit", "App quits without error", test_app_quit);
