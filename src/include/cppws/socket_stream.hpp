#pragma once

#include <iostream>
#include <memory_resource>
#include <vector>

#include <cppws/socket.hpp>

namespace cppws {

/**
 * \brief streambuf implementation that reads characters from a socket.
 *
 * \tparam Char character type
 * \tparam Traits character traits of the stream
 * \tparam Alloc allocator used to allocate the stream buffer.
 */
template <typename Char, typename Traits = std::char_traits<Char>,
          typename Alloc = std::allocator<Char>>
class basic_socket_streambuf : public std::streambuf {
  std::vector<Char, Alloc> ibuf_;
  std::vector<Char, Alloc> obuf_;
  class socket socket_;

public:
  basic_socket_streambuf() = default;

  /**
   * \brief Constructs a new stream buffer.
   *
   * \param sock Socket to assume ownership of.
   * \param allocator Allocator used to allocate the stream buffer.
   * \param ibufsz Size of the input buffer.
   * \param obufsz Size of the output buffer.
   */
  basic_socket_streambuf(class socket &&sock, const Alloc &allocator,
                         std::size_t ibufsz = 1024, std::size_t obufsz = 1024)
      : socket_(std::move(sock)), ibuf_(allocator), obuf_(allocator) {
    ibuf_.resize(ibufsz);
    obuf_.resize(obufsz);
  }

  /**
   * \brief Gets the size of the input buffer.
   */
  std::size_t in_buffer_size() const noexcept { return ibuf_.size(); }

  /**
   * \brief Gets the size of the output buffer.
   */
  std::size_t out_buffer_size() const noexcept { return obuf_.size(); }

  /**
   * \brief Gets the socket associated with this socket stream buffer.
   */
  class socket &socket() noexcept { return socket_; }

  /**
   * \brief Gets the socket associated with this socket stream buffer.
   */
  const class socket &socket() const noexcept { return socket_; }

protected:
  int_type overflow(int_type ch) override {
    if (pptr() == epptr()) {
      sync();
    }
    if (pptr() != epptr()) {
      Traits::assign(*pptr(), ch);
      return *pptr();
    }
    return Traits::eof();
  }

  int_type sync() override {
    std::size_t n = socket_.write(reinterpret_cast<const char *>(obuf_.data()),
                                  obuf_.size() * sizeof(Char));
    setp(obuf_.data(), obuf_.data() + n / sizeof(Char));
    return pptr() == epptr() ? Traits::eof() : *pptr();
  }

  int_type underflow() override {
    if (gptr() == egptr()) {
      std::size_t n = socket_.read(reinterpret_cast<char *>(ibuf_.data()),
                                   ibuf_.size() * sizeof(Char));
      setg(ibuf_.data(), ibuf_.data(), ibuf_.data() + n / sizeof(Char));
    }
    return gptr() == egptr() ? Traits::eof() : Traits::to_int_type(*gptr());
  }
};

/**
 * \brief I/O stream for reading/writing to a socket.
 */
template <typename Char, typename Traits = std::char_traits<Char>,
          typename Alloc = std::allocator<Char>>
class basic_socket_iostream : public std::basic_iostream<Char, Traits> {
  basic_socket_streambuf<Char, Traits, Alloc> streambuf_;

public:
  basic_socket_iostream() = default;

  /**
   * \brief Constructs a new socket stream.
   * \param socket Socket to read/write
   * \param allocator Allocator used to allocate the internal stream buffer.
   */
  basic_socket_iostream(socket &&socket, const Alloc &allocator = Alloc())
      : std::basic_iostream<Char, Traits>(&streambuf_),
        streambuf_(std::move(socket), allocator) {}

  /**
   * \brief Constructs a new socket stream.
   * \param buffer Socket stream buffer to use.
   */
  basic_socket_iostream(basic_socket_streambuf<Char, Traits, Alloc> &&buffer)
      : std::basic_iostream<Char, Traits>(&streambuf_),
        streambuf_(std::move(buffer)) {}

  /**
   * \brief Gets the underlying socket stream buffer.
   */
  basic_socket_streambuf<Char, Traits, Alloc> &socket_streambuf() noexcept {
    return streambuf_;
  }

  /**
   * \brief Gets the underlying socket stream buffer.
   */
  const basic_socket_streambuf<Char, Traits, Alloc> &
  socket_streambuf() const noexcept {
    return streambuf_;
  }

  /**
   * \brief Gets the socket associated with this stream.
   */
  class socket &socket() noexcept { return socket_streambuf().socket(); }

  /**
   * \brief Gets the socket associated with this stream.
   */
  const class socket &socket() const noexcept {
    return socket_streambuf().socket();
  }

  virtual ~basic_socket_iostream() noexcept {}
  basic_socket_iostream(basic_socket_iostream &&) noexcept = default;
  basic_socket_iostream(const basic_socket_iostream &) = delete;

  basic_socket_iostream &operator=(basic_socket_iostream &&) noexcept = default;
  basic_socket_iostream &operator=(const basic_socket_iostream &) = delete;
};

/**
 * \brief Input stream for reading from a socket.
 */
template <typename Char, typename Traits = std::char_traits<Char>,
          typename Alloc = std::allocator<Char>>
class basic_socket_istream : public std::basic_istream<Char, Traits> {
  basic_socket_streambuf<Char, Traits, Alloc> streambuf_;

public:
  basic_socket_istream() = default;

  /**
   * \brief Constructs a new socket stream.
   * \param socket Socket to read/write
   * \param allocator Allocator used to allocate the internal stream buffer.
   */
  basic_socket_istream(socket &&socket, const Alloc &allocator = Alloc())
      : std::basic_istream<Char, Traits>(&streambuf_),
        streambuf_(std::move(socket), allocator, 1024, 0) {}

  /**
   * \brief Constructs a new socket stream.
   * \param buffer Socket stream buffer to use.
   */
  basic_socket_istream(basic_socket_streambuf<Char, Traits, Alloc> &&buffer)
      : std::basic_istream<Char, Traits>(&streambuf_),
        streambuf_(std::move(buffer)) {}

  /**
   * \brief Gets the underlying socket stream buffer.
   */
  basic_socket_streambuf<Char, Traits, Alloc> &socket_streambuf() noexcept {
    return streambuf_;
  }

  /**
   * \brief Gets the underlying socket stream buffer.
   */
  const basic_socket_streambuf<Char, Traits, Alloc> &
  socket_streambuf() const noexcept {
    return streambuf_;
  }

  /**
   * \brief Gets the socket associated with this stream.
   */
  class socket &socket() noexcept { return socket_streambuf().socket(); }

  /**
   * \brief Gets the socket associated with this stream.
   */
  const class socket &socket() const noexcept {
    return socket_streambuf().socket();
  }

  virtual ~basic_socket_istream() noexcept {}
  basic_socket_istream(basic_socket_istream &&) noexcept = default;
  basic_socket_istream(const basic_socket_istream &) = delete;

  basic_socket_istream &operator=(basic_socket_istream &&) noexcept = default;
  basic_socket_istream &operator=(const basic_socket_istream &) = delete;
};

/**
 * \brief Output stream for writing to a socket.
 */
template <typename Char, typename Traits = std::char_traits<Char>,
          typename Alloc = std::allocator<Char>>
class basic_socket_ostream : public std::basic_ostream<Char, Traits> {
  basic_socket_streambuf<Char, Traits, Alloc> streambuf_;

public:
  basic_socket_ostream() = default;

  /**
   * \brief Constructs a new socket stream.
   * \param socket Socket to read/write
   * \param allocator Allocator used to allocate the internal stream buffer.
   */
  basic_socket_ostream(socket &&socket, const Alloc &allocator = Alloc())
      : std::basic_ostream<Char, Traits>(&streambuf_),
        streambuf_(std::move(socket), allocator, 0, 1024) {}

  /**
   * \brief Constructs a new socket stream.
   * \param buffer Socket stream buffer to use.
   */
  basic_socket_ostream(basic_socket_streambuf<Char, Traits, Alloc> &&buffer)
      : std::basic_ostream<Char, Traits>(&streambuf_),
        streambuf_(std::move(buffer)) {}

  /**
   * \brief Gets the underlying socket stream buffer.
   */
  basic_socket_streambuf<Char, Traits, Alloc> &socket_streambuf() noexcept {
    return streambuf_;
  }

  /**
   * \brief Gets the underlying socket stream buffer.
   */
  const basic_socket_streambuf<Char, Traits, Alloc> &
  socket_streambuf() const noexcept {
    return streambuf_;
  }

  /**
   * \brief Gets the socket associated with this stream.
   */
  class socket &socket() noexcept { return socket_streambuf().socket(); }

  /**
   * \brief Gets the socket associated with this stream.
   */
  const class socket &socket() const noexcept {
    return socket_streambuf().socket();
  }

  virtual ~basic_socket_ostream() noexcept {}
  basic_socket_ostream(basic_socket_ostream &&) noexcept = default;
  basic_socket_ostream(const basic_socket_ostream &) = delete;

  basic_socket_ostream &operator=(basic_socket_ostream &&) noexcept = default;
  basic_socket_ostream &operator=(const basic_socket_ostream &) = delete;
};

using socket_streambuf = basic_socket_streambuf<char>;
using socket_wstreambuf = basic_socket_streambuf<wchar_t>;
using socket_u8streambuf = basic_socket_streambuf<char8_t>;
using socket_u16streambuf = basic_socket_streambuf<char16_t>;
using socket_u32streambuf = basic_socket_streambuf<char32_t>;

using socket_iostream = basic_socket_iostream<char>;
using socket_istream = basic_socket_istream<char>;
using socket_ostream = basic_socket_ostream<char>;

using socket_wiostream = basic_socket_iostream<wchar_t>;
using socket_wistream = basic_socket_istream<wchar_t>;
using socket_wostream = basic_socket_ostream<wchar_t>;

using socket_u8iostream = basic_socket_iostream<char8_t>;
using socket_u8istream = basic_socket_istream<char8_t>;
using socket_u8ostream = basic_socket_ostream<char8_t>;

using socket_u16iostream = basic_socket_iostream<char16_t>;
using socket_u16istream = basic_socket_istream<char16_t>;
using socket_u16ostream = basic_socket_ostream<char16_t>;

using socket_u32iostream = basic_socket_iostream<char32_t>;
using socket_u32istream = basic_socket_istream<char32_t>;
using socket_u32ostream = basic_socket_ostream<char32_t>;

namespace pmr {

using socket_streambuf =
    basic_socket_streambuf<char, std::char_traits<char>,
                           std::pmr::polymorphic_allocator<char>>;
using socket_wstreambuf =
    basic_socket_streambuf<wchar_t, std::char_traits<wchar_t>,
                           std::pmr::polymorphic_allocator<wchar_t>>;
using socket_u8streambuf =
    basic_socket_streambuf<char8_t, std::char_traits<char8_t>,
                           std::pmr::polymorphic_allocator<char8_t>>;
using socket_u16streambuf =
    basic_socket_streambuf<char16_t, std::char_traits<char16_t>,
                           std::pmr::polymorphic_allocator<char16_t>>;
using socket_u32streambuf =
    basic_socket_streambuf<char32_t, std::char_traits<char32_t>,
                           std::pmr::polymorphic_allocator<char32_t>>;

using socket_iostream =
    basic_socket_iostream<char, std::char_traits<char>,
                          std::pmr::polymorphic_allocator<char>>;
using socket_istream =
    basic_socket_istream<char, std::char_traits<char>,
                         std::pmr::polymorphic_allocator<char>>;
using socket_ostream =
    basic_socket_ostream<char, std::char_traits<char>,
                         std::pmr::polymorphic_allocator<char>>;

using socket_wiostream =
    basic_socket_iostream<wchar_t, std::char_traits<wchar_t>,
                          std::pmr::polymorphic_allocator<wchar_t>>;
using socket_wistream =
    basic_socket_istream<wchar_t, std::char_traits<wchar_t>,
                         std::pmr::polymorphic_allocator<wchar_t>>;
using socket_wostream =
    basic_socket_ostream<wchar_t, std::char_traits<wchar_t>,
                         std::pmr::polymorphic_allocator<wchar_t>>;

using socket_u8iostream =
    basic_socket_iostream<char8_t, std::char_traits<char8_t>,
                          std::pmr::polymorphic_allocator<char8_t>>;
using socket_u8istream =
    basic_socket_istream<char8_t, std::char_traits<char8_t>,
                         std::pmr::polymorphic_allocator<char8_t>>;
using socket_u8ostream =
    basic_socket_ostream<char8_t, std::char_traits<char8_t>,
                         std::pmr::polymorphic_allocator<char8_t>>;

using socket_u16iostream =
    basic_socket_iostream<char16_t, std::char_traits<char16_t>,
                          std::pmr::polymorphic_allocator<char16_t>>;
using socket_u16istream =
    basic_socket_istream<char16_t, std::char_traits<char16_t>,
                         std::pmr::polymorphic_allocator<char16_t>>;
using socket_u16ostream =
    basic_socket_ostream<char16_t, std::char_traits<char16_t>,
                         std::pmr::polymorphic_allocator<char16_t>>;

using socket_u32iostream =
    basic_socket_iostream<char32_t, std::char_traits<char32_t>,
                          std::pmr::polymorphic_allocator<char32_t>>;
using socket_u32istream =
    basic_socket_istream<char32_t, std::char_traits<char32_t>,
                         std::pmr::polymorphic_allocator<char32_t>>;
using socket_u32ostream =
    basic_socket_ostream<char32_t, std::char_traits<char32_t>,
                         std::pmr::polymorphic_allocator<char32_t>>;

} // namespace pmr

} // namespace cppws
