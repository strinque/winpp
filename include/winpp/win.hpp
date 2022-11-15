#pragma once
#include <string>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include <chrono>
#include <functional>
#include <filesystem>
#include <stdbool.h>
#include <windows.h>
#include <fmt/format.h>
#include <fmt/color.h>

namespace win
{
  // polling period to read the console output
  constexpr std::chrono::milliseconds polling_period(100);
  
  // class to handle the execution of windows process
  class process
  {
  public:
    // constructor
    process() :
      m_mutex(),
      m_cv(),
      m_running(false),
      m_timeout(std::chrono::milliseconds::max()),
      m_working_dir(std::filesystem::current_path()),
      m_default_error_code(-1),
      m_last_ts(),
      m_stdin_rd(),
      m_stdin_wr(),
      m_stdout_rd(),
      m_stdout_wr(),
      m_pi(),
      m_sa(),
      m_si()
    {
    }

    // destructor
    virtual ~process()
    {
      close_process();
    }

    // setters
    void set_timeout(const std::chrono::milliseconds& timeout) noexcept { m_timeout = timeout; }
    void set_default_error_code(const int default_error_code = -1) noexcept { m_default_error_code = default_error_code; }
    void set_working_dir(const std::filesystem::path& working_dir) noexcept { m_working_dir = working_dir; }

    // terminate the process
    void stop()
    {
      std::lock_guard<std::mutex> lck(m_mutex);

      // terminate the windows process
      if (m_pi.hProcess)
        TerminateProcess(m_pi.hProcess, m_default_error_code);

      // stop reading the output
      m_running = false;
      m_cv.notify_all();
    }

  protected:
    // initialize and create a windows process
    void create_process(const std::string& cmd)
    {
      std::lock_guard<std::mutex> lck(m_mutex);

      // initialize handles
      m_stdin_rd = nullptr;
      m_stdin_wr = nullptr;
      m_stdout_rd = nullptr;
      m_stdout_wr = nullptr;

      // setup members of PROCESS_INFORMATION
      m_pi = PROCESS_INFORMATION{};

      // set the security attributes to be inherited by child process
      m_sa = SECURITY_ATTRIBUTES{};
      m_sa.nLength = sizeof(SECURITY_ATTRIBUTES);
      m_sa.bInheritHandle = true;
      m_sa.lpSecurityDescriptor = nullptr;

      // check the validity of the working directory
      if (!std::filesystem::is_directory(m_working_dir))
        throw std::runtime_error(fmt::format("working directory isn't valid: \"{}\"", m_working_dir.string()));

      // create a pipe for the child process's STDOUT
      if (!CreatePipe(&m_stdout_rd, &m_stdout_wr, &m_sa, 0))
        throw std::runtime_error(fmt::format("CreatePipe for stdout failed with error: 0x{:x}", GetLastError()));

      // ensure the read handle to the pipe for STDOUT is not inherited
      if (!SetHandleInformation(m_stdout_rd, HANDLE_FLAG_INHERIT, 0))
        throw std::runtime_error(fmt::format("SetHandleInformation for stdout failed with error: 0x{:x}", GetLastError()));

      // create a pipe for the child process's STDIN
      if (!CreatePipe(&m_stdin_rd, &m_stdin_wr, &m_sa, 0))
        throw std::runtime_error(fmt::format("CreatePipe for stdin failed with error: 0x{:x}", GetLastError()));

      // ensure the write handle to the pipe for STDIN is not inherited
      if (!SetHandleInformation(m_stdin_wr, HANDLE_FLAG_INHERIT, 0))
        throw std::runtime_error(fmt::format("SetHandleInformation for stdin failed with error: 0x{:x}", GetLastError()));

      // set utf8 encoding input/output for the console
      if (!SetEnvironmentVariableA("OutputEncoding", "utf8"))
        throw std::runtime_error(fmt::format("SetEnvironmentVariable: \"{}\" failed with error: 0x{:x}", "OutputEncoding", GetLastError()));

      // set utf8 encoding input/output for python
      if (!SetEnvironmentVariableA("PYTHONIOENCODING", "utf8"))
        throw std::runtime_error(fmt::format("SetEnvironmentVariable: \"{}\" failed with error: 0x{:x}", "PYTHONIOENCODING", GetLastError()));

      // setup members of STARTUPINFO
      m_si = STARTUPINFOA{};
      m_si.cb = sizeof(m_si);
      m_si.hStdInput = m_stdin_rd;
      m_si.hStdOutput = m_stdout_wr;
      m_si.hStdError = m_stdout_wr;
      m_si.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
      m_si.wShowWindow = SW_HIDE;

      // save the starting timestamp
      m_last_ts = std::chrono::steady_clock::now();

      // create the child process
      std::string args = cmd;
      m_running = true;
      if (!CreateProcessA(nullptr,                             // no module name
                          args.data(),                         // command line 
                          nullptr,                             // process security attributes 
                          nullptr,                             // primary thread security attributes 
                          true,                                // handles are inherited 
                          CREATE_NO_WINDOW,                    // creation flags 
                          nullptr,                             // use parent's environment 
                          m_working_dir.string().c_str(),      // set working directory 
                          &m_si,                               // pointer to STARTUPINFO structure 
                          &m_pi))                              // pointer to PROCESS_INFORMATION structure
        throw std::runtime_error(fmt::format("CreateProcess failed with error: 0x{:x}", GetLastError()));

      // close pipes we don't need anymore
      close_handle(m_stdout_wr);
      close_handle(m_stdin_rd);
    }

    // read periodically the stdout of the process until termination
    int read_process(std::function<void(const std::string&)> cb_logs)
    {
      // lambda function to log
      auto& log = [cb_logs](const std::string& logs) {
        if (!logs.empty())
          cb_logs(logs);
      };

      DWORD exit_code = 0;
      do
      {
        // read stdout of the current running process
        log(read_from_pipe(m_stdout_rd));

        // retrieve the exit-code of the process
        {
          std::lock_guard<std::mutex> lck(m_mutex);
          if (!GetExitCodeProcess(m_pi.hProcess, &exit_code))
            break;
        }

        if (m_running && (exit_code == STILL_ACTIVE))
        {
          // wait for notification or wakeup periodically to send logs
          std::unique_lock<std::mutex> lck(m_mutex);
          if (m_cv.wait_for(lck, polling_period) != std::cv_status::timeout && !m_running)
            GetExitCodeProcess(m_pi.hProcess, &exit_code);

          // check if timeout has occured
          if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_last_ts) >= m_timeout)
          {
            log(read_from_pipe(m_stdout_rd));
            m_running = false;
            throw std::runtime_error(fmt::format("timeout of {}ms triggered: process stopped", m_timeout.count()));
          }
        }
      } while (m_running && (exit_code == STILL_ACTIVE));

      // send stdout logs and exit-code to client
      log(read_from_pipe(m_stdout_rd));
      m_running = false;
      return static_cast<int>(exit_code);
    }

    // close all handles and pipes
    void close_process() noexcept
    {
      std::lock_guard<std::mutex> lck(m_mutex);

      // close process and thread handles
      close_handle(m_pi.hProcess);
      close_handle(m_pi.hThread);

      // close pipes
      close_handle(m_stdin_rd);
      close_handle(m_stdin_wr);
      close_handle(m_stdout_rd);
      close_handle(m_stdout_wr);
    }

  private:
    // read data from pipe
    const std::string read_from_pipe(HANDLE hdl) const noexcept
    {
      DWORD len = 0;
      char buf[1024];
      std::string logs;
      while (true)
      {
        // check if there are still data to retrieve from stdout
        if (!PeekNamedPipe(hdl, 0, 0, 0, &len, 0) || !len)
          break;

        // read output from the child process's pipe for STDOUT - blocking
        if (ReadFile(hdl, buf, sizeof(buf) - 1, &len, nullptr) && len)
        {
          buf[len] = 0;
          logs += buf;
        }
        if (GetLastError() != ERROR_SUCCESS &&
            GetLastError() != ERROR_BROKEN_PIPE)
          break;
      }
      return logs;
    }

    // close handle properly
    void close_handle(HANDLE& hdl) const noexcept
    {
      if (hdl)
      {
        CloseHandle(hdl);
        hdl = nullptr;
      }
    }

  protected:
    // handle the blocking part of the read sequence
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_running;

    // input parameters for the process
    std::chrono::milliseconds m_timeout;
    std::filesystem::path m_working_dir;
    int m_default_error_code;

  private:
    // to handle timeout 
    std::chrono::time_point<std::chrono::steady_clock> m_last_ts;

    // pipes between processes
    HANDLE m_stdin_rd;
    HANDLE m_stdin_wr;
    HANDLE m_stdout_rd;
    HANDLE m_stdout_wr;

    // process variables
    PROCESS_INFORMATION m_pi;
    SECURITY_ATTRIBUTES m_sa;
    STARTUPINFOA m_si;
  };

  // execute a windows process - non-blocking api
  class async_process final : public process
  {
  public:
    // constructor
    async_process() :
      m_thread(),
      m_cb_logs(),
      m_cb_exit()
    {
    }

    // destructor
    ~async_process()
    {
      // terminate the program and stop the console reading
      stop();

      // wait for the thread to properly terminate
      if (m_thread.joinable())
        m_thread.join();
    }

    // execute the process - non-blocking api
    bool execute(const std::string& cmd,
                 std::function<void(const std::string&)> cb_logs = [](const std::string&) {},
                 std::function<void(const int)> cb_exit = [](const int) {}) noexcept
    {
      try
      {
        // check if another process was running
        if (m_running)
          throw std::runtime_error("another process is already running");

        // wait for the previous thread to properly terminate
        if (m_thread.joinable())
        {
          m_thread.join();
          close_process();
        }

        // launch the windows process
        create_process(cmd);

        // create new thread to handle logs and exit-code
        m_cb_logs = cb_logs;
        m_cb_exit = cb_exit;
        m_thread = std::thread(&async_process::run, this);
        return true;
      }
      catch (const std::exception& ex)
      {
        cb_logs(fmt::format("{} {}",
          fmt::format(fmt::fg(fmt::color::red) | fmt::emphasis::bold, "error: "),
          ex.what()));
        cb_exit(m_default_error_code);
        return false;
      }
    }

  private:
    // the process is managed in a thread to avoid blocking the main thread
    void run()
    {
      try
      {
        m_cb_exit(read_process(m_cb_logs));
      }
      catch (const std::exception& ex)
      {
        m_cb_logs(fmt::format("{} {}",
          fmt::format(fmt::fg(fmt::color::red) | fmt::emphasis::bold, "error: "),
          ex.what()));
        m_cb_exit(m_default_error_code);
      }
    }

  private:
    std::thread m_thread;
    std::function<void(const std::string&)> m_cb_logs;
    std::function<void(const int)> m_cb_exit;
  };

  // execute a windows process - blocking api
  class sync_process final : public process
  {
  public:
    // constructor/destructor
    sync_process() = default;
    ~sync_process() = default;

    // execute the process - blocking api
    int execute(const std::string& cmd)
    {
      try
      {
        // clear logs
        m_logs.clear();

        // execute the process in the current thread
        create_process(cmd);
        auto& cb_logs = [&](const std::string& l) {
          m_logs += l;
        };
        const int exit_code = read_process(cb_logs);
        close_process();
        return exit_code;
      }
      catch (const std::exception& ex)
      {
        m_logs = fmt::format("{} {}",
          fmt::format(fmt::fg(fmt::color::red) | fmt::emphasis::bold, "error: "),
          ex.what());
        return m_default_error_code;
      }
    }

    // retrieve the logs
    const std::string get_logs() const { return m_logs; }

  private:
    std::string m_logs;
  };

  // execute a windows process - blocking mode
  inline int execute(const std::string& cmd,
                     std::string& logs = std::string(),
                     const std::filesystem::path& working_directory = std::filesystem::current_path(),
                     const std::chrono::milliseconds timeout = std::chrono::milliseconds::max(),
                     const int default_error_code = -1)
  {
    win::sync_process proc;
    proc.set_working_dir(working_directory);
    proc.set_timeout(timeout);
    proc.set_default_error_code(default_error_code);
    const int res = proc.execute(cmd);
    logs = proc.get_logs();
    return res;
  }
}