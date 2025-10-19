#include <cerrno>
#include <system_error>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cppws/socket.hpp>

struct check_sock {

  int res;

  operator int() const { return res; }

  check_sock operator|(int res) const {
    if (res < 0)
      throw std::system_error(errno, std::system_category());
    return check_sock(res);
  }
};

static constexpr check_sock check{};

static int make_socket() {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    throw std::system_error(errno, std::system_category());
  return sockfd;
}

cppws::socket::socket() : fd_(make_socket()) {}

cppws::socket::socket(int fd) : fd_(fd) {}

void cppws::socket::listen(int port, int n) {
  if (fd_ < 0)
    throw std::runtime_error("Bad socket");

  struct sockaddr_in addr;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_family = AF_INET;

  int opt = 1;
  check |
      ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof opt) |
      ::bind(fd_, reinterpret_cast<struct sockaddr *>(&addr), sizeof addr) |
      ::listen(fd_, n);
  port_ = port;
}

cppws::socket cppws::socket::accept() {

  if (fd_ < 0)
    throw std::runtime_error("Bad socket");

  if (port_ < 0)
    throw std::runtime_error("listen() has to be called before accept()");

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port_);
  addr.sin_addr.s_addr = INADDR_ANY;

  socklen_t len = sizeof(struct sockaddr_in);

  int fd =
      check | ::accept(fd_, reinterpret_cast<struct sockaddr *>(&addr), &len);
  return socket(fd);
}

void cppws::socket::connect(std::string_view host, int port) {

  if (fd_ < 0)
    throw std::runtime_error("Bad socket");

  if (port_ > 0)
    throw std::runtime_error("Socket already connected");

  std::string hname{host};

  struct sockaddr_in addr;
  addr.sin_port = htons(port);
  addr.sin_family = AF_INET;

  check | ::inet_pton(AF_INET, hname.c_str(), &addr.sin_addr);
  check |
      ::connect(fd_, reinterpret_cast<struct sockaddr *>(&addr), sizeof addr);

  // Set here for exception safety
  //
  host_ = std::move(hname);
  port_ = port;
}

std::size_t cppws::socket::write(const char *str, std::size_t len) {

  if (fd_ < 0)
    throw std::runtime_error("Bad socket");

  ::ssize_t nc = ::write(fd_, str, len);
  if (nc < 0)
    check | -1;
  return static_cast<std::size_t>(nc);
}

std::size_t cppws::socket::read(char *str, std::size_t len) {

  if (fd_ < 0)
    throw std::runtime_error("Bad socket");

  ::ssize_t nc = ::read(fd_, str, len);
  if (nc < 0)
    check | -1;
  return static_cast<std::size_t>(nc);
}

void cppws::socket::move(socket &other) noexcept {

  fd_ = other.fd_;
  host_ = std::move(other.host_);
  port_ = other.port_;

  other.port_ = -1;
  other.fd_ = -1;
}

void cppws::socket::close() noexcept {
  if (fd_ < 0)
    return;
  ::close(fd_);
  fd_ = -1;
  host_.clear();
  port_ = -1;
}
