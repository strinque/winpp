# WinPP c++ header-only libraries

## Description

A collection of modern c++ header-only libraries primarily for windows.

Multiples components have been developped:
- [x] `console.hpp`: initialize windows-console (ascii escape sequence, resize, handle utf8)
- [x] `utf8.hpp`: handle utf8 convertion using windows api (faster than STL)
- [x] `parser.hpp`: parse the command-line arguments

## Installation

The `winpp` library can be used using modern cmake:

- copy `vcpkg/ports/winpp` directory to your vcpkg ports directory
- install `winpp` using `vcpkg.exe install winpp`

To import the library in cmake:
```cmake
    find_package(fmt CONFIG REQUIRED)
    find_package(winpp CONFIG REQUIRED)
    target_link_libraries(main PRIVATE 
      fmt::fmt-header-only
      winpp::winpp)
```

## Description

### console

Initialize Windows Console:

- [x] activate utf-8 console mode
- [x] enable escape sequence (ex: display colors)
- [x] resize console if necessary

Code:
```cpp
#include <winpp/console.hpp>

int main(int argc, char** argv)
{
  // initialize windows console with utf8 encoding without changing console size
  console::init();

  // initialize windows console with utf8 encoding and change size to 1080x620
  console::init(true, 1080, 620);
  return 0;
}
```

### utf8

Convert `std::string`/`std::wstring` from/to utf8 encoding.

Code:
```cpp
#include <winpp/console.hpp>
#include <winpp/utf8.hpp>
#include <iostream>

int main(int argc, char** argv)
{
  console::init();

  std::string str = u8"Japanese: ゴム手袋";
  std::cout << utf8::from_utf8(str) << std::endl;
  return 0;
}
```

### parser

A windows command-line parser: 

- [x] use `getopt` style to parse command-line arguments in c++
- [x] use `fmt` for formating/colors
- [x] handle boolean flags
- [x] handle text arguments
- [x] handle int/float/double arguments
- [x] handle "-h, --help" option by default (calls display usage)
- [x] handle "-v, --version" option by default
- [x] display usage automaticaly when no arguments are provided

Code:
```cpp
#include <iostream>
#include <string>
#include <stdbool.h>
#include <winpp/console.hpp>
#include <winpp/parser.hpp>

int main(int argc, char** argv)
{
  // initialize windows console
  console::init();

  // parse command-line arguments
  bool debug = false;
  std::string file;
  std::string output;
  console::parser parser("program", "1.0");
  parser.add("d", "debug", "activate the debug mode", debug); // flag (default: false)
  parser.add("f", "file", "read the file", file, true);       // std::string argument (mandatory)
  parser.add("o", "output", "set the output file", output);   // std::string argument (optional)
  if (!parser.parse(argc, argv))
  {
    parser.print_usage();
    return -1;
  }

  std::cout << "debug: " << (debug ? "true" : "false" )<< std::endl;
  std::cout << "file: " << file << std::endl;
  if(!output.empty())
    std::cout << "output: " << output << std::endl;
  return 0;
}
```

Usage (without arguments):
```bash
./program.exe
usage: program.exe [options]
  -h,  --help        display the program help
  -v,  --version     display the program version
  -d,  --debug       activate the debug mode
  -f,  --file        read the file                 [mandatory]
  -o,  --output      set the output file
```

Usage (with arguments):
```bash
./program.exe -f input.cpp --debug
debug: true
file: input.cpp
```