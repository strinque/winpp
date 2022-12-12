#pragma once
#include <string>
#include <stdbool.h>
#include <atlstr.h>
#include <windows.h>
#include <fmt/format.h>

namespace win
{
  // Provides a system-wide, recursive, named lock - raii
  // This class satisfies requirements of C++11 concept "Lockable",  i.e. can be (and should be) used with unique_lock etc.
  class system_mutex final
  {
    // Moveable, not copyable
    system_mutex(const system_mutex& other) = delete;
    system_mutex(system_mutex&& other) = default;
    system_mutex& operator=(const system_mutex& other) = delete;
    system_mutex& operator=(system_mutex&& other) = default;

  public:
    // constructor
    system_mutex(const std::string& name) :
      m_mutex()
    {
      m_mutex = CreateMutexA(nullptr, false, name.c_str());
      if (!m_mutex)
        throw std::runtime_error(fmt::format("system_mutex: CreateMutex failed failed with error: 0x{:x}", GetLastError()));
    }

    // destructor
    ~system_mutex()
    {
      if (m_mutex)
        static_cast<void>(CloseHandle(m_mutex));
    }

    void lock()
    {
      const DWORD result = WaitForSingleObject(m_mutex, INFINITE);
      switch (result)
      {
      case WAIT_ABANDONED:
      case WAIT_OBJECT_0:
        // lock obtained, but on an abandoned mutex (was not correctly released before, e.g. due to a crash)
        break;
      default:
        throw std::runtime_error(fmt::format("system_mutex: WaitForSingleObject failed failed with error: 0x{:x}", GetLastError()));
        break;
      }
    }

    void unlock()
    {
      if (!ReleaseMutex(m_mutex))
        throw std::runtime_error(fmt::format("system_mutex: ReleaseMutex failed failed with error: 0x{:x}", GetLastError()));
    }

    bool try_lock()
    {
      const DWORD result = WaitForSingleObject(m_mutex, 0);
      switch (result)
      {
      case WAIT_TIMEOUT:
        return false;
        break;
      case WAIT_OBJECT_0:
        return true;
        break;
      default:
        throw std::runtime_error(fmt::format("system_mutex: WaitForSingleObject failed failed with error: 0x{:x}", GetLastError()));
        break;
      }
    }

  private:
    HANDLE m_mutex;
  };
}