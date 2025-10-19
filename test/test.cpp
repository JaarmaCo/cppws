#include <iostream>

#include <cppws/socket.hpp>
#include <cppws/socket_stream.hpp>

int main() {

  cppws::server_socket server{1234};
  {
    cppws::socket_istream client = server.accept();
    std::string line;
    while (std::getline(client, line)) {
      std::cout << line << std::endl;
    }
  }
  return 0;
}
