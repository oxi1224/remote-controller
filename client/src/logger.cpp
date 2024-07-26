#include <ctime>
#include <format>
#include <fstream>
#include <iostream>
#include <logger.hpp>

Logger::Logger() {
  if (log_to_file == true) m_file = std::ofstream(m_logFileName, std::fstream::app);
}

Logger::Logger(std::string filename) {
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

void Logger::write(std::string txt, std::string ansi_color) {
  std::cout << ANSI_GREEN << getTimeStamp() << ANSI_RESET << ansi_color << txt << ANSI_RESET << std::endl;
  if (log_to_file && !m_file.is_open()) {
    m_file = std::ofstream(m_logFileName, std::fstream::app);
  }
  if (log_to_file) m_file << getTimeStamp() << txt << std::endl;
}

void Logger::log(std::string str) {
  write("[INFO] " + str, ANSI_WHITE);
}

template<typename... Args>
void Logger::logf(std::string fmt, Args &&...args) {
  log(std::format(fmt, args...));
}

void Logger::warn(std::string str) {
  write("[WARN] " + str, ANSI_YELLOW);
}

template<typename... Args>
void Logger::warnf(std::string fmt, Args &&...args) {
  warn(std::format(fmt, args...));
}

void Logger::error(std::string str) {
  write("[ERROR] " + str, ANSI_RED);
}

template<typename... Args>
void Logger::errorf(std::string fmt, Args &&...args) {
  error(std::format(fmt, args...));
}

void Logger::debug(std::string str) {
  write("[DEBUG] " + str, ANSI_GRAY);
}

template<typename... Args>
void Logger::debugf(std::string fmt, Args &&...args) {
  debug(std::format(fmt, args...));
}
