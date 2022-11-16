#pragma once
#include <string>
#include <atlstr.h>

namespace utf8
{
  // convert string to wstring using the code_page
  inline const std::wstring s2ws(const std::string& str, const UINT code_page)
  {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(code_page, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(code_page, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
  }

  // convert wstring to string using the code_page
  inline const std::string ws2s(const std::wstring& wstr, const UINT code_page)
  {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(code_page, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(code_page, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
  }

  // from wide unicode string to utf8 string
  inline const std::string to_utf8(const std::wstring& wstr)
  {
    return ws2s(wstr, CP_UTF8);
  }

  // from extended ascii to utf8 string
  inline const std::string to_utf8(const std::string& str)
  {
    return ws2s(s2ws(str, CP_ACP), CP_UTF8);
  }

  // convert from utf8 string to extended ascii
  inline const std::string from_utf8(const std::string& str)
  {
    return ws2s(s2ws(str, CP_UTF8), CP_ACP);
  }
}