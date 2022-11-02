#pragma once
#include <string>
#include <vector>
#include <regex>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <functional>
#include <sys/stat.h>
#include <fmt/format.h>
#include <windows.h>
#include <winpp/hashpp.h>

namespace files
{
  // get all directories and sub-directories with filtering (using a std::function)
  inline const std::vector<std::filesystem::path> get_dirs(const std::filesystem::path& path,
                                                           const std::function<bool(const std::filesystem::path&)>& dir_filter)
  {
    const auto& check_dir = [&](const std::filesystem::path& p) -> bool {
      return std::filesystem::is_directory(p) && dir_filter(p);
    };

    if (!std::filesystem::is_directory(path))
      throw std::runtime_error(fmt::format("invalid directory: \"{}\"", path.string()));

    std::vector<std::filesystem::path> dirs;
    if (check_dir(path))
      dirs.push_back(path);
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
    {
      if (check_dir(entry.path()))
        dirs.push_back(entry.path());
    }
    return dirs;
  }

  // get all directories and sub-directories with filtering (using a std::regex, fullpath for dir)
  inline const std::vector<std::filesystem::path> get_dirs(const std::filesystem::path& path,
                                                           const std::regex& dir_regex = std::regex(R"(.*)"),
                                                           const std::vector<std::filesystem::path>& skip_dirs = {})
  {
    const auto& filter = [&](const std::filesystem::path& p) -> bool {
      return std::regex_search(p.string(), dir_regex) &&
             std::find(skip_dirs.begin(), skip_dirs.end(), p) == skip_dirs.end();
    };
    return get_dirs(path, filter);
  }

  // get all files on a directory and its sub-directories with filtering (using a std::function)
  inline const std::vector<std::filesystem::path> get_files(const std::filesystem::path& path,
                                                            const std::function<bool(const std::filesystem::path&)>& dir_filter,
                                                            const std::function<bool(const std::filesystem::path&)>& file_filter)
  {
    const auto& check_file = [&](const std::filesystem::path& p) -> bool {
      return !std::filesystem::is_directory(p) && file_filter(p);
    };

    if (!std::filesystem::is_directory(path))
      throw std::runtime_error(fmt::format("invalid directory: \"{}\"", path.string()));

    std::vector<std::filesystem::path> files;
    const std::vector<std::filesystem::path>& dirs = get_dirs(path, dir_filter);
    for (const auto& dir : dirs)
    {
      for (const auto& entry : std::filesystem::directory_iterator(dir))
      {
        if (check_file(entry.path()))
          files.push_back(entry.path().string());
      }
    }
    return files;
  }

  // get all files on a directory and its sub-directories with filtering (using a std::regex, fullpath for dir, filename with extension for file)
  inline const std::vector<std::filesystem::path> get_files(const std::filesystem::path& path,
                                                            const std::regex& dir_regex,
                                                            const std::regex& file_regex,
                                                            const std::vector<std::filesystem::path>& skip_dirs = {},
                                                            const std::vector<std::filesystem::path>& skip_files = {})
  {
    const auto& dir_filter = [&](const std::filesystem::path& p) -> bool {
      return std::regex_search(p.string(), dir_regex) &&
             std::find(skip_dirs.begin(), skip_dirs.end(), p) == skip_dirs.end();
    };
    const auto& file_filter = [&](const std::filesystem::path& p) -> bool {
      return std::regex_search(p.filename().string(), file_regex) &&
             std::find(skip_files.begin(), skip_files.end(), p) == skip_files.end();
    };
    return get_files(path, dir_filter, file_filter);
  }

  // get all files on a directory and sub-directories (if recursive flag is true) with filtering on filename (by regex)
  inline const std::vector<std::filesystem::path> get_files(const std::filesystem::path& path,
                                                            const bool recursive = true,
                                                            const std::regex& file_regex = std::regex(R"(.*)"),
                                                            const std::vector<std::filesystem::path>& skip_files = {})
  {
    if (recursive)
    {
      return get_files(path,
                       std::regex(R"(.*)"),                   // accept all sub-directories
                       file_regex, 
                       {},                                    // empty skip directory
                       skip_files);
    }
    else
    {
      const std::string& p = std::regex_replace(path.string(), std::regex(R"(\\)"), "\\\\");
      return get_files(path,
                       std::regex(fmt::format("^{}$", p)),    // avoid all sub-directories
                       file_regex,
                       {},                                    // empty skip directory
                       skip_files);
    }
  }

  // get the sha-256 hash of a file (using hashpp header-only library)
  inline const std::string get_hash(const std::filesystem::path& file)
  {
    // try to open file in binary mode
    std::ifstream f(file.string(), std::ios::binary);
    if (!f.good())
      throw std::runtime_error(fmt::format("can't open file: \"{}\"", file.filename().string()));
    return hashpp::get::getFileHash(hashpp::ALGORITHMS::SHA2_256, file.string()).getString();
  }

  // get stat from file
  inline const struct stat get_stat(const std::filesystem::path& file)
  {
    struct stat file_info;
    if (stat(file.string().c_str(), &file_info) != 0)
      throw std::runtime_error(fmt::format("can't read file information for \"{}\" (err: \"{}\"", file.string(), strerror(errno)));
    return file_info;
  }

  // set ctime/mtime for file
  inline const void set_stat(const std::filesystem::path& file,
                             const uint64_t ctime,
                             const uint64_t atime,
                             const uint64_t mtime)
  {
    HANDLE fp = CreateFile(file.string().c_str(),
                           GENERIC_WRITE,
                           FILE_SHARE_WRITE,
                           NULL,
                           OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL);
    if (fp != INVALID_HANDLE_VALUE)
    {
      auto to_ts = [](const uint64_t ts) { return (ts * 10000000) + 116444736000000000; };
      const uint64_t c_ctime = to_ts(ctime);
      const uint64_t c_atime = to_ts(atime);
      const uint64_t c_mtime = to_ts(mtime);
      FILETIME creation_time{ c_ctime & 0xFFFFFFFF, c_ctime >> 32 };
      FILETIME access_time{ c_atime & 0xFFFFFFFF, c_atime >> 32 };
      FILETIME modification_time{ c_mtime & 0xFFFFFFFF, c_mtime >> 32 };
      SetFileTime(fp,
                  ctime ? &creation_time : nullptr,
                  atime ? &access_time : nullptr,
                  mtime ? &modification_time : nullptr);
      CloseHandle(fp);
    }
  }
}