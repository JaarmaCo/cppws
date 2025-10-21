#pragma once

#include <compare>
#include <cstring>
#include <format>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

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

struct url_authority {

  std::string_view full;

  std::string_view username;
  std::string_view password;
  std::string_view domain;
  short port;
};

class bad_url : public std::runtime_error {
  std::size_t off_;

public:
  bad_url(std::size_t off, const std::string &message)
      : std::runtime_error(
            std::format("URL error at position {}: {}", off, message)),
        off_(off) {}

  explicit bad_url(std::size_t off) : bad_url(off, "Syntax error") {}

  std::size_t location() const noexcept { return off_; }
};

class url {
public:
  static std::string encode(const char *str, std::size_t len);
  static std::string decode(const char *str, std::size_t len);

  static inline std::string encode(const std::string &str) {
    return encode(str.data(), str.length());
  }

  static inline std::string decode(const std::string &str) {
    return decode(str.data(), str.length());
  }

  static inline std::string encode(std::string_view str) {
    return encode(str.data(), str.length());
  }

  static inline std::string decode(std::string_view str) {
    return decode(str.data(), str.length());
  }

  static inline std::string encode(const char *str) {
    return encode(str, std::strlen(str));
  }

  static inline std::string decode(const char *str) {
    return decode(str, std::strlen(str));
  }

  url() = default;
  url(std::string &&str);
  url(const std::string &str);
  url(const char *str);
  url(const char *str, std::size_t len);

  std::string_view string_view() const noexcept { return full_; }

  std::string_view scheme() const noexcept { return scheme_; }

  const url_authority &authority() const noexcept { return auth_; }

  const std::vector<std::string_view> &path() const noexcept { return path_; }

  std::string_view path_str() const noexcept { return pathStr_; }

  std::string_view fragment() const noexcept { return fragment_; }

  std::optional<std::string_view> query(std::string_view key) const noexcept {
    auto it = query_.find(key);
    if (it != query_.end())
      return std::optional(it->second);
    return std::nullopt;
  }

  const std::vector<std::string_view> &query_params() const noexcept {
    return queryParams_;
  }

  url_decode_range decode() const {
    return {{data(), data() + length()},
            {data() + length(), data() + length()}};
  }

  url parent() const;

  url operator/(std::string_view) const;

  std::strong_ordering operator<=>(const url &other) const noexcept {
    return full_ <=> other.full_;
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

  auto begin() const noexcept { return full_.begin(); }
  auto end() const noexcept { return full_.end(); }

  const char *data() const noexcept { return full_.data(); }

  std::size_t length() const noexcept { return full_.length(); }

  std::size_t size() const noexcept { return length(); }

private:
  void decompose();
  std::string_view decompose_scheme(std::string_view);
  std::string_view decompose_authority(std::string_view);
  std::string_view decompose_path(std::string_view);
  std::string_view decompose_query(std::string_view);

  std::shared_ptr<std::string> str_;

  std::string_view full_;

  std::string_view scheme_;
  url_authority auth_;
  std::string_view pathStr_;
  std::vector<std::string_view> path_;
  std::vector<std::string_view> queryParams_;
  std::unordered_map<std::string_view, std::string_view> query_;
  std::string_view fragment_;
};

namespace url_literals {

inline url operator""_url(const char *str, std::size_t len) {
  return url(str, len);
}

} // namespace url_literals

} // namespace cppws
