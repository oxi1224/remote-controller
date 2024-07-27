#pragma once

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
  void logf(std::string fmt, Args &&...args);
  void log(std::string str);
  template<typename... Args>
  void warnf(std::string fmt, Args &&...args);
  void warn(std::string str);
  template<typename... Args>
  void errorf(std::string fmt, Args &&...args);
  void error(std::string str);
  template<typename... Args>
  void debugf(std::string fmt, Args &&...args);
  void debug(std::string str);

  private:
  void write(std::string str, std::string ansi_timestamp, std::string log_type);
  std::string getTimeStamp();
  std::string m_logFileName = "log.txt";
  std::string m_logID;
  std::ofstream m_file;
};
