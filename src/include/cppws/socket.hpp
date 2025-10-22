#pragma once

#include <string>
#include <string_view>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace cppws {

class socket {
public:
  socket();
  explicit socket(int sockfd);

  void listen(int port, int n = 3);

  socket accept();

  void connect(std::string_view host, int port = 8080);

  std::string_view host() const noexcept {
    return host_.empty() ? std::string_view("127.0.0.1")
                         : std::string_view(host_);
  }

  int port() const noexcept { return port_; }

  std::size_t write(const char *str, std::size_t len);

  std::size_t read(char *str, std::size_t len);

  void close() noexcept;

  operator bool() const noexcept { return fd_ >= 0; }

private:
  int fd_ = -1;
  std::string host_;
  int port_ = -1;

  void move(socket &) noexcept;

public:
  virtual ~socket() noexcept { close(); }
  socket(const socket &) = delete;
  socket(socket &&other) noexcept { move(other); }

  socket &operator=(const socket &) = delete;
  socket &operator=(socket &&other) noexcept {
    if (this == &other)
      return *this;
    close();
    move(other);
    return *this;
  }
};

/**
 * \brief Wrapper around a socket instance, specfically designed for server
 * sockets.
 */
class server_socket : socket {
public:
  /**
   * \brief Construct a new server socket that uses the specified socket for
   * listening for connections.
   * \param sock Socket to use when listening for connections.
   */
  server_socket(socket &&sock) : socket(std::move(sock)) {}

  /**
   * \brief Creates a new server socket that listens on the specified port.
   * \param port Port number to listen on.
   * \param n Maximum number of pending connections at once.
   */
  explicit server_socket(int port, int n = 3) : socket() { listen(port, n); }

  using socket::accept;
  using socket::close;
  using socket::host;
  using socket::port;
  using socket::operator bool;
};

} // namespace cppws
