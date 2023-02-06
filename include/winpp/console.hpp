#pragma once
#include <string>
#include <memory>
#include <cctype>
#include <stdbool.h>
#include <windows.h>
#include <conio.h>
#include <fmt/format.h>

namespace console
{
  constexpr uint16_t max_input_length = 256;

  // initialize a Windows console
  inline void init(int width=-1, int height=-1)
  {
    // activate utf-8 console mode
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // enable escape sequence for Windows
    HANDLE hdl = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hdl, &mode);
    SetConsoleMode(hdl, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    // resize console
    if((width != -1) && (height != -1))
    {
      HWND console = GetConsoleWindow();
      RECT r;
      GetWindowRect(console, &r); // get the console's current dimensions
      MoveWindow(console, r.left, r.top, width, height, TRUE);
    }
  }
  
  // ask user for input wih [y/n] message
  inline bool ask_user(const std::string& str)
  {
    int c;
    fmt::print("{} [y/n]: ", str);
    while (true)
    {
      c = getch();
      if (std::tolower(c) == 'n')
      {
        fmt::print("n\n");
        return false;
      }
      else if (std::tolower(c) == 'y')
      {
        fmt::print("y\n");
        return true;
      }
    }
  }

  // read user input as utf8
  inline const std::string input(bool password = false)
  {
    // setup input console handle - disable echo for password
    DWORD orig = 0;
    HANDLE hi = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    GetConsoleMode(hi, &orig);
    DWORD mode = orig;
    mode |= ENABLE_PROCESSED_INPUT;
    mode |= ENABLE_LINE_INPUT;
    if (password)
      mode &= ~ENABLE_ECHO_INPUT;
    SetConsoleMode(hi, mode);

    // allocate buffers the size of the output - raii
    std::unique_ptr<char[]> buf = std::make_unique<char[]>(max_input_length);
    std::unique_ptr<WCHAR[]> wbuf = std::make_unique<WCHAR[]>(max_input_length);

    // read and convert to UTF-8
    DWORD nread;
    ReadConsoleW(hi, wbuf.get(), max_input_length, &nread, 0);
    if (nread >= 2)
    {
      wbuf[nread - 2] = 0;  // truncate "\r\n"
      WideCharToMultiByte(CP_UTF8, 0, wbuf.get(), -1, buf.get(), max_input_length, 0, 0);
    }

    // write trailing line
    if (password)
    {
      HANDLE ho = CreateFileA("CONOUT$", GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
      WriteConsoleA(ho, "\n", 1, 0, 0);
      CloseHandle(ho);
    }

    // restore console mode and close handles
    SetConsoleMode(hi, orig);
    CloseHandle(hi);

    return buf.get();
  }
}