#include "base64.hpp"
#include <stdexcept>
#include <string>
#include <vector>

std::string Base64::encode(std::string str) {
  std::string encoded;
  std::vector<unsigned char> data(str.begin(), str.end());

  int padding = (3 - (data.size() % 3)) % 3;
  std::vector<unsigned char> padded = data;
  padded.insert(padded.end(), padding, 0);
  
  for (size_t i = 0; i < padded.size(); i += 3) {
    unsigned int val = (padded[i] << 16) + (padded[i + 1] << 8) + padded[i + 2];
    encoded += base64chars[(val >> 18) & 0x3F];
    encoded += base64chars[(val >> 12) & 0x3F];
    encoded += base64chars[(val >> 6) & 0x3F];
    encoded += base64chars[val & 0x3F];
  }

  encoded.erase(encoded.end() - padding, encoded.end());
  for (int i = 0; i < padding; ++i) {
    encoded += "=";
  }
  return encoded;
}

std::string Base64::decode(std::string str) {
  std::vector<unsigned char> decoded;
  size_t len = str.size();
  int padding = 0;
  if (len > 0 && str[len - 1] == '=') padding++;
  if (len > 1 && str[len - 2] == '=') padding++;

  size_t num_blocks = (len + 3) / 4;
  for (size_t i = 0; i < num_blocks; ++i) {
    unsigned int val = 0;
    for (size_t j = 0; j < 4; ++j) {
      char c = (i * 4 + j < len) ? str[i * 4 + j] : 'A';
      val <<= 6;
      if (c != '=') {
        size_t idx = base64chars.find(c);
        if (idx == std::string::npos) {
          throw new std::invalid_argument("Invalid Base64 character");
        }
        val += idx;
      }
    }

    decoded.push_back((val >> 16) & 0xFF);
    if (i * 4 + 2 < len - padding) decoded.push_back((val >> 8) & 0xFF);
    if (i * 4 + 3 < len - padding) decoded.push_back(val & 0xFF);
  }
  return std::string(decoded.begin(), decoded.end());
}
