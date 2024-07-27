#include <ctime>
#include <format>
#include <fstream>
#include <iostream>
#include <logger.hpp>

Logger::Logger(std::string logID) {
  m_logID = logID;
  if (log_to_file == true) m_file = std::ofstream(m_logFileName, std::fstream::app);
}

Logger::Logger(std::string logID, std::string filename) {
  m_logID = logID;
  m_logFileName = filename;
  if (log_to_file == true) m_file = std::ofstream(m_logFileName, std::fstream::app);
}

Logger::~Logger() {
  m_file.close();
}

std::string Logger::getTimeStamp() {
	std::time_t rawtime;
  time(&rawtime);
	struct tm timeinfo;
  localtime_s(&timeinfo, &rawtime);
	std::string strHr = std::to_string(timeinfo.tm_hour);
	std::string strMin = std::to_string(timeinfo.tm_min);
	std::string strSec = std::to_string(timeinfo.tm_sec);

	if (strHr.length() != 2) {
		strHr = "0" + strHr;
	}
	if (strMin.length() != 2) {
		strMin = "0" + strMin;
	}
	if (strSec.length() != 2) {
		strSec = "0" + strSec;
	}

	std::string timestamp = "[";
	timestamp += strHr;
	timestamp += ":";
	timestamp += strMin;
	timestamp += ":";
	timestamp += strSec;
	timestamp += "] ";
  return timestamp;
}

void Logger::write(std::string txt, std::string ansi_color, std::string log_type) {
  std::string log_info = "[" + m_logID + "/" + log_type + "] ";
  std::cout << ANSI_GREEN << getTimeStamp() << ANSI_RESET << ansi_color << log_info << txt << ANSI_RESET << std::endl;
  if (log_to_file && !m_file.is_open()) {
    m_file = std::ofstream(m_logFileName, std::fstream::app);
  }
  if (log_to_file) m_file << getTimeStamp() << log_info << txt << std::endl;
}

void Logger::log(std::string str) {
  write(str, ANSI_WHITE, "INFO");
}

template<typename... Args>
void Logger::logf(std::string fmt, Args &&...args) {
  log(std::format(fmt, args...));
}

void Logger::warn(std::string str) {
  write(str, ANSI_YELLOW, "WARN");
}

template<typename... Args>
void Logger::warnf(std::string fmt, Args &&...args) {
  warn(std::format(fmt, args...));
}

void Logger::error(std::string str) {
  write(str, ANSI_RED, "ERROR");
}

template<typename... Args>
void Logger::errorf(std::string fmt, Args &&...args) {
  error(std::format(fmt, args...));
}

void Logger::debug(std::string str) {
  write(str, ANSI_GRAY, "DEBUG");
}

template<typename... Args>
void Logger::debugf(std::string fmt, Args &&...args) {
  debug(std::format(fmt, args...));
}
