#pragma once
#include <string>
#include <vector>
#include <regex>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sys/stat.h>
#include <openssl/sha.h>
#include <fmt/format.h>
#include <windows.h>

namespace files
{
  // get all directories and sub-directories with filtering (by regex)
  inline const std::vector<std::filesystem::path> get_dirs(const std::string& path,
                                                           const std::regex& dir_pattern = std::regex(R"(.*)"),
                                                           const std::vector<std::filesystem::path>& skip_dirs = {})
  {
    std::vector<std::filesystem::path> dirs;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
    {
      if (entry.is_directory() &&
          std::regex_search(entry.path().string(), dir_pattern) &&
          std::find(skip_dirs.begin(), skip_dirs.end(), entry.path()) == skip_dirs.end())
        dirs.push_back(entry.path());
    }
    return dirs;
  }

  // get all files on a directory and its sub-directories with filtering (by regex)
  inline const std::vector<std::filesystem::path> get_files(const std::string& path,
                                                            const std::regex& dir_pattern = std::regex(R"(.*)"),
                                                            const std::regex& file_pattern = std::regex(R"(.*)"))
  {
    std::vector<std::filesystem::path> files;
    const std::vector<std::filesystem::path> dirs = get_dirs(path, dir_pattern);
    for (const auto& dir : dirs)
    {
      for (const auto& entry : std::filesystem::directory_iterator(dir))
      {
        if (!entry.is_directory() &&
            std::regex_search(entry.path().filename().string(), file_pattern))
          files.push_back(entry.path().string());
      }
    }
    return files;
  }

  // get the sha-256 hash of a file (using OpenSSL crypto algorithm)
  inline const std::string get_hash(const std::filesystem::path& file)
  {
    // open file in binary mode
    std::ifstream f(file, std::ios::binary);
    if (!f.good())
      throw std::runtime_error(fmt::format("can't open file: \"{}\"", file.filename().string()));

    // initialize hash context
    SHA256_CTX ctx;
    SHA256_Init(&ctx);

    // hash the file content
    unsigned char hash[SHA256_DIGEST_LENGTH] = {};
    constexpr const std::size_t buf_size{ 1 << 9 };
    char buf[buf_size];
    while (f.good())
    {
      f.read(buf, buf_size);
      SHA256_Update(&ctx, buf, f.gcount());
    }
    SHA256_Final(hash, &ctx);

    // convert to std::string
    std::string str;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
      str += fmt::format("{:02x}", static_cast<unsigned int>(hash[i]));
    return str;
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
                  atime ? &access_time: nullptr,
                  mtime ? &modification_time: nullptr);
      CloseHandle(fp);
    }
  }
}