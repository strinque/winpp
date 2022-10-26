#pragma once
#include <stdbool.h>
#include <windows.h>
#include <string>
#include <memory>
#define MAX_INPUT_LENGTH  256

namespace console
{
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
    std::unique_ptr<char[]> buf = std::make_unique<char[]>(MAX_INPUT_LENGTH);
    std::unique_ptr<WCHAR[]> wbuf = std::make_unique<WCHAR[]>(MAX_INPUT_LENGTH);

    // read and convert to UTF-8
    DWORD nread;
    ReadConsoleW(hi, wbuf.get(), MAX_INPUT_LENGTH, &nread, 0);
    if (nread >= 2)
    {
      wbuf[nread - 2] = 0;  // truncate "\r\n"
      WideCharToMultiByte(CP_UTF8, 0, wbuf.get(), -1, buf.get(), MAX_INPUT_LENGTH, 0, 0);
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
