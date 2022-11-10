#pragma once
#include <string>
#include <filesystem>
#include <stdbool.h>
#include <windows.h>
#include <fmt/format.h>
#include <fmt/color.h>

namespace win
{
  // execute a windows process - blocking
  inline int execute(const std::string& cmd,
                     const std::filesystem::path& working_directory = std::filesystem::current_path(),
                     std::string& logs = std::string(),
                     const int default_error_code = -1)
  {
    // initialize handles
    HANDLE stdout_rd = nullptr;
    HANDLE stdout_wr = nullptr;

    // setup members of PROCESS_INFORMATION
    PROCESS_INFORMATION pi{};

    // clear logs
    logs.clear();
    
    // execute the process - using pipes to read output
    int exit_code = default_error_code;
    try
    {
      // check current working directory
      if (!std::filesystem::is_directory(working_directory))
        throw std::runtime_error(fmt::format("working directory isn't valid: \"{}\"", working_directory.string()));

      // set the security attributes to be inherited by child process
      SECURITY_ATTRIBUTES sa{};
      sa.nLength = sizeof(SECURITY_ATTRIBUTES);
      sa.bInheritHandle = true;
      sa.lpSecurityDescriptor = nullptr;

      // create a pipe for the child process's STDOUT 
      if (!CreatePipe(&stdout_rd, &stdout_wr, &sa, 0))
        throw std::runtime_error(fmt::format("CreatePipe failed with error: 0x{:x}", GetLastError()));

      // ensure the read handle to the pipe for STDOUT is not inherited.
      if (!SetHandleInformation(stdout_rd, HANDLE_FLAG_INHERIT, 0))
        throw std::runtime_error(fmt::format("SetHandleInformation failed with error: 0x{:x}", GetLastError()));

      // set utf8 encoding input/output for python
      if (!SetEnvironmentVariableA("PYTHONIOENCODING", "utf8"))
        throw std::runtime_error(fmt::format("SetEnvironmentVariable failed with error: 0x{:x}", GetLastError()));

      // setup members of STARTUPINFO
      STARTUPINFOA si{};
      si.cb = sizeof(si);
      si.hStdInput = reinterpret_cast<HANDLE>(-1);// GetStdHandle(STD_INPUT_HANDLE);
      si.hStdOutput = stdout_wr;
      si.hStdError = stdout_wr;
      si.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
      si.wShowWindow = SW_HIDE;

      // create the child process
      std::string args = cmd;
      if (!CreateProcessA(nullptr,                             // no module name
                          args.data(),                         // command line 
                          nullptr,                             // process security attributes 
                          nullptr,                             // primary thread security attributes 
                          true,                                // handles are inherited 
                          CREATE_NO_WINDOW,                    // creation flags 
                          nullptr,                             // use parent's environment 
                          working_directory.string().c_str(),  // set working directory 
                          &si,                                 // pointer to STARTUPINFO structure 
                          &pi))                                // pointer to PROCESS_INFORMATION structure
        throw std::runtime_error(fmt::format("CreateProcess failed with error: 0x{:x}", GetLastError()));

      // close pipes we don't need anymore
      CloseHandle(stdout_wr);
      stdout_wr = nullptr;

      // read buffer from pipe - until the other end has been broken
      DWORD len = 0;
      char buf[1024];
      while (ReadFile(stdout_rd, buf, sizeof(buf) - 1, &len, nullptr) && len)
      {
        buf[len] = 0;
        logs += buf;
      }
      if (GetLastError() != ERROR_SUCCESS && 
          GetLastError() != ERROR_BROKEN_PIPE)
        throw std::runtime_error(fmt::format("ReadFile failed with error: 0x{:x}", GetLastError()));

      // read exit code
      DWORD res;
      WaitForSingleObject(pi.hProcess, INFINITE);
      if (!GetExitCodeProcess(pi.hProcess, &res))
        throw std::runtime_error(fmt::format("GetExitCodeProcess failed with error: 0x{:x}", GetLastError()));
      exit_code = static_cast<int>(res);
    }
    catch (const std::exception& ex)
    {
      logs = fmt::format("{} {}",
              fmt::format(fmt::fg(fmt::color::red) | fmt::emphasis::bold, "error: "),
              ex.what());
    }

    // close process and thread handles
    if(pi.hProcess) CloseHandle(pi.hProcess);
    if(pi.hThread) CloseHandle(pi.hThread);

    // close pipes
    if (stdout_rd) CloseHandle(stdout_rd);
    if (stdout_wr) CloseHandle(stdout_wr);

    // return exit code
    return exit_code;
  }
}