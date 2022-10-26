#pragma once
#include <string>
#include <atlstr.h>

namespace utf8
{
  // from extended ascii to utf8 string
  inline const std::string to_utf8(const std::string& str)
  {
    if (str.empty()) return std::string();
    int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);

    if (wstrTo.empty()) return std::string();
    size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstrTo[0], (int)wstrTo.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstrTo[0], (int)wstrTo.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
  }

  // from wide unicode string to utf8 string
  inline const std::string to_utf8(const std::wstring& wstr)
  {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
  }

  // convert from utf8 string to extended ascii
  inline const std::string from_utf8(const std::string& str)
  {
    if (str.empty()) return std::string();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);

    if (wstrTo.empty()) return std::string();
    size_needed = WideCharToMultiByte(CP_ACP, 0, &wstrTo[0], (int)wstrTo.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_ACP, 0, &wstrTo[0], (int)wstrTo.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
  }

  // string to wstring
  inline const std::wstring s2ws(const std::string& str)
  {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
  }

  // wstring to string
  inline const std::string ws2s(const std::wstring& wstr)
  {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
  }
}