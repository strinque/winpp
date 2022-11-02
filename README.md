# WinPP c++ header-only libraries

## Description

A collection of modern c++ header-only libraries primarily for windows.

Multiples components have been developped:
- [x] `console.hpp`: initialize windows-console (ascii escape sequence, resize, handle utf8)
- [x] `utf8.hpp`: handle utf8 convertion using windows api (faster than STL)
- [x] `parser.hpp`: parse the command-line arguments
- [x] `progress-bar.hpp`: improved progress-bar based on indicators::ProgressBar
- [x] `files.hpp`: set of functions to handle files

## Installation

The `winpp` library can be used using modern cmake:

- copy `vcpkg/ports/winpp` directory to your vcpkg ports directory
- install `winpp` using `vcpkg.exe install winpp`

To import the library in cmake:
```cmake
    find_package(fmt CONFIG REQUIRED)
    find_package(indicators CONFIG REQUIRED)
    find_package(winpp CONFIG REQUIRED)
    target_link_libraries(main PRIVATE 
      indicators::indicators
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

### progress-bar

The header-only `winpp::ProgressBar` class is an enhanced *progress-bar* based on `indicators::ProgressBar`.  
It only updates every **100ms** instead of trying to display every elements and is designed with RAII.

Features:

- [x] add a prefix to the progress-bar
- [x] enhanced styling to display progress-bar progression
- [x] automatically set the suffix with format: "xx% [xx/yy]"
- [x] doesn't update more than once every 100ms
- [x] uses RAII => initializes in constructor, clears properly in destructor
- [x] based on indicators::ProgressBar

```cpp
#include <vector>
#include <thread>
#include <chrono>
#include <winpp/console.hpp>
#include <winpp/progress-bar.hpp>

int main(int argc, char** argv)
{
  console::init();
  
  std::vector<int> vec(500, 1);
  console::ProgressBar progress_bar("testing: ", vec.size());
  for (const auto& v : vec)
  {
    progress_bar.tick();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  return 0;
}
```

### files

A set of functions to handle files:

- [x] retrieve a list of files in a directory and its sub-directories
- [x] retrieve a list of directories in a directory and its sub-directories
- [x] use std::regex to filter files/directories
- [x] generic std::function to filter files/directories using custom rules
- [x] compute the SHA-256 **hash** of a file using `haspp` header-only library
- [x] read the `ctime`, `atime`, `mtime` of a file
- [x] set the `ctime`, `atime`, `mtime` of a file

<h3><code>get_dirs</code></h3>
Get all directories in directory and sub-directories with `std::function` filtering. 
Arguments: 

- `path`: directory to analyze
- `dir_filter`: `std::function` that determines if the directory is taken (***true***) or not (***false***)

```cpp
// get all directories and sub-directories with filtering (using a std::function)
const std::vector<std::filesystem::path> get_dirs(const std::filesystem::path& path,
                                                  const std::function<bool(const std::filesystem::path&)>& dir_filter)
```

Get all directories in directory and sub-directories with `std::regex` filtering.
Arguments:

- `path`: directory to analyze
- `dir_regex`: `std::regex` that determines if the directory is taken (***true***) or not (***false***)
- `skip_files`: exclude a list of directories (default: skip none)

```cpp
// get all directories and sub-directories with filtering (using a std::regex, fullpath for dir)
const std::vector<std::filesystem::path> get_dirs(const std::filesystem::path& path,
                                                  const std::regex& dir_regex = std::regex(R"(.*)"),
                                                  const std::vector<std::filesystem::path>& skip_dirs = {})
```

<h3><code>get_files</code></h3>
Get all files in directory and sub-directories with `std::function` filtering.  
Arguments:

- `path`: directory to analyze
- `dir_filter`: `std::function` that determines if the directory is taken (***true***) or not (***false***)
- `file_filter`: `std::function` that determines if the filename is taken (***true***) or not (***false***)

```cpp
// get all files on a directory and its sub-directories with filtering (using a std::function)
const std::vector<std::filesystem::path> get_files(const std::filesystem::path& path,
                                                   const std::function<bool(const std::filesystem::path&)>& dir_filter,
                                                   const std::function<bool(const std::filesystem::path&)>& file_filter)
```

Get all files in directory and sub-directories with `std::regex` filtering.  
Arguments:

- `path`: directory to analyze
- `dir_regex`: `std::function` that determines if the directory is taken (***true***) or not (***false***)
- `file_regex`: `std::function` that determines if the filename is taken (***true***) or not (***false***)
- `skip_dirs`: exclude a list of directories (default: skip none)
- `skip_files`: exclude a list of files (default: skip none)

```cpp
// get all files on a directory and its sub-directories with filtering (using a std::regex, fullpath for dir, filename with extension for file)
inline const std::vector<std::filesystem::path> get_files(const std::filesystem::path& path,
                                                          const std::regex& dir_regex,
                                                          const std::regex& file_regex,
                                                          const std::vector<std::filesystem::path>& skip_dirs = {},
                                                          const std::vector<std::filesystem::path>& skip_files = {})
```

Get all files in directory and sub-directories (***recursive*** = true) or only in the directory (***recursive*** = false) with `std::regex` filtering.  
Arguments:

- `path`: directory to analyze
- `recursive`: get files recursively (***true***) or not (***false***)
- `file_regex`: `std::function` that determines if the filename is taken (***true***) or not (***false***)
- `skip_files`: exclude a list of files (default: skip none)

```cpp

  // get all files on a directory and sub-directories (if recursive flag is true) with filtering on filename (by regex)
  inline const std::vector<std::filesystem::path> get_files(const std::filesystem::path& path,
                                                            const bool recursive = true,
                                                            const std::regex& file_regex = std::regex(R"(.*)"),
                                                            const std::vector<std::filesystem::path>& skip_files = {})
```

<h3><code>get_hash</code></h3>
Get the sha-256 hash of a file using hashpp header-only library.

```cpp
std::string files::get_hash(const std::filesystem::path& file);
```

<h3>Exemple</h3>

```cpp
#include <iostream>
#include <winpp/files.hpp>

const std::string directory = "C:\\Windows\\Boot";
const std::string file = "C:\\Windows\\Boot\\BootDebuggerFiles.ini";

int main(int argc, char** argv)
{
  {
    // list all directories and sub-directories
    //  using std::regex filter (path containing "\DVD")
    const std::vector<std::filesystem::path>& dirs = files::get_dirs(directory, std::regex(R"(\\DVD)"));
    for (const auto& d : dirs)
      std::cout << d.string() << std::endl;
    std::cout << std::endl;
  }

  {
    // list all files in directory and sub-directories
    //   using std::regex filter (filename finishing with ".exe")
    const std::vector<std::filesystem::path>& files = files::get_files(directory, true, std::regex(R"(\.exe$)"));
    for (const auto& f : files)
      std::cout << f.string() << std::endl;
    std::cout << std::endl;
  }

  {
    // list all files in directory and sub-directories
    //   using std::function filters (directories containing "PCAT", filenames less than 12 bytes finishing with ".dll")
    const std::string dir = "PCAT";
    const auto& dir_filter = [&](const std::filesystem::path& p) -> bool {
      return (p.string().find(dir) != std::string::npos);
    };

    const std::regex ext(R"(\.dll$)");
    const auto& file_filter = [&](const std::filesystem::path& p) -> bool {
      return p.filename().string().size() < 14 &&
             std::regex_search(p.filename().string(), ext);
             
    };

    const std::vector<std::filesystem::path>& files = files::get_files(directory, dir_filter, file_filter);
    for (const auto& f : files)
      std::cout << f.string() << std::endl;
    std::cout << std::endl;
  }
  
  // get the sha-256 hash of a file using hashpp header-only library
  std::cout << "hash for: \"" << file << "\" = " << files::get_hash(file) << std::endl;
  return 0;
}
```