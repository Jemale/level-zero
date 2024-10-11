/*
 *
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#ifdef _WIN32
#include <windows.h>
#endif

#include "gtest/gtest.h"

#include "loader/ze_loader.h"
#include "ze_api.h"

#if defined(_WIN32)
#define putenv_safe _putenv
#else
#define putenv_safe putenv
#endif

namespace {

TEST(
    LoaderAPI,
    GivenLevelZeroLoaderPresentWhenCallingzeGetLoaderVersionsAPIThenValidVersionIsReturned) {

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInit(0));

  size_t size = 0;
  EXPECT_EQ(ZE_RESULT_SUCCESS, zelLoaderGetVersions(&size, nullptr));
  EXPECT_GT(size, 0);

  std::vector<zel_component_version_t> versions(size);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zelLoaderGetVersions(&size, versions.data()));

  std::cout << "Found " << versions.size() << " versions" << std::endl;
  std::cout << std::endl;
  const std::string loader_name = "loader";
  for (auto &component : versions) {
    std::cout << "component.component_name: " << component.component_name
              << std::endl;
    std::cout << "component.component_lib_version.major: "
              << component.component_lib_version.major << std::endl;
    std::cout << "component.spec_version: " << component.spec_version
              << std::endl;
    std::cout << "component.component_lib_name: " << component.component_name
              << std::endl;
    std::cout << std::endl;

    if (loader_name == component.component_name) {
      EXPECT_GE(component.component_lib_version.major, 1);
    }
  }
}

TEST(
    LoaderInit,
    GivenLevelZeroLoaderPresentWhenCallingZeInitDriversWithGPUTypeThenExpectPassWithGPUorAllOnly) {

  uint32_t pCount = 0;
  ze_init_driver_type_desc_t desc = {ZE_STRUCTURE_TYPE_INIT_DRIVER_TYPE_DESC};
  desc.flags = ZE_INIT_DRIVER_TYPE_FLAG_GPU;
  desc.pNext = nullptr;
  putenv_safe(const_cast<char *>("ZEL_TEST_NULL_DRIVER_TYPE=GPU"));
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInitDrivers(&pCount, nullptr, &desc));
  EXPECT_GT(pCount, 0);
  desc.flags = UINT32_MAX;
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInitDrivers(&pCount, nullptr, &desc));
  EXPECT_GT(pCount, 0);
  desc.flags = ZE_INIT_DRIVER_TYPE_FLAG_GPU | ZE_INIT_DRIVER_TYPE_FLAG_NPU;
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInitDrivers(&pCount, nullptr, &desc));
  EXPECT_GT(pCount, 0);
}

TEST(
    LoaderInit,
    GivenLevelZeroLoaderPresentWhenCallingZeInitDriversWithNPUTypeThenExpectPassWithNPUorAllOnly) {

  uint32_t pCount = 0;
  ze_init_driver_type_desc_t desc = {ZE_STRUCTURE_TYPE_INIT_DRIVER_TYPE_DESC};
  desc.flags = ZE_INIT_DRIVER_TYPE_FLAG_NPU;
  desc.pNext = nullptr;
  putenv_safe(const_cast<char *>("ZEL_TEST_NULL_DRIVER_TYPE=NPU"));
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInitDrivers(&pCount, nullptr, &desc));
  EXPECT_GT(pCount, 0);
  desc.flags = UINT32_MAX;
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInitDrivers(&pCount, nullptr, &desc));
  EXPECT_GT(pCount, 0);
  desc.flags = ZE_INIT_DRIVER_TYPE_FLAG_GPU | ZE_INIT_DRIVER_TYPE_FLAG_NPU;
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInitDrivers(&pCount, nullptr, &desc));
  EXPECT_GT(pCount, 0);
}

TEST(
    LoaderInit,
    GivenLevelZeroLoaderPresentWhenCallingZeInitDriversWithAnyTypeWithNullDriverAcceptingAllThenExpectatLeast1Driver) {

  uint32_t pCount = 0;
  ze_init_driver_type_desc_t desc = {ZE_STRUCTURE_TYPE_INIT_DRIVER_TYPE_DESC};
  desc.flags = ZE_INIT_DRIVER_TYPE_FLAG_NPU;
  desc.pNext = nullptr;
  putenv_safe(const_cast<char *>("ZEL_TEST_NULL_DRIVER_TYPE=ALL"));
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInitDrivers(&pCount, nullptr, &desc));
  EXPECT_GT(pCount, 0);
  desc.flags = UINT32_MAX;
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInitDrivers(&pCount, nullptr, &desc));
  EXPECT_GT(pCount, 0);
  desc.flags = ZE_INIT_DRIVER_TYPE_FLAG_GPU;
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInitDrivers(&pCount, nullptr, &desc));
  EXPECT_GT(pCount, 0);
  desc.flags = ZE_INIT_DRIVER_TYPE_FLAG_GPU | ZE_INIT_DRIVER_TYPE_FLAG_NPU;
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInitDrivers(&pCount, nullptr, &desc));
  EXPECT_GT(pCount, 0);
}

TEST(
    LoaderInit,
    GivenLevelZeroLoaderPresentWhenCallingZeInitDriversThenzeInitThenBothCallsSucceedWithAllTypes) {

  uint32_t pCount = 0;
  ze_init_driver_type_desc_t desc = {ZE_STRUCTURE_TYPE_INIT_DRIVER_TYPE_DESC};
  desc.flags = UINT32_MAX;
  desc.pNext = nullptr;
  putenv_safe(const_cast<char *>("ZEL_TEST_NULL_DRIVER_TYPE=ALL"));
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInitDrivers(&pCount, nullptr, &desc));
  EXPECT_GT(pCount, 0);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInit(0));
}

TEST(
    LoaderInit,
    GivenLevelZeroLoaderPresentWhenCallingZeInitDriversThenzeInitThenBothCallsSucceedWithGPUTypes) {

  uint32_t pCount = 0;
  ze_init_driver_type_desc_t desc = {ZE_STRUCTURE_TYPE_INIT_DRIVER_TYPE_DESC};
  desc.flags = UINT32_MAX;
  desc.pNext = nullptr;
  putenv_safe(const_cast<char *>("ZEL_TEST_NULL_DRIVER_TYPE=GPU"));
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInitDrivers(&pCount, nullptr, &desc));
  EXPECT_GT(pCount, 0);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInit(ZE_INIT_FLAG_GPU_ONLY));
}

TEST(
    LoaderInit,
    GivenLevelZeroLoaderPresentWhenCallingZeInitDriversThenzeInitThenBothCallsSucceedWithNPUTypes) {

  uint32_t pCount = 0;
  ze_init_driver_type_desc_t desc = {ZE_STRUCTURE_TYPE_INIT_DRIVER_TYPE_DESC};
  desc.flags = UINT32_MAX;
  desc.pNext = nullptr;
  putenv_safe(const_cast<char *>("ZEL_TEST_NULL_DRIVER_TYPE=NPU"));
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInitDrivers(&pCount, nullptr, &desc));
  EXPECT_GT(pCount, 0);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInit(ZE_INIT_FLAG_VPU_ONLY));
}

TEST(
    LoaderInit,
    GivenLevelZeroLoaderPresentWhenCallingzeInitThenZeInitDriversThenBothCallsSucceedWithAllTypes) {

  uint32_t pCount = 0;
  ze_init_driver_type_desc_t desc = {ZE_STRUCTURE_TYPE_INIT_DRIVER_TYPE_DESC};
  desc.flags = UINT32_MAX;
  desc.pNext = nullptr;
  putenv_safe(const_cast<char *>("ZEL_TEST_NULL_DRIVER_TYPE=ALL"));
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInit(0));
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInitDrivers(&pCount, nullptr, &desc));
  EXPECT_GT(pCount, 0);
}

TEST(
    LoaderInit,
    GivenLevelZeroLoaderPresentWhenCallingzeInitThenZeInitDriversThenBothCallsSucceedWithGPUTypes) {

  uint32_t pCount = 0;
  ze_init_driver_type_desc_t desc = {ZE_STRUCTURE_TYPE_INIT_DRIVER_TYPE_DESC};
  desc.flags = UINT32_MAX;
  desc.pNext = nullptr;
  putenv_safe(const_cast<char *>("ZEL_TEST_NULL_DRIVER_TYPE=GPU"));
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInit(ZE_INIT_FLAG_GPU_ONLY));
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInitDrivers(&pCount, nullptr, &desc));
  EXPECT_GT(pCount, 0);
}

TEST(
    LoaderInit,
    GivenLevelZeroLoaderPresentWhenCallingzeInitThenZeInitDriversThenBothCallsSucceedWithNPUTypes) {

  uint32_t pCount = 0;
  ze_init_driver_type_desc_t desc = {ZE_STRUCTURE_TYPE_INIT_DRIVER_TYPE_DESC};
  desc.flags = UINT32_MAX;
  desc.pNext = nullptr;
  putenv_safe(const_cast<char *>("ZEL_TEST_NULL_DRIVER_TYPE=NPU"));
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInit(ZE_INIT_FLAG_VPU_ONLY));
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInitDrivers(&pCount, nullptr, &desc));
  EXPECT_GT(pCount, 0);
}

#ifdef WIN32
int run_windows_test() {
  HANDLE hRead, hWrite;
  SECURITY_ATTRIBUTES saAttr = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};

  if (!CreatePipe(&hRead, &hWrite, &saAttr, 0)) {
    std::cerr << "CreatePipe failed\n";
    return 1;
  }

  if (!SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0)) {
    std::cerr << "SetHandleInformation failed\n";
    return 1;
  }

  PROCESS_INFORMATION pi;
  STARTUPINFO si;

  ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
  ZeroMemory(&si, sizeof(STARTUPINFO));

  si.cb = sizeof(STARTUPINFO);
  si.hStdError = hWrite;
  si.hStdOutput = hWrite;
  si.dwFlags |= STARTF_USESTDHANDLES;

  if (!CreateProcess("zello_world",  // App Name
                     nullptr, // Command Line
                     nullptr, // Process handle not inherited
                     nullptr, // Thread handle not inherited
                     TRUE,    // Set handle inheritance to TRUE
                     0,       // No create flags
                     nullptr, // Use parent's environment block
                     nullptr, // Use Parent's starting directory
                     &si,     // Pointer to STARTUPINFO structure
                     &pi)     // Pointer to PROCESS_INFORMATION structure
  ) {
    std::cerr << "CreateProcess failed\n";
    return 1;
  }

  if (!CloseHandle(hWrite)) {
    std::cerr << "CloseHandle Write failed\n";
    return 1;
  }

  DWORD waitResult = WaitForSingleObject(pi.hProcess, INFINITE);
  if (waitResult != WAIT_OBJECT_0) {
    std::cerr << "WaitForSingleObject failed\n";
    return 1;
  }

  char buffer[4096] = {};
  DWORD bytesRead;
  std::vector<char> output;
  while (ReadFile(hRead, buffer, sizeof(buffer) - 1, &bytesRead, nullptr)) {
    output.insert(output.end(), buffer, buffer + sizeof(buffer));
  }

  std::string result_string(output.begin(), output.end());
  EXPECT_EQ(result_string, "");
  

  if (!CloseHandle(pi.hProcess)) {
    std::cerr << "CloseHandle Process failed\n";
    return 1;
  }
  if (!CloseHandle(pi.hThread)) {
    std::cerr << "CloseHandle Thread failed\n";
    return 1;
  }

  return 0;
}
#endif

#ifdef __linux__
int run_linux_test() {
  // start a level zero application in a new process
  //  redirect stdout and stderr to a file
  //  check that the loader does not emit any output

  int pipefd[2];
  if (pipe(pipefd) == -1) {
    std::cerr << "Pipe failed\n";
    return 1;
  }

  pid_t pid = fork();

  if (pid == 0) {
    std::cout << "Child Started\n";

    // child process
    // redirect stdout and stderr to a file
    // exec the level zero application
    if (close(pipefd[0]) == -1) {
      std::cerr << "Close failed\n";
      return 1;
    }
    dup2(pipefd[1], STDOUT_FILENO);

    dup2(pipefd[1], STDERR_FILENO);
    if (close(pipefd[1]) == -1) {
      std::cerr << "Close failed\n";
      exit(1);
    }

    if (execlp("true", "true", nullptr) == -1) {
      std::cerr << "Exec failed\n";
      exit(1);
    }

  } else if (pid > 0) {
    // parent process
    // wait for the child process to finish
    // check that the loader does not emit any output
    if (close(pipefd[1]) == -1) {
      std::cerr << "Close failed\n";
      return 1;
    }

    char buffer[4096] = {};
    std::vector<char> output;
    size_t count;
    while ((count = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
      output.insert(output.end(), buffer, buffer + count);
    }

    if (close(pipefd[0]) == -1) {
      std::cerr << "Close failed\n";
      return 1;
    }

    std::string result_string(output.begin(), output.end());
    std::cout << "Output: " << result_string << std::endl;
    EXPECT_EQ(result_string, "");

    int status = 0;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
      if (WEXITSTATUS(status) != 0) {
        std::cerr << "Child process failed\n";
        return 1;
      }
    } else {
      std::cerr << "Child process failed\n";
      return 1;
    }
  } else {
    std::cerr << "Fork failed\n";
    return 1;
  }

  return 0;
}
#endif

TEST(
    LoaderTest,
    GivenApplicationUsingLevelZeroLoaderWhenExecutingThenLoaderDoesNotEmitOutput) {

  int result = 0;
#ifdef WIN32
  result = run_windows_test();
#else
  result = run_linux_test();

#endif
  EXPECT_EQ(0, result);
}

} // namespace
