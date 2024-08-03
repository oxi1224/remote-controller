#pragma once

#include "frame.hpp"
#include "logger.hpp"
#include <string>
#include <winsock2.h>

class Client {
  public:
  inline static const int VERSION = 1;
  inline static bool masking = true;

  ~Client();

  void close();
  // Avoid name collision with winsock2 connect
  void connectTo(std::string ipaddr, int port);
  void sendFrame(Frame f);

  private:
  SOCKET m_socket;
  WSAData m_wsaData;
  inline static Logger logger = Logger("Client");

  void performHandshake();
};
