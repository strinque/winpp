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
    const auto& check_dir = [dir_filter, check_depth, depth](const std::filesystem::path& p, const int d) -> bool {     
      return std::filesystem::is_directory(p) && check_depth(depth, d) && dir_filter(p);
    };

    if (!std::filesystem::is_directory(path))
      throw std::runtime_error(fmt::format("invalid directory: \"{}\"", path.string()));

    std::vector<std::filesystem::path> dirs;
    auto entry = std::filesystem::recursive_directory_iterator(path);
    while (entry != std::filesystem::recursive_directory_iterator())
    {
      if (check_dir(entry->path(), entry.depth()))
        dirs.push_back(entry->path());
      ++entry;
    }
    return dirs;
  }

  // get all directories and sub-directories with filtering (using a std::regex, fullpath for skip_dirs)
  inline const std::vector<std::filesystem::path> get_dirs(const std::filesystem::path& path,
                                                           const int depth = infinite_depth,
                                                           const std::regex& dir_regex = all_dirs,
                                                           const std::vector<std::filesystem::path>& skip_dirs = {})
  {
    const auto& filter = [dir_regex, skip_dirs](const std::filesystem::path& p) -> bool {
      const auto& compare_path = [p](const std::filesystem::path& p2) -> bool {
        return p.string().find(p2.string()) != std::string::npos;
      };
      return std::regex_search(p.string(), dir_regex) &&
             std::find_if(skip_dirs.begin(), skip_dirs.end(), compare_path) == skip_dirs.end();
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
    const auto& check_dir = [&](const std::filesystem::path& p, const int d) -> bool {
      return include_dirs ? check_depth(depth, d) && dir_filter(p) : false;
    };
    const auto& check_file = [&](const std::filesystem::path& p, const int d) -> bool {
      return check_depth(depth, d) && dir_filter(p) && file_filter(p);
    };
    const auto& check = [&](const std::filesystem::path& p, const int d) -> bool {
      return std::filesystem::is_directory(p) ? check_dir(p, d) : check_file(p, d);
    };

    if (!std::filesystem::is_directory(path))
      throw std::runtime_error(fmt::format("invalid directory: \"{}\"", path.string()));

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
    const auto& dir_filter = [&](const std::filesystem::path& p) -> bool {
      const auto& compare_path = [p](const std::filesystem::path& p2) -> bool {
        return p.string().find(p2.string()) != std::string::npos;
      };
      return std::regex_search(p.string(), dir_regex) &&
             std::find_if(skip_dirs.begin(), skip_dirs.end(), compare_path) == skip_dirs.end();
    };
    const auto& file_filter = [&](const std::filesystem::path& p) -> bool {
      const auto& compare_path = [p](const std::filesystem::path& p2) -> bool {
        return std::filesystem::equivalent(p, p2);
      };
      return std::regex_search(p.filename().string(), file_regex) &&
             std::find_if(skip_files.begin(), skip_files.end(), compare_path) == skip_files.end();
    };
    return get_files(path, depth, include_dirs, dir_filter, file_filter);
  }

  // read file in one std::string
  inline const std::string read(const std::filesystem::path& path)
  {
    std::ifstream file(path);
    if (!file)
      throw std::runtime_error(fmt::format("can't open file: \"{}\"", path.filename().u8string()));
    file.seekg(0, std::ios::end);
    std::string str;
    str.reserve(file.tellg());
    file.seekg(0, std::ios::beg);
    str.assign((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());
    return str;
  }

  // get the sha-256 hash of a file (using hashpp header-only library)
  inline const std::string get_hash(const std::filesystem::path& file, 
                                    const hashpp::ALGORITHMS& algorithm = hashpp::ALGORITHMS::SHA2_256)
  {
    // try to open file in binary mode
    std::ifstream f(file.string(), std::ios::binary);
    if (!f.good())
      throw std::runtime_error(fmt::format("can't open file: \"{}\"", file.filename().string()));
    return hashpp::get::getFileHash(algorithm, file.string()).getString();
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
    HANDLE fp = CreateFileA(file.string().c_str(),
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