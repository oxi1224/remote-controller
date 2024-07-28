#include <gtest/gtest.h>
#include <string>
#include "base64.hpp"

TEST(Base64Test, EncodeTest) {
  std::string in = "Hello world!!";
  std::string expected = "SGVsbG8gd29ybGQhIQ==";
  std::string out = Base64::encode(in);
  ASSERT_EQ(expected, out);
}

TEST(Base64Test, DecodeTest) {
  std::string in = "SGVsbG8gd29ybGQhIQ==";
  std::string expected = "Hello world!!";
  std::string out = Base64::decode(in);
  ASSERT_EQ(expected, out);
}

