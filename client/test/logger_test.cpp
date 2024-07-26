#include <fstream>
#include <gtest/gtest.h>
#include <iterator>
#include <logger.hpp>

TEST(LoggerTest, MainTest) {
  Logger l;
  l.log("Hello World");
  std::ifstream logFile("log.txt", std::ios_base::app);
  std::string contents(
    (std::istreambuf_iterator<char>(logFile)),
    std::istreambuf_iterator<char>()
  );
  size_t content_pos = contents.find_first_of(" ");
  std::string line = contents.substr(content_pos + 1, contents.length());
  ASSERT_EQ("[INFO] Hello World\n", line);
}
