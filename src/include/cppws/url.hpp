#pragma once

#include <compare>
#include <stdexcept>
#include <string>

namespace cppws {

class url_decode_iterator {
  const char *ptr_;
  const char *end_;

public:
  url_decode_iterator(const char *start, const char *end)
      : ptr_(start), end_(end) {}

  url_decode_iterator &operator++() {
    if (ptr_ == end_)
      throw std::runtime_error("Iterator moved past end.");
    if (*ptr_ == '%') {
      const char *exp = ptr_ + 3;
      for (; ptr_ != exp && ptr_ != end_; ++ptr_)
        ;
    } else {
      ++ptr_;
    }
    return *this;
  }

  url_decode_iterator operator++(int) {
    auto cp = *this;
    ++*this;
    return cp;
  }

  char operator*() {
    if (*ptr_ == '%') {

      if (end_ - ptr_ < 3)
        throw std::runtime_error("Expected two hexadecimal values after '%'");

      int lo = ptr_[1];
      int hi = ptr_[2];

      if (lo <= 'f' && lo >= 'a')
        lo = lo - 'a' + 10;
      else if (lo <= 'F' && lo >= 'A')
        lo = lo - 'A' + 10;
      else if (lo <= '9' && lo >= '0')
        lo = lo - '0';
      else
        throw std::runtime_error("Expected a hexadecimal digit after '%'");

      if (hi <= 'f' && hi >= 'a')
        hi = hi - 'a' + 10;
      else if (hi <= 'F' && hi >= 'A')
        hi = hi - 'A' + 10;
      else if (hi <= '9' && hi >= '0')
        hi = hi - '0';
      else
        throw std::runtime_error(
            "Expected a hexadecimal digit after '%' and first hex digit.");

      return lo | (hi << 4);

    } else {
      return *ptr_;
    }
  }

  bool operator==(const url_decode_iterator &other) const {
    return ptr_ == other.ptr_;
  }
  bool operator!=(const url_decode_iterator &other) const {
    return ptr_ != other.ptr_;
  }
};

struct url_decode_range {
  url_decode_iterator first;
  url_decode_iterator last;

  url_decode_iterator begin() const { return first; }
  url_decode_iterator end() const { return last; }
};

class url : std::string {
public:
  static bool validate(const char *str, std::size_t len);
  static url encode(const char *str, std::size_t len);

  url() = default;
  url(std::string &&str);
  url(const std::string &str);
  url(const char *str);
  url(const char *str, std::size_t len);

  const std::string &string() const & noexcept {
    return static_cast<const std::string &>(*this);
  }

  std::string &string() & noexcept { return static_cast<std::string &>(*this); }

  std::string &&string() && noexcept {
    return std::move(static_cast<std::string &>(*this));
  }

  std::string_view protocol() const;

  std::string_view domain() const;

  std::string_view path() const;

  url_decode_range decode() const {
    return {{data(), data() + length()},
            {data() + length(), data() + length()}};
  }

  url parent() const;
  url root() const;

  url operator/(const url &) const &;
  url operator/(url &&) const &;
  url &&operator/(const url &) &&;
  url &&operator/(url &&) &&;

  std::strong_ordering operator<=>(const url &other) const noexcept {
    return static_cast<const std::string &>(*this) <=>
           static_cast<const std::string &>(other);
  }

  bool operator==(const url &other) const noexcept {
    return *this <=> other == 0;
  }

  bool operator!=(const url &other) const noexcept {
    return *this <=> other != 0;
  }

  bool operator>=(const url &other) const noexcept {
    return *this <=> other >= 0;
  }

  bool operator<=(const url &other) const noexcept {
    return *this <=> other <= 0;
  }

  bool operator>(const url &other) const noexcept {
    return *this <=> other > 0;
  }

  bool operator<(const url &other) const noexcept {
    return *this <=> other < 0;
  }

  using std::string::begin;
  using std::string::c_str;
  using std::string::cbegin;
  using std::string::cend;
  using std::string::crbegin;
  using std::string::crend;
  using std::string::data;
  using std::string::end;
  using std::string::length;
  using std::string::rbegin;
  using std::string::rend;
  using std::string::size;
};

namespace url_literals {

inline url operator""_url(const char *str, std::size_t len) {
  return url(str, len);
}

} // namespace url_literals

} // namespace cppws
