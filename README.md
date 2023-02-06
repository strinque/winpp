# WinPP c++ header-only libraries

## Description

A collection of modern c++ header-only libraries primarily for windows.

Multiples components have been developped:
- [x] `console.hpp`: initialize windows-console (ascii escape sequence, resize, handle utf8)
- [x] `utf8.hpp`: handle utf8 convertion using windows api (faster than STL)
- [x] `parser.hpp`: parse the command-line arguments
- [x] `progress-bar.hpp`: console progress-bar which works with NamedPipes
- [x] `files.hpp`: set of functions to handle files
- [x] `win.hpp`: set of generic windows functions (ex: execute a process)
- [x] `system-mutex.hpp`: system wide named lock mutex

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

### utf8

Convert `std::string`/`std::wstring` from/to utf8 encoding.

<h3><code>utf8::to_utf8</code></h3>

Convert from extended ascii `std::string` to utf8 `std::string`.  
Arguments:

- `str`: `std::string` to convert

```cpp
// from extended ascii to utf8 string
const std::string utf8::to_utf8(const std::string& str)
```

Convert from extended ascii `std::wstring` to utf8 `std::string`.  
Arguments:

- `wstr`: `std::wstring` to convert

```cpp
// from wide unicode string to utf8 string
const std::string to_utf8(const std::wstring& wstr)
```

<h3><code>utf8::from_utf8</code></h3>

Convert from utf8 `std::string` to extended ascii `std::string`.  
Arguments:

- `str`: `std::string` to convert

```cpp
// convert from utf8 string to extended ascii
const std::string utf8::from_utf8(const std::string& str)
```

<h3><code>utf8::s2ws</code></h3>

Convert `std::string` to `std::wstring` using the selected **code-page**.  
Arguments:

- `str`: `std::string` to convert
- `code_page`: code-page used for convertion

```cpp
// convert string to wstring using the code_page
const std::wstring s2ws(const std::string& str, const UINT code_page)
```

<h3><code>utf8::ws2s</code></h3>

Convert `std::wstring` to `std::string` using the selected **code-page**.  
Arguments:

- `wstr`: `std::wstring` to convert
- `code_page`: code-page used for convertion

```cpp
// convert wstring to string using the code_page
const std::string ws2s(const std::wstring& wstr, const UINT code_page)
```

<h3>Usage</h3>

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

### console

Initialize Windows Console:

- [x] activate utf-8 console mode
- [x] enable escape sequence (ex: display colors)
- [x] resize console if necessary

<h3><code>console::init</code></h3>

Initialize a windows console for ascii escape-sequence and utf8 encoding.  
Arguments:

- `width`: set the ***width*** of the console (default: -1 = don't set)
- `height`: set the ***height*** of the console (default: -1 = don't set)

```cpp
// initialize a Windows console
void console::init(int width=-1, int height=-1)
```

<h3><code>console::ask_user</code></h3>
Function to prompt the user a question with [y/n] pattern without the need of pressing enter key.  

Arguments:

- `str`: question that needs to be asked

```cpp
// prompt user with a question
bool ask_user(const std::string& str)
```

<h3>Usage</h3>

```cpp
#include <iostream>
#include <winpp/console.hpp>

int main(int argc, char** argv)
{
  // initialize Windows console
  console::init();

  // prompt user with a question
  if (!console::ask_user("Do you want to continue"))
    std::cout << "user doesn't want to continue" << std::endl;
  return 0;
}
```

<h3><code>console::input</code></h3>

Read user input as utf8.  
Arguments:

- `password`: display `*` instead of typed characters (for password)

```cpp
// read user input as utf8
const std::string console::input(bool password = false)
```

<h3>Usage</h3>

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

<h3>Usage</h3>

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
  std::vector<float> vfloat;
  console::parser parser("program", "1.0");
  parser.add("d", "debug", "activate the debug mode", debug)  // flag (default: false)
        .add("f", "file", "read the file", file, true)        // std::string argument (mandatory)
        .add("o", "output", "set the output file", output)    // std::string argument (optional)
        .add("v", "vfloat", "list of float", vfloat);
  if (!parser.parse(argc, argv))
  {
    parser.print_usage();
    return -1;
  }

  std::cout << "debug: " << (debug ? "true" : "false" )<< std::endl;
  std::cout << "file: " << file << std::endl;
  if(!output.empty())
    std::cout << "output: " << output << std::endl;
  for (const auto& v: vfloat)
    std::cout << "float: " << v << std::endl;
  return 0;
}
```

Call without arguments:

```bash
./program.exe
usage: program.exe [options]
  -h,  --help        display the program help
  -v,  --version     display the program version
  -d,  --debug       activate the debug mode
  -f,  --file        read the file                 [mandatory]
  -o,  --output      set the output file
```

Call with arguments:

```bash
./program.exe -f input.cpp --debug --vfloat "3.2;54.9"
debug: true
file: input.cpp
float: 3.2
float: 54.9
```

### progress-bar

The header-only `winpp::progress_bar` class is an enhanced *progress-bar*.  
It only updates every **100ms** instead of trying to display every elements and is designed with RAII.

Features:

- [x] add a prefix to the progress-bar
- [x] enhanced styling to display progress-bar progression
- [x] automatically set the suffix with format: "xx% [xx/yy]"
- [x] doesn't update more than once every 100ms
- [x] uses RAII => initializes in constructor, clears properly in destructor
- [x] works with NamedPipes (doesn't use console/terminal based function)

<h3>Usage</h3>

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
  console::progress_bar progress_bar("testing:", vec.size());
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

<h3><code>files::get_dirs</code></h3>

Get all directories in directory and sub-directories with `std::function` filtering.  
Arguments: 

- `path`: directory to analyze
- `depth`: depth of the analyze (files::infinite_depth for infinite recursive)
- `dir_filter`: `std::function` that determines if the directory is taken (***true***) or not (***false***)

```cpp
// get all directories and sub-directories with filtering (using a std::function)
const std::vector<std::filesystem::path> get_dirs(const std::filesystem::path& path,
                                                  const int depth,
                                                  const std::function<bool(const std::filesystem::path&)>& dir_filter)
```

Get all directories in directory and sub-directories with `std::regex` filtering.
Arguments:

- `path`: directory to analyze
- `depth`: depth of the analyze (files::infinite_depth for infinite recursive)
- `dir_regex`: `std::regex` that determines if the directory is taken (***true***) or not (***false***)
- `skip_dirs`: exclude a list of directories (default: skip none)

```cpp
// get all directories and sub-directories with filtering (using a std::regex, fullpath for skip_dirs)
const std::vector<std::filesystem::path> get_dirs(const std::filesystem::path& path,
                                                  const int depth = infinite_depth,
                                                  const std::regex& dir_regex = all_dirs,
                                                  const std::vector<std::filesystem::path>& skip_dirs = {})
```

<h3><code>files::get_files</code></h3>
Get all files in directory and sub-directories with `std::function` filtering.  
Arguments:

- `path`: directory to analyze
- `depth`: depth of the analyze (files::infinite_depth for infinite recursive)
- `include_dirs`: add the directories to the list of files (default: false)
- `dir_filter`: `std::function` that determines if the directory is taken (***true***) or not (***false***)
- `file_filter`: `std::function` that determines if the filename is taken (***true***) or not (***false***)

```cpp
// get all files on a directory and its sub-directories with filtering (using a std::function)
const std::vector<std::filesystem::path> get_files(const std::filesystem::path& path,
                                                   const int depth,
                                                   const bool include_dirs,
                                                   const std::function<bool(const std::filesystem::path&)>& dir_filter,
                                                   const std::function<bool(const std::filesystem::path&)>& file_filter)
```

Get all files in directory and sub-directories with `std::regex` filtering.  
Arguments:

- `path`: directory to analyze
- `depth`: depth of the analyze (files::infinite_depth for infinite recursive)
- `include_dirs`: add the directories to the list of files (default: false)
- `dir_regex`: `std::function` that determines if the directory is taken (***true***) or not (***false***)
- `file_regex`: `std::function` that determines if the filename is taken (***true***) or not (***false***)
- `skip_dirs`: exclude a list of directories (default: skip none)
- `skip_files`: exclude a list of files (default: skip none)

```cpp
// get all files on a directory and its sub-directories with filtering (using a std::regex, fullpath for dir, filename with extension for file)
const std::vector<std::filesystem::path> get_files(const std::filesystem::path& path,
                                                   const int depth = infinite_depth,
                                                   const bool include_dirs = false,
                                                   const std::regex& dir_regex = all_dirs,
                                                   const std::regex& file_regex = all_files,
                                                   const std::vector<std::filesystem::path>& skip_dirs = {},
                                                   const std::vector<std::filesystem::path>& skip_files = {})
```

<h3><code>files::get_hash</code></h3>
Get the sha-256 hash of a file using hashpp header-only library.  
Arguments:

- `file`: file to analyze
- `algorithm`: algorithm used for the hash (default: SHA2_256)

```cpp
// get the sha-256 hash of a file
const std::string get_hash(const std::filesystem::path& file, 
                           const hashpp::ALGORITHMS& algorithm = hashpp::ALGORITHMS::SHA2_256)
```

<h3>Usage</h3>

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
    const std::vector<std::filesystem::path>& dirs = files::get_dirs(directory, files::infinite_depth, std::regex(R"(\\DVD)"));
    for (const auto& d : dirs)
      std::cout << d.string() << std::endl;
    std::cout << std::endl;
  }

  {
    // list all files in directory and sub-directories
    //   using std::regex filter (filename finishing with ".exe")
    const std::vector<std::filesystem::path>& files = files::get_files(directory,
                                                                       files::infinite_depth,
                                                                       false,
                                                                       files::all_dirs,
                                                                       std::regex(R"(\.exe$)"));
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

    const std::vector<std::filesystem::path>& files = files::get_files(directory,
                                                                       files::infinite_depth,
                                                                       false,
                                                                       dir_filter,
                                                                       file_filter);
    for (const auto& f : files)
      std::cout << f.string() << std::endl;
    std::cout << std::endl;
  }

  // get the sha-256 hash of a file using hashpp header-only library
  std::cout << "hash for: \"" << file << "\" = " << files::get_hash(file) << std::endl;
  return 0;
}
```

### win

A set of classes and functions to handle windows specific api:

- [x] function: `win::execute` to execute a windows process in blocking mode and retrieve logs
- [x] class: `win::sync_process` to execute a windows process in blocking mode and retrieve logs
- [x] class: `win::async_process` to execute a windows process in non-blocking mode and retrieve logs
- [x] function: `win::expand_path` to expand a windows path to a std::string
- [x] function: `win::get_vcvars` to retrieve the latest vcvars available

<h3><code>win::execute</code></h3>
Function to execute a windows process and retrieve the output of the console in a `std::string`.  
This is a **blocking** api.
Arguments:

- `cmd`: command-line to execute
- `logs`: write all the process logs in this `std::string`
- `working_directory`: set the working-directory of the process
- `timeout`: stop the process after a given timeout (in milliseconds)
- `default_error_code`: default error code when the process fails (default: -1)

```cpp
// execute a windows process - blocking
int execute(const std::string& cmd,
            std::string& logs = std::string(),
            const std::filesystem::path& working_directory = std::filesystem::current_path(),
            const std::chrono::milliseconds timeout = std::chrono::milliseconds::max(),
            const int default_error_code = -1)
```

<h3>Usage</h3>

```cpp
#include <iostream>
#include <winpp/console.hpp>
#include <winpp/win.hpp>

int main(int argc, char** argv)
{
  // initialize Windows console
  console::init();

  // execute program - function api: blocking mode
  std::cout << "execute program - function api: blocking mode" << std::endl;
  std::string logs;
  int exit_code = win::execute("ping 127.0.0.1", logs);
  std::cout << "exit code: " << std::to_string(exit_code) << std::endl;
  std::cout << logs << std::endl;
  return 0;
}
```

<h3><code>win::sync_process</code></h3>
Class to execute a windows process and retrieve the output of the console.  
This is a **blocking** api that can be interrupted using the `stop()` function.  

<h3>Usage</h3>

```cpp
#include <iostream>
#include <signal.h>
#include <winpp/console.hpp>
#include <winpp/win.hpp>

// defined in global to be accessible via exit_program()
win::sync_process g_proc;

// define the function to be called when ctrl-c is sent to process
void exit_program(int signum)
{
  std::cout << "event: ctrl-c called => stopping program" << std::endl;
  g_proc.stop();
}

int main(int argc, char** argv)
{
  // initialize Windows console
  console::init();

  // register signal handler
  signal(SIGINT, exit_program);

  // execute program - class api: blocking mode
  std::cout << "execute program - class api: blocking mode" << std::endl;
  g_proc.set_timeout(std::chrono::milliseconds(3000));
  g_proc.set_working_dir(".");
  g_proc.set_default_error_code(-1);
  int exit_code = g_proc.execute("ping 127.0.0.1 -n 6");
  std::cout << "exit code: " << std::to_string(exit_code) << std::endl;
  std::cout << g_proc.get_logs() << std::endl;
  return 0;
}
```

<h3><code>win::async_process</code></h3>
Class to execute a windows process and retrieve the output of the console.  
This is a **non-blocking** api that can be interrupted using the `stop()` function.  

The `execute` function requires two optional arguments: 

 - one callback that will be called when logs are gathered: cb_logs
 - one callback that will be called when the process terminates/stops: cb_exit

<h3>Usage</h3>

```cpp
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <winpp/console.hpp>
#include <winpp/win.hpp>

int main(int argc, char** argv)
{
  // initialize Windows console
  console::init();

  // execute program - class api: non-blocking mode
  {
    // callback called to display program's logs
    auto& cb_logs = [](const std::string& logs) {
      std::cout << logs;
    };

    // callback called when program terminates
    int exit_code = -1;
    bool program_running = true;
    std::mutex mtx;
    std::condition_variable cv;
    auto& cb_exit = [&](const int _exit_code) {
      exit_code = _exit_code;
      program_running = false;
      cv.notify_one();
    };

    std::cout << "execute program - class api: non-blocking mode" << std::endl;
    win::async_process proc;
    if (proc.execute("ping 127.0.0.1 -n 6", cb_logs, cb_exit))
    {
      std::cout << "waiting for process to terminate" << std::endl;
      std::unique_lock<std::mutex> lck(mtx);
      cv.wait(lck, [&] {return !program_running; });
      std::cout << "exit code: " << std::to_string(exit_code) << std::endl;
    }
  }
  return 0;
}
```

<h3><code>win::expand_path</code></h3>
Expand a windows path to a std::string.
Arguments:

- `path`: windows path that needs to be expanded

```cpp
// expand environment path
const std::filesystem::path expand_path(const std::string& path)
```

<h3>Usage</h3>

```cpp
#include <iostream>
#include <winpp/win.hpp>

int main(int argc, char** argv)
{
  // initialize Windows console
  console::init();

  // expand variable
  std::cout << "expand variable \"%ProgramFiles%\": " << win::expand_path("%ProgramFiles%").string() << std::endl;
  return 0;
}
```

<h3><code>win::get_vcvars</code></h3>
Function to retrieve the latest vcvars available using `vswhere.exe`.

```cpp
// retrieve the latest vcvars available
const std::filesystem::path get_vcvars()
```

<h3>Usage</h3>

```cpp
#include <iostream>
#include <winpp/win.hpp>

int main(int argc, char** argv)
{
  // initialize Windows console
  console::init();

  // get the path from the latest vcvars file
  std::cout << "latest vcvars64.bat: " << win::get_vcvars().string() << std::endl;
  return 0;
}
```

### system-mutex

Provides a system-wide, recursive, named lock (**RAII**).  
This class satisfies requirements of C++11 concept "Lockable",  i.e. can be (and should be) used with unique_lock etc.

```cpp
// acquire system wide mutex to avoid multiples executions in //
win::system_mutex mtx("Global\\XXX");
std::lock_guard<win::system_mutex> lock(mtx);
```

<h3><code>win::system_mutex</code></h3>

```cpp
#include <iostream>
#include <winpp/console.hpp>
#include <winpp/system-mutex.hpp>

int main(int argc, char** argv)
{
  // initialize Windows console
  console::init();

  try
  {
    // acquire system wide mutex to avoid multiples executions in //
    win::system_mutex mtx("Global\\XXX");
    std::lock_guard<win::system_mutex> lock(mtx);

    // long process that will be protected
    return 0;
  }
  catch (const std::exception& ex)
  {
    std::cout << "error: " << ex.what() << std::endl;
    return -1;
  }
}
```