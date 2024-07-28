#pragma once

#include <format>
#include <string>
#include <fstream>

const std::string ANSI_RESET = "\033[0m";
const std::string ANSI_RED = "\033[38;5;160m";
const std::string ANSI_GREEN = "\033[38;5;157m";
const std::string ANSI_YELLOW = "\033[38;5;220m";
const std::string ANSI_GRAY = "\033[38;5;251m";
const std::string ANSI_WHITE = "\033[37m";

class Logger {
  public:
  inline static bool log_to_file = true;
  Logger(std::string logID);
  Logger(std::string logID, std::string filename);
  ~Logger();

  template<typename... Args>
  void logf(const std::format_string<Args...> fmt, Args&&...args) {
    log(std::vformat(fmt.get(), std::make_format_args(args...)));
  }
  void log(const std::string& str);
  template<typename... Args>
  void warnf(const std::format_string<Args...> fmt, Args&&...args) {
    warn(std::vformat(fmt.get(), std::make_format_args(args...)));
  }
  void warn(const std::string& str);
  template<typename... Args>
  void errorf(const std::format_string<Args...> fmt, Args&&...args) {
    error(std::vformat(fmt.get(), std::make_format_args(args...)));
  }
  void error(const std::string& str);
  template<typename... Args>
  void debugf(const std::format_string<Args...> fmt, Args&&...args) {
    debug(std::vformat(fmt.get(), std::make_format_args(args...)));
  }
  void debug(const std::string& str);

  private:
  void write(const std::string& str, const std::string& ansi_timestamp, const std::string& log_type);
  std::string getTimeStamp();
  std::string m_logFileName = "log.txt";
  std::string m_logID;
  std::ofstream m_file;
};
