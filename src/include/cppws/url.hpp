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

/**
 * \brief _ForwardIterator_ that decodes URL strings as it is traversed.
 */
class url_decode_iterator {
  const char *ptr_;
  const char *end_;

public:
  constexpr url_decode_iterator(const char *start, const char *end)
      : ptr_(start), end_(end) {}

  constexpr url_decode_iterator &operator++() {
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

  constexpr url_decode_iterator operator++(int) {
    auto cp = *this;
    ++*this;
    return cp;
  }

  constexpr char operator*() {
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

  constexpr bool operator==(const url_decode_iterator &other) const {
    return ptr_ == other.ptr_;
  }
  constexpr bool operator!=(const url_decode_iterator &other) const {
    return ptr_ != other.ptr_;
  }
};

/**
 * \brief Pair of url_decode_iterator that can be iterated on.
 */
struct url_decode_range {
  url_decode_iterator first;
  url_decode_iterator last;

  url_decode_iterator begin() const { return first; }
  url_decode_iterator end() const { return last; }

  static constexpr url_decode_range
  from_string_view(std::string_view v) noexcept {
    return {{v.data(), v.data() + v.length()},
            {v.data() + v.length(), v.data() + v.length()}};
  }
};

/**
 * \brief The _authority_ part of a URL: [[username]:[password]@][domain][:port]
 */
struct url_authority {

  // Full view over the URL authority
  //
  std::string_view full;

  std::string_view username;
  std::string_view password;
  std::string_view domain;
  short port;

  /**
   * \brief Decoded view into the authority username.
   */
  url_decode_range decoded_username() const noexcept {
    return url_decode_range::from_string_view(username);
  }

  /**
   * \brief Decoded view into the authority password.
   */
  url_decode_range decoded_password() const noexcept {
    return url_decode_range::from_string_view(password);
  }

  /**
   * \brief Decoded view into the authority domain.
   */
  url_decode_range decoded_domain() const noexcept {
    return url_decode_range::from_string_view(domain);
  }
};

/**
 * \brief Exception thrown if a string did not follow the URL format.
 */
class bad_url : public std::runtime_error {
  std::size_t off_;

public:
  /**
   * \brief Constructor.
   * \param off Offset into the URL string where the error occured.
   * \param message Error message.
   */
  bad_url(std::size_t off, const std::string &message)
      : std::runtime_error(
            std::format("URL error at position {}: {}", off, message)),
        off_(off) {}

  /**
   * \brief Constructor.
   * \param off Offset into the URL where the error occured.
   */
  explicit bad_url(std::size_t off) : bad_url(off, "Syntax error") {}

  /**
   * \brief Position into the URL where the error occured.
   */
  std::size_t location() const noexcept { return off_; }
};

/**
 * \brief Helper class for handling URLs
 */
class url {
public:
  /**
   * \brief Encodes the given string so it can be safely included in a URL.
   * \group cppws::url::encode
   * \{
   */
  static std::string encode(const char *str, std::size_t len);

  static inline std::string encode(const std::string &str) {
    return encode(str.data(), str.length());
  }

  static inline std::string encode(std::string_view str) {
    return encode(str.data(), str.length());
  }

  static inline std::string encode(const char *str) {
    return encode(str, std::strlen(str));
  }
  /** \} */

  /**
   * \brief Decodes the given URL encoded string to it's literal representation.
   * \group cppws::url::decode
   * \{
   */
  static std::string decode(const char *str, std::size_t len);

  static inline std::string decode(const std::string &str) {
    return decode(str.data(), str.length());
  }

  static inline std::string decode(std::string_view str) {
    return decode(str.data(), str.length());
  }

  static inline std::string decode(const char *str) {
    return decode(str, std::strlen(str));
  }
  /** \} */

  /**
   * \brief Constructs an empty URL.
   */
  url() = default;

  /**
   * \brief Constructs a URL from a string.
   * \group cppws::url::url
   * \{
   */
  url(std::string &&str);
  url(const std::string &str);
  url(const char *str);
  url(const char *str, std::size_t len);
  /** \} */

  /**
   * \brief Gets a view into the full URL.
   */
  std::string_view string_view() const noexcept { return full_; }

  /**
   * \brief Gets the scheme part of the URL.
   */
  std::string_view scheme() const noexcept { return scheme_; }

  /**
   * \brief Gets the authority part of the URL.
   */
  const url_authority &authority() const noexcept { return auth_; }

  /**
   * \brief Gets the path of the URL.
   * \group cppws::url::path
   * \{
   */
  const std::vector<std::string_view> &path() const noexcept { return path_; }

  std::vector<url_decode_range> decoded_path() const noexcept {
    std::vector<url_decode_range> r;
    for (std::string_view v : path()) {
      r.push_back(url_decode_range::from_string_view(v));
    }
    return r;
  }
  /** \} */

  /**
   * \brief Gets the path, as a string, of the URL.
   * \group cppws::url::path_str
   * \{
   */
  std::string_view path_str() const noexcept { return pathStr_; }

  url_decode_range decoded_path_str() const noexcept {
    return url_decode_range::from_string_view(path_str());
  }
  /** \} */

  /**
   * \brief Gets the fragment part of the URL.
   * \group cppws::url::fragment
   * \{
   */
  std::string_view fragment() const noexcept { return fragment_; }

  url_decode_range decoded_fragment() const noexcept {
    return url_decode_range::from_string_view(fragment());
  }
  /** \} */

  /**
   * \brief Extracts a query parameter from the URL.
   * \group cppws::url::query
   * \{
   */
  std::optional<std::string_view> query(std::string_view key) const noexcept {
    auto it = query_.find(decode(key));
    if (it != query_.end())
      return std::optional(it->second);
    return std::nullopt;
  }

  std::optional<url_decode_range>
  decoded_query(std::string_view key) const noexcept {
    auto opt = query(key);
    return opt.has_value()
               ? std::optional(url_decode_range::from_string_view(opt.value()))
               : std::nullopt;
  }
  /** \} */

  /**
   * \brief Extracts an ordered list of query parameter keys.
   * \group cppws::url::query_params
   * \{
   */
  const std::vector<std::string_view> &query_params() const noexcept {
    return queryParams_;
  }

  std::vector<url_decode_range> decoded_query_params() const noexcept {
    std::vector<url_decode_range> r;
    for (std::string_view p : query_params())
      r.push_back(url_decode_range::from_string_view(p));
    return r;
  }
  /** \} */

  /**
   * \brief Gets a decoded range over the URL string.
   */
  url_decode_range decode() const {
    return url_decode_range::from_string_view(full_);
  }

  /**
   * \brief Gets the URL without the last path element of this URL.
   *
   * Example: http://website.org/path/to/item.html -> http://website.org/path/to
   */
  url parent() const;

  /**
   * \brief Creates a new URL by appending the specified string to the path of
   * this URL.
   */
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
