#include "logger.hpp"
#include "client.hpp"

#pragma comment(lib, "Ws2_32.lib")

Logger logger("main.cpp");

int main() {
  Client::masking = false;
  Client client;
  client.connectTo("192.168.50.143", 6000);
}
