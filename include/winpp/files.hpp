#pragma once
#include <string>
#include <vector>
#include <regex>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <functional>
#include <sys/stat.h>
#include <fmt/format.h>
#include <windows.h>
#include <winpp/hashpp.h>

namespace files
{
  constexpr int infinite_depth = -1;
  constexpr auto default_filter = [](const std::filesystem::path& p) -> bool { return true; };
  const std::regex all_dirs = std::regex(R"(.*)");
  const std::regex all_files = std::regex(R"(.*)");

  // get all directories and sub-directories with filtering (using a std::function)
  inline const std::vector<std::filesystem::path> get_dirs(const std::filesystem::path& path,
                                                           const int depth,
                                                           const std::function<bool(const std::filesystem::path&)>& dir_filter)
  {
    const auto& check_depth = [](const int max_depth, const int current_depth) -> bool {
      return (max_depth == infinite_depth) ? true : (current_depth <= max_depth);
    };
    const auto& check_dir = [=](const std::filesystem::path& p, const int d) -> bool {     
      return std::filesystem::is_directory(p) && check_depth(depth, d) && dir_filter(p);
    };

    if (!std::filesystem::is_directory(path))
      throw std::runtime_error(fmt::format("invalid directory: \"{}\"", path.u8string()));

    std::vector<std::filesystem::path> dirs;
    if (check_dir(path, 0))
      dirs.push_back(path);
    auto entry = std::filesystem::recursive_directory_iterator(path);
    while (entry != std::filesystem::recursive_directory_iterator())
    {
      if (check_dir(entry->path(), entry.depth()))
        dirs.push_back(entry->path());
      ++entry;
    }
    return dirs;
  }

  // convert a path to a lowercase std::string
  inline const std::string to_str(const std::filesystem::path& path)
  {
    std::string str = path.string();
    std::transform(str.cbegin(), str.cend(), str.begin(), [](const char& c) { return std::tolower(c); });
    return str;
  }

  // get all directories and sub-directories with filtering (using a std::regex, fullpath for skip_dirs)
  inline const std::vector<std::filesystem::path> get_dirs(const std::filesystem::path& path,
                                                           const int depth = infinite_depth,
                                                           const std::regex& dir_regex = all_dirs,
                                                           const std::vector<std::filesystem::path>& skip_dirs = {})
  {
    const auto& filter = [=](const std::filesystem::path& p1) -> bool {
      const auto& compare_path = [p1](const std::filesystem::path& p2) -> bool {
        return to_str(p1).find(to_str(p2)) != std::string::npos;
      };
      return std::regex_search(p1.string(), dir_regex) &&
             std::find_if(skip_dirs.cbegin(), skip_dirs.cend(), compare_path) == skip_dirs.cend();
    };
    return get_dirs(path, depth, filter);
  }

  // get all files on a directory and its sub-directories with filtering (using a std::function)
  inline const std::vector<std::filesystem::path> get_files(const std::filesystem::path& path,
                                                            const int depth,
                                                            const bool include_dirs,
                                                            const std::function<bool(const std::filesystem::path&)>& dir_filter,
                                                            const std::function<bool(const std::filesystem::path&)>& file_filter)
  {
    const auto& check_depth = [](const int max_depth, const int current_depth) -> bool {
      return (max_depth == infinite_depth) ? true : (current_depth <= max_depth);
    };
    const auto& check_dir = [=](const std::filesystem::path& p, const int d) -> bool {
      return include_dirs ? check_depth(depth, d) && dir_filter(p) : false;
    };
    const auto& check_file = [=](const std::filesystem::path& p, const int d) -> bool {
      return check_depth(depth, d) && dir_filter(p) && file_filter(p);
    };
    const auto& check = [=](const std::filesystem::path& p, const int d) -> bool {
      return std::filesystem::is_directory(p) ? check_dir(p, d) : check_file(p, d);
    };

    if (!std::filesystem::is_directory(path))
      throw std::runtime_error(fmt::format("invalid directory: \"{}\"", path.u8string()));

    std::vector<std::filesystem::path> files;
    if (depth == 0)
    {
      for (const auto& entry : std::filesystem::directory_iterator(path))
      {
        if (check(entry.path(), 0))
          files.push_back(entry.path());
      }
    }
    else
    {
      auto entry = std::filesystem::recursive_directory_iterator(path);
      while (entry != std::filesystem::recursive_directory_iterator())
      {
        if (check(entry->path(), entry.depth()))
          files.push_back(entry->path());
        ++entry;
      }
    }
    return files;
  }

  // get all files on a directory and its sub-directories with filtering (using a std::regex, fullpath for dir, filename with extension for file)
  inline const std::vector<std::filesystem::path> get_files(const std::filesystem::path& path,
                                                            const int depth = infinite_depth,
                                                            const bool include_dirs = false,
                                                            const std::regex& dir_regex = all_dirs,
                                                            const std::regex& file_regex = all_files,
                                                            const std::vector<std::filesystem::path>& skip_dirs = {},
                                                            const std::vector<std::filesystem::path>& skip_files = {})
  {
    const auto& dir_filter = [=](const std::filesystem::path& p1) -> bool {
      const auto& compare_path = [p1](const std::filesystem::path& p2) -> bool {
        return to_str(p1).find(to_str(p2)) != std::string::npos;
      };
      return std::regex_search(p1.string(), dir_regex) &&
             std::find_if(skip_dirs.cbegin(), skip_dirs.cend(), compare_path) == skip_dirs.cend();
    };
    const auto& file_filter = [=](const std::filesystem::path& p1) -> bool {
      const auto& compare_path = [p1](const std::filesystem::path& p2) -> bool {
        return std::filesystem::equivalent(p1, p2);
      };
      return std::regex_search(p1.filename().string(), file_regex) &&
             std::find_if(skip_files.cbegin(), skip_files.cend(), compare_path) == skip_files.cend();
    };
    return get_files(path, depth, include_dirs, dir_filter, file_filter);
  }

  // read file in one std::string
  inline const std::string read(const std::filesystem::path& path)
  {
    std::ifstream file(path, std::ios::binary);
    if (!file)
      throw std::runtime_error(fmt::format("can't open file: \"{}\"", path.filename().u8string()));
    file.seekg(0, std::ios::end);
    const std::size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::string str(size, 0);
    file.read(&str[0], size);
    return str;
  }

  // get the sha-256 hash of a file (using hashpp header-only library)
  inline const std::string get_hash(const std::filesystem::path& file, 
                                    const hashpp::ALGORITHMS& algorithm = hashpp::ALGORITHMS::SHA2_256)
  {
    // try to open file in binary mode
    std::ifstream f(file, std::ios::binary);
    if (!f.good())
      throw std::runtime_error(fmt::format("can't open file: \"{}\"", file.filename().u8string()));
    return hashpp::get::getFileHash(algorithm, file.string()).getString();
  }

  // get stat from file
  inline const struct stat get_stat(const std::filesystem::path& file)
  {
    struct stat file_info;
    if (stat(file.string().c_str(), &file_info) != 0)
      throw std::runtime_error(fmt::format("can't read file information for \"{}\" (err: \"{}\"", file.u8string(), strerror(errno)));
    return file_info;
  }

  // set ctime/mtime for file
  inline const void set_stat(const std::filesystem::path& file,
                             const uint64_t ctime,
                             const uint64_t atime,
                             const uint64_t mtime)
  {
    HANDLE fp = CreateFileA(file.string().c_str(),
                            GENERIC_WRITE,
                            FILE_SHARE_WRITE,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);
    if (fp != INVALID_HANDLE_VALUE)
    {
      auto to_ft = [](const uint64_t ts) -> FILETIME {
        ULARGE_INTEGER uli = { 0 };
        uli.QuadPart = static_cast<ULONGLONG>(ts) * 10000000ULL + 116444736000000000ULL;
        FILETIME ft;
        ft.dwLowDateTime = uli.LowPart;
        ft.dwHighDateTime = uli.HighPart;
        return ft;
      };
      FILETIME creation_time = to_ft(ctime);
      FILETIME access_time = to_ft(atime);
      FILETIME modification_time = to_ft(mtime);
      if (!SetFileTime(fp,
                       ctime ? &creation_time : nullptr,
                       atime ? &access_time : nullptr,
                       mtime ? &modification_time : nullptr))
      {
        CloseHandle(fp);
        throw std::runtime_error(fmt::format("can't write file information for \"{}\" (err: \"{}\"", file.u8string(), GetLastError()));
      }
      CloseHandle(fp);
    }
  }
}