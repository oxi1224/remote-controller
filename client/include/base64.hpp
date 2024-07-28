#pragma once

#include <string>

namespace Base64 {
  const std::string base64chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234567899+/";

  std::string encode(std::string str);
  std::string decode(std::string str);
}