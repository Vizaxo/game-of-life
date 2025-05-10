#include "test.h"

#include "dxutils.h"

HRESULT hr_file_not_found = 0x80070002;
HRESULT hr_dir_not_found = 0x80070003;
TestResult test_shader_comp_file_not_found() {
	ID3DBlob* shader_blob = nullptr;
	HRESULT hr = compile_shader(L"non_existent_file.hlsl", "main", ShaderType::Pixel, &shader_blob);
	HRESULT expected = hr_file_not_found;
	assert_equal(hr, expected);
	assert_fatal_log_hit();
	assert_equal(shader_blob, nullptr, "%p");
	test_return();
}
DEFTEST("shader file not found", "shader compilation fails when file is not found", test_shader_comp_file_not_found);

TestResult test_shader_comp_illegal_shader() {
	ID3DBlob* shader_blob = nullptr;
	HRESULT hr = compile_shader(L"test_failure.hlsl", "main", ShaderType::Pixel, &shader_blob);
	assert_not_equal(hr, hr_file_not_found);
	assert_not_equal(hr, hr_dir_not_found);
	assert_not_equal(hr, S_OK);
	assert_equal(hr, E_FAIL, "%d");
	assert_equal(shader_blob, nullptr, "%p");
	assert_fatal_log_hit();
	test_return();
}
DEFTEST("compiling illegal shader", "compiling an illegal shader gives an error", test_shader_comp_illegal_shader);

TestResult test_ps_shader_comp_success() {
	ID3DBlob* shader_blob;
	HRESULT hr = compile_shader(L"test_success_ps.hlsl", "main", ShaderType::Pixel, &shader_blob);
	assert_equal(hr, S_OK);
	assert_not_equal(shader_blob, nullptr);
	test_return();
}
DEFTEST("pixel shader compilation success", "compiling a legal pixel shader does not give an error", test_ps_shader_comp_success);

TestResult test_vs_shader_comp_success() {
	ID3DBlob* shader_blob;
	HRESULT hr = compile_shader(L"test_success_vs.hlsl", "main", ShaderType::Vertex, &shader_blob);
	assert_equal(hr, S_OK);
	assert_not_equal(shader_blob, nullptr);
	test_return();
}
DEFTEST("vertex shader compilation success", "compiling a legal vertex shader does not give an error", test_vs_shader_comp_success);