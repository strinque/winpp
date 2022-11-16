#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <fmt/format.h>
#include <indicators/cursor_control.hpp>
#include <indicators/progress_bar.hpp>

namespace console
{
  // Improved indicators::ProgressBar which only updates every x ms (default x=100ms)
  // with new style and progress-bar suffix such as "13% [25/200]"
  class progress_bar final
  {
  public:
    // constructor
    progress_bar(const std::string& prefix,
                 const std::size_t size,
                 const std::size_t width = 35,
                 const std::chrono::milliseconds interval = std::chrono::milliseconds(100)) :
      m_bar(),
      m_size(size),
      m_current(0),
      m_interval(interval),
      m_last_ts()
    {
      indicators::show_console_cursor(false);
      m_bar.set_option(indicators::option::BarWidth(width));
      m_bar.set_option(indicators::option::Start("["));
      m_bar.set_option(indicators::option::End("]"));
      m_bar.set_option(indicators::option::Fill("■"));
      m_bar.set_option(indicators::option::Lead(" "));
      m_bar.set_option(indicators::option::ForegroundColor(indicators::Color::white));
      m_bar.set_option(indicators::option::FontStyles(std::vector<indicators::FontStyle>{indicators::FontStyle::bold}));
      m_bar.set_option(indicators::option::PrefixText(prefix));
      m_bar.set_option(indicators::option::MaxProgress(size));
    }

    // destructor
    ~progress_bar()
    {
      if (m_size && (m_current != m_size))
      {
        m_current = m_size - 1;
        tick();
      }
      indicators::show_console_cursor(true);
    }

    // handle a tick on the progress-bar (only updated every x ms)
    void tick()
    {
      if ((++m_current == m_size) ||
          (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_last_ts) >= m_interval))
      {
        m_bar.set_option(indicators::option::PostfixText{ fmt::format("{:02}% [{}/{}]", m_current * 100 / m_size, m_current,  m_size) });
        m_bar.set_progress(m_current);
        m_last_ts = std::chrono::steady_clock::now();
      }
    }

  private:
    indicators::ProgressBar m_bar;
    std::size_t m_size;
    std::size_t m_current;
    std::chrono::milliseconds m_interval;
    std::chrono::time_point<std::chrono::steady_clock> m_last_ts;
  };
}