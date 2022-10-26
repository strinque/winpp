# WinPP c++ header-only libraries

## Description

A collection of modern c++ header-only libraries primarily for windows.

Multiples components have been developped:
- [x] `console.hpp`: initialize windows-console (ascii escape sequence, resize, handle utf8)

## Installation

The `winpp` library can be used using modern cmake:

- copy `vcpkg/ports/winpp` directory to your vcpkg ports directory
- install `winpp` using `vcpkg.exe install winpp`

To import the library in cmake:
```cmake
    find_package(winpp CONFIG REQUIRED)
    target_link_libraries(main PRIVATE winpp::winpp)
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

// initialize windows console with utf8 encoding without changing console size
console::init();

// initialize windows console with utf8 encoding and change size to 1080x620
console::init(true, 1080, 620);
```