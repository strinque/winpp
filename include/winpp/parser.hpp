#pragma once
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/color.h>
#include <stdbool.h>
#include <winpp/utf8.hpp>

namespace console
{
  // base class for Option (to store inside std::map)
  class OptionBase
  {
  public:
    // constructor/destructor
    OptionBase(const std::string& _short,
               const std::string& _full,
               const std::string& _desc,
               bool _mandatory) :
               m_short(_short),
               m_full(_full),
               m_desc(_desc),
               m_mandatory(_mandatory),
               m_defined(false) {}
    virtual ~OptionBase() = default;

    // getters
    std::string get_short() const { return m_short; }
    std::string get_full() const { return m_full; }
    std::string get_desc() const { return m_desc; }
    bool get_mandatory() const { return m_mandatory; }
    bool get_defined() const { return m_defined; }

    // virtual function to determine if this option has an argument
    virtual bool has_arg() const = 0;

    // virtual function to set the value in the 
    virtual void set(const std::string& str="") = 0;

  protected:
    std::string m_short;  // short command-line option (ex: "h")
    std::string m_full;   // full command-line option (ex: "help")
    std::string m_desc;   // command-line option description
    bool m_mandatory;     // is this option mandatory?
    bool m_defined;       // has this option been set?
  };

  // store option informations
  template<typename T>
  class Option final : public OptionBase
  {
  public:
    // constructor/destructor
    Option(const std::string& _short,
           const std::string& _full,
           const std::string& _desc,
           T& _value,
           bool _mandatory = false) :
      OptionBase(_short,
                _full,
                _desc,
                _mandatory),
      m_value(_value)
    {
      m_value = T();
    }
    ~Option() = default;

    // check if the option has an argument
    bool has_arg() const override { return inner_has_arg<T>(); }
    template<typename T> bool inner_has_arg() const { return true; }
    template<> bool inner_has_arg<bool>() const { return false; }

    // overriden function to parse and store the argument value
    void set(const std::string& str = "") override
    {
      // check argument validity
      if (inner_has_arg<T>() && str.empty())
        throw std::exception(fmt::format("missing argument for [\"-{}\", \"--{}\"]", m_short, m_full).c_str());

      try
      {
        inner_set<T>(str);
        m_defined = true;
      }
      catch (...)
      {
        throw std::exception(fmt::format("can't convert argument: \"{}\" for [\"-{}\", \"--{}\"]", str, m_short, m_full).c_str());
      }
    }

    // forbid non specialized template for the parser
    template<typename T> void inner_set(const std::string& str);

    // define specialized template for the parser
    template<> void inner_set<std::string>(const std::string& str) { m_value = utf8::to_utf8(str); }
    template<> void inner_set<bool>(const std::string& str) { m_value = true; }
    template<> void inner_set<int>(const std::string& str) { m_value = std::stoi(str); }
    template<> void inner_set<unsigned int>(const std::string& str) { m_value = std::stoui(str); }
    template<> void inner_set<long>(const std::string& str) { m_value = std::stol(str); }
    template<> void inner_set<unsigned long>(const std::string& str) { m_value = std::stoul(str); }
    template<> void inner_set<long long>(const std::string& str) { m_value = std::stoll(str); }
    template<> void inner_set<unsigned long long>(const std::string& str) { m_value = std::stoull(str); }
    template<> void inner_set<double>(const std::string& str) { m_value = std::stod(str); }
    template<> void inner_set<long double>(const std::string& str) { m_value = std::stold(str); }
    template<> void inner_set<float>(const std::string& str) { m_value = std::stof(str); }

  private:
    T& m_value;     // reference to the original argument
  };

  // command-line parser
  class parser final
  {
  public:
    // list of keys that could be used to retrieve option from vector
    enum class Key { None, Short, Full };

    // constructor/destructor
    parser(const std::string& program_name,
           const std::string& program_version) :
      m_program_name(program_name),
      m_program_version(program_version),
      m_option_help(false),
      m_option_version(false),
      m_options(),
      m_errors()
    {
      // register default arguments: help and version
      add("h", "help", "display the program help", m_option_help);
      add("v", "version", "display the program version", m_option_version);
    }
    ~parser() = default;

    // try to register the argument
    template<typename T>
    parser& add(const std::string& _short,
                const std::string& _full,
                const std::string& _desc, 
                T& _value, 
                bool _mandatory = false)
    {
      // check arguments validity
      if (_short.empty() && _full.empty())
        m_errors.push_back("empty argument");
      else if (_short.empty())
        m_errors.push_back(fmt::format("argument [\"--{}\"] has no short-sized argument", _full));
      else if (_full.empty())
        m_errors.push_back(fmt::format("argument [\"-{}\"] has no full-sized argument", _short));
      else
      {
        // add this option to the list of options
        if (find(Key::Short, _short) != m_options.end())
          m_errors.push_back(fmt::format("argument [\"-{}\"] is already defined", _short));
        else if (find(Key::Full, _full) != m_options.end())
          m_errors.push_back(fmt::format("argument [\"--{}\"] is already defined", _full));
        else
          m_options.push_back(std::make_shared<Option<T>>(_short,
                                                          _full,
                                                          _desc,
                                                          _value,
                                                          _mandatory));
      }
      return *this;
    }

    // parse command-line arguments with get-opt syntaxe
    bool parse(int argc, char *argv[])
    {
      try
      {
        // check for add() errors
        if (!m_errors.empty())
        {
          std::string errors = "wrong definition of options with add():\n";
          for (const auto& e : m_errors)
            errors += "  " + e + "\n";
          throw std::exception(errors.c_str());
        }

        // parse all command-line arguments
        for (int i = 1; i < argc; ++i)
        {
          // extract command-line option and argument
          const std::string& cmd_option = argv[i];
          const std::string& cmd_arg = (i < argc-1) ? argv[i + 1] : "";

          // find the option in the options list
          std::vector<std::shared_ptr<OptionBase>>::const_iterator opt;
          if (cmd_option.find("--") == 0)
            opt = find(Key::Full, cmd_option.substr(2));
          else if (cmd_option.find("-") == 0)
            opt = find(Key::Short, cmd_option.substr(1));
          else
            throw std::exception(fmt::format("invalid option: [\"{}\"]", cmd_option).c_str());
          if (opt == m_options.end())
            throw std::exception(fmt::format("unknow option [\"{}\"]", cmd_option).c_str());

          // set command-line arguments
          (*opt)->set(cmd_arg);

          // skip next element if this option contained an argument
          if ((*opt)->has_arg())
            ++i;
        }

        // handle help/version default command-line options
        if (m_option_help)
        {
          print_usage();
          exit(0);
        }
        else if (m_option_version)
        {
          fmt::print("{}.exe (version: v{})\n", m_program_name, m_program_version);
          exit(0);
        }

        // check that all mandatory options have been set
        auto opt = check_mandatory();
        if (opt != m_options.end())
        {
          if (argc != 1)
            throw std::exception(fmt::format("option [\"-{}\", \"--{}\"] is mandatory and hasn't been set",
              (*opt)->get_short(),
              (*opt)->get_full()).c_str());
          else
            return false;
        }
        return true;
      }
      catch (const std::exception& ex)
      {
        fmt::print("{} {}\n",
          fmt::format(fmt::fg(fmt::color::red) | fmt::emphasis::bold, "error: "),
          ex.what());
        return false;
      }
    }

    // display program usage
    void print_usage() const
    {
      // lambda function to display a colored tag
      auto add_tag = [](const fmt::color color, const std::string& text) {
        return fmt::format(fmt::fg(color) | fmt::emphasis::bold, "[{}]", text);
      };

      // display program command
      fmt::print("{} {}\n",
        fmt::format(fmt::emphasis::bold, "usage: {}.exe", m_program_name),
        add_tag(fmt::color::green, "options"));

      // calc max size of cmd_full and desc - for alignment
      std::size_t max_size_cmd_full = 0;
      std::size_t max_size_desc = 0;

      for (const auto& opt : m_options)
      {
        max_size_cmd_full = (std::max)(opt->get_full().size(), max_size_cmd_full);
        max_size_desc = (std::max)(opt->get_desc().size(), max_size_desc);
      }

      // display specific options
      std::string format;
      format = "  -{}, ";
      format += " --{:<" + std::to_string(max_size_cmd_full + 2) + "} ";
      format += "{:<" + std::to_string(max_size_desc + 2) + "} ";
      format += "{}\n";
      for (const auto& opt : m_options)
        fmt::print(format,
                   opt->get_short(),
                   opt->get_full(),
                   opt->get_desc(),
                   opt->get_mandatory() ? add_tag(fmt::color::orange, "mandatory") : "");
    }

  private:
    // find an option in the options vector using a Key::Short/Key::Full
    std::vector<std::shared_ptr<OptionBase>>::const_iterator find(const enum class Key key,
                                                                  const std::string& str) const
    {
      // lambda function to find the option
      auto find_option = [str, key](const std::shared_ptr<OptionBase>& opt) -> bool
      {
        switch (key)
        {
        case Key::Short:
          return opt->get_short() == str;
          break;
        case Key::Full:
          return opt->get_full() == str;
          break;
        case Key::None:
        default:
          throw std::exception("invalid find key");
          break;
        }
      };
      return std::find_if(m_options.begin(), m_options.end(), find_option);
    }

    // find an option in the options vector that is mandatory but not defined
    std::vector<std::shared_ptr<OptionBase>>::const_iterator check_mandatory() const
    {
      // lambda function to find the unset mandatory option
      auto check_mandatory_option = [](const std::shared_ptr<OptionBase>& opt) -> bool
      {
        return opt->get_mandatory() && !opt->get_defined();
      };
      return std::find_if(m_options.begin(), m_options.end(), check_mandatory_option);
    }

  private:
    // store program informations
    std::string m_program_name;
    std::string m_program_version;

    // default arguments
    bool m_option_help;
    bool m_option_version;

    // store list of options - sorted by order of insertion
    std::vector<std::shared_ptr<OptionBase>> m_options;

    // store errors detected during add()
    std::vector<std::string> m_errors;
  };
}