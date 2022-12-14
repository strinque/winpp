#pragma once
#include <string>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <stdbool.h>
#include <windows.h>

namespace console
{
  // Implementation of a progress-bar designed to work within NamedPipes (CreateProcess etc.)
  //   - doesn't use terminal/console features which breaks ReadFile in NamedPipes
  //   - only updates every x ms (default x=100ms)
  //   - automatic suffix with format "13% [25/200]"
  class progress_bar final
  {
  public:
    // constructor
    progress_bar(const std::string& prefix,
                 const std::size_t max_progress,
                 const std::size_t bar_width = 40,
                 const std::chrono::milliseconds refresh_interval = std::chrono::milliseconds{ 100 },
                 const std::string& start = "[",
                 const std::string& fill = "■",
                 const std::string& empty = " ",
                 const std::string& end = "]") :
      m_prefix(prefix),
      m_max_progress(max_progress),
      m_bar_width(bar_width),
      m_refresh_interval(refresh_interval),
      m_start(start),
      m_fill(fill),
      m_empty(empty),
      m_end(end),
      m_progress(),
      m_last_ts(),
      m_completed(false)
    {
      show_console_cursor(false);
    }

    // destructor
    ~progress_bar()
    {
      if (!m_completed)
      {
        m_progress = m_max_progress - 1;
        tick();
      }
      show_console_cursor(true);
    }

    // handle a tick on the progress-bar (only updated every x ms)
    void tick()
    {
      // avoid division by zero error when the progress-bar isn't properly initialized
      if (!m_max_progress)
        return;

      if (!m_completed)
      {
        // check for completion
        if (++m_progress == m_max_progress)
          m_completed = true;

        // display progress-bar
        if (m_completed || std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_last_ts) >= m_refresh_interval)
        {
          const std::size_t fill_length = (m_bar_width - 2) * m_progress / m_max_progress;
          const std::size_t empty_length = (m_bar_width - 2) - fill_length;
          std::cout << "\r";
          std::cout << m_prefix << " ";
          std::cout << m_start;
          for (int i = 0; i < fill_length; ++i)
            std::cout << m_fill;
          for (int i = 0; i < empty_length; ++i)
            std::cout << m_empty;
          std::cout << m_end << " ";
          std::cout << std::setfill('0') << std::setw(2) << std::to_string(m_progress * 100 / m_max_progress) << "% ";
          std::cout << "[" << std::to_string(m_progress) << "/" << std::to_string(m_max_progress) << "]";
          std::cout << std::flush;
          m_last_ts = std::chrono::steady_clock::now();
        }

        if (m_completed)
          std::cout << std::endl;
      }
    }

  private:
    // set the console cursor visibility
    void show_console_cursor(const bool show)
    {
      HANDLE hdl = GetStdHandle(STD_OUTPUT_HANDLE);
      CONSOLE_CURSOR_INFO cursor_info;
      GetConsoleCursorInfo(hdl, &cursor_info);
      cursor_info.bVisible = show;
      SetConsoleCursorInfo(hdl, &cursor_info);
    }

  private:
    // progress-bar default settings
    std::string m_prefix;
    std::size_t m_max_progress;
    std::size_t m_bar_width;
    std::chrono::milliseconds m_refresh_interval;
    std::string m_start;
    std::string m_fill;
    std::string m_empty;
    std::string m_end;

    // control progress-bar progression
    std::size_t m_progress;
    std::chrono::time_point<std::chrono::steady_clock> m_last_ts;
    bool m_completed;
  };
}