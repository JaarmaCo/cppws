#pragma once

#include <iostream>
#include <memory_resource>
#include <string>
#include <unordered_map>

#include <cppws/url.hpp>

namespace cppws {

/**
 * Enumerates all available http methods.
 */
enum class http_method {
  GET,
  HEAD,
  POST,
  PUT,
  DELETE,
  CONNECT,
  OPTIONS,
  TRACE,
  PATCH
};

enum class http_request_header {
  Accept,
  AcceptCharset,
  AcceptEncoding,
  AcceptLanguage,
  Authorization,
  CacheControl,
  Connection,
  Cookie,
  ContentLength,
  ContentType,
  Date,
  Expect,
  From,
  Host,
  IfMatch,
  IfModifiedSince,
  IfNoneMatch,
  IfRange,
  IfUnmodifiedSince,
  MaxForwards,
  Pragma,
  ProxyAuthorization,
  Range,
  Referer,
  TE,
  Upgrade,
  UserAgent,
  Via,
  Warning,
  Unknown
};

constexpr std::string_view to_string(http_request_header header) noexcept {
  switch (header) {
  case http_request_header::Accept:
    return "Accept";
  case http_request_header::AcceptCharset:
    return "Accept-Charset";
  case http_request_header::AcceptEncoding:
    return "Accept-Encoding";
  case http_request_header::AcceptLanguage:
    return "Accept-Language";
  case http_request_header::Authorization:
    return "Authorization";
  case http_request_header::CacheControl:
    return "Cache-Control";
  case http_request_header::Connection:
    return "Connection";
  case http_request_header::Cookie:
    return "Cookie";
  case http_request_header::ContentLength:
    return "Content-Length";
  case http_request_header::ContentType:
    return "Content-Type";
  case http_request_header::Date:
    return "Date";
  case http_request_header::Expect:
    return "Expect";
  case http_request_header::From:
    return "From";
  case http_request_header::Host:
    return "Host";
  case http_request_header::IfMatch:
    return "If-Match";
  case http_request_header::IfModifiedSince:
    return "If-Modified-Since";
  case http_request_header::IfNoneMatch:
    return "If-None-Match";
  case http_request_header::IfRange:
    return "If-Range";
  case http_request_header::IfUnmodifiedSince:
    return "If-Unmodified-Since";
  case http_request_header::MaxForwards:
    return "Max-Forwards";
  case http_request_header::Pragma:
    return "Pragma";
  case http_request_header::ProxyAuthorization:
    return "Proxy-Authorization";
  case http_request_header::Range:
    return "Range";
  case http_request_header::Referer:
    return "Referer";
  case http_request_header::TE:
    return "TE";
  case http_request_header::Upgrade:
    return "Upgrade";
  case http_request_header::UserAgent:
    return "User-Agent";
  case http_request_header::Via:
    return "Via";
  case http_request_header::Warning:
    return "Warning";
  default:
    return "Unknown";
  }
}

namespace {
constexpr bool iequals(std::string_view a, std::string_view b) noexcept {
  if (a.size() != b.size())
    return false;
  for (size_t i = 0; i < a.size(); ++i) {
    unsigned char ca = static_cast<unsigned char>(a[i]);
    unsigned char cb = static_cast<unsigned char>(b[i]);
    if (ca >= 'A' && ca <= 'Z')
      ca += 32;
    if (cb >= 'A' && cb <= 'Z')
      cb += 32;
    if (ca != cb)
      return false;
  }
  return true;
}
} // namespace

constexpr std::from_chars_result
from_chars(const char *first, const char *last,
           http_request_header &value) noexcept {
  // Convert input range to string_view for convenience
  std::string_view sv(first, static_cast<size_t>(last - first));

  if (iequals(sv, "Accept")) {
    value = http_request_header::Accept;
    return {last, {}};
  }
  if (iequals(sv, "Accept-Charset")) {
    value = http_request_header::AcceptCharset;
    return {last, {}};
  }
  if (iequals(sv, "Accept-Encoding")) {
    value = http_request_header::AcceptEncoding;
    return {last, {}};
  }
  if (iequals(sv, "Accept-Language")) {
    value = http_request_header::AcceptLanguage;
    return {last, {}};
  }
  if (iequals(sv, "Authorization")) {
    value = http_request_header::Authorization;
    return {last, {}};
  }
  if (iequals(sv, "Cache-Control")) {
    value = http_request_header::CacheControl;
    return {last, {}};
  }
  if (iequals(sv, "Connection")) {
    value = http_request_header::Connection;
    return {last, {}};
  }
  if (iequals(sv, "Cookie")) {
    value = http_request_header::Cookie;
    return {last, {}};
  }
  if (iequals(sv, "Content-Length")) {
    value = http_request_header::ContentLength;
    return {last, {}};
  }
  if (iequals(sv, "Content-Type")) {
    value = http_request_header::ContentType;
    return {last, {}};
  }
  if (iequals(sv, "Date")) {
    value = http_request_header::Date;
    return {last, {}};
  }
  if (iequals(sv, "Expect")) {
    value = http_request_header::Expect;
    return {last, {}};
  }
  if (iequals(sv, "From")) {
    value = http_request_header::From;
    return {last, {}};
  }
  if (iequals(sv, "Host")) {
    value = http_request_header::Host;
    return {last, {}};
  }
  if (iequals(sv, "If-Match")) {
    value = http_request_header::IfMatch;
    return {last, {}};
  }
  if (iequals(sv, "If-Modified-Since")) {
    value = http_request_header::IfModifiedSince;
    return {last, {}};
  }
  if (iequals(sv, "If-None-Match")) {
    value = http_request_header::IfNoneMatch;
    return {last, {}};
  }
  if (iequals(sv, "If-Range")) {
    value = http_request_header::IfRange;
    return {last, {}};
  }
  if (iequals(sv, "If-Unmodified-Since")) {
    value = http_request_header::IfUnmodifiedSince;
    return {last, {}};
  }
  if (iequals(sv, "Max-Forwards")) {
    value = http_request_header::MaxForwards;
    return {last, {}};
  }
  if (iequals(sv, "Pragma")) {
    value = http_request_header::Pragma;
    return {last, {}};
  }
  if (iequals(sv, "Proxy-Authorization")) {
    value = http_request_header::ProxyAuthorization;
    return {last, {}};
  }
  if (iequals(sv, "Range")) {
    value = http_request_header::Range;
    return {last, {}};
  }
  if (iequals(sv, "Referer")) {
    value = http_request_header::Referer;
    return {last, {}};
  }
  if (iequals(sv, "TE")) {
    value = http_request_header::TE;
    return {last, {}};
  }
  if (iequals(sv, "Upgrade")) {
    value = http_request_header::Upgrade;
    return {last, {}};
  }
  if (iequals(sv, "User-Agent")) {
    value = http_request_header::UserAgent;
    return {last, {}};
  }
  if (iequals(sv, "Via")) {
    value = http_request_header::Via;
    return {last, {}};
  }
  if (iequals(sv, "Warning")) {
    value = http_request_header::Warning;
    return {last, {}};
  }

  // No match
  return {first, std::errc::invalid_argument};
}

/**
 * \brief Gets the string representation of the specified HTTP method.
 */
constexpr std::string_view to_string(http_method m) {
  switch (m) {
    using enum http_method;
  case GET:
    return "GET";
  case HEAD:
    return "HEAD";
  case POST:
    return "POST";
  case PUT:
    return "PUT";
  case DELETE:
    return "DELETE";
  case CONNECT:
    return "CONNECT";
  case OPTIONS:
    return "OPTIONS";
  case TRACE:
    return "TRACE";
  case PATCH:
    return "PATCH";
  default:
    return "";
  }
}

/**
 * \brief Similar to std::from_chars: parses an http_method instance from a
 * string range.
 */
constexpr std::from_chars_result from_chars(const char *first, const char *last,
                                            http_method &method) noexcept {
  std::string_view v{first, last};
  if (v.starts_with("GET ")) {
    method = http_method::GET;
    return {first + 4, std::errc{}};
  } else if (v.starts_with("HEAD ")) {
    method = http_method::HEAD;
    return {first + 5, std::errc{}};
  } else if (v.starts_with("POST ")) {
    method = http_method::POST;
    return {first + 5, std::errc{}};
  } else if (v.starts_with("PUT ")) {
    method = http_method::PUT;
    return {first + 4, std::errc{}};
  } else if (v.starts_with("DELETE ")) {
    method = http_method::DELETE;
    return {first + 7, std::errc{}};
  } else if (v.starts_with("OPTIONS ")) {
    method = http_method::OPTIONS;
    return {first + 8, std::errc{}};
  } else if (v.starts_with("PATCH ")) {
    method = http_method::PATCH;
    return {first + 6, std::errc{}};
  } else if (v.starts_with("CONNECT ")) {
    method = http_method::CONNECT;
    return {first + 8, std::errc{}};
  } else if (v.starts_with("TRACE ")) {
    method = http_method::TRACE;
    return {first + 6, std::errc{}};
  } else {
    return {first, std::errc::invalid_argument}; // Unknown HTTP method
  }
}

/**
 * \brief Encapsulates an HTTP request.
 */
class http_request {
public:
  /**
   * \brief Construct an empty request.
   */
  http_request() : http_request(std::pmr::get_default_resource()) {}

  /**
   * \brief Constuct an empty request.
   *
   * \param upstream Upstream memory resource used to allocate the request.
   * \param initialSize Initial size for the http_request buffer.
   */
  explicit http_request(std::pmr::memory_resource *upstream,
                        std::size_t initialSize = 8192)
      : buffer_(initialSize, upstream) {}

  /**
   * \brief Reads in a new HTTP request from the specified input stream.
   *
   * \param[out] out Variable that receives the parsed request.
   * \param[inout] stream Stream to parse the request from.
   * \return true if the request could be parsed.
   */
  static bool accept(http_request &out, std::istream &stream);

  /**
   * \brief HTTP method of the request.
   */
  enum http_method http_method() const noexcept { return httpMethod_; }

  /**
   * \brief Gets the HTTP version as a version integer.
   * \return A 3-digit decimal integer where the hundreds place represents the
   * major version and the tens place the minor version.
   */
  int http_version() const noexcept { return httpVersion_; }

  /**
   * \brief Gets the resource URI referenced by the request.
   */
  const std::pmr::vector<std::pmr::string> &uri() const noexcept {
    return requestUri_;
  }

  /**
   * \brief Gets the request body.
   * \group cppws::http_request::body
   * \{
   */
  const std::pmr::vector<std::byte> &body() const & noexcept { return data_; }
  std::pmr::vector<std::byte> &&body() && noexcept { return std::move(data_); }
  std::string_view body_text() const noexcept {
    return std::string_view(reinterpret_cast<const char *>(data_.data()),
                            data_.size());
  }
  /** \} */

  /**
   * \brief Extracts an HTTP header from the request.
   *
   * \param name Name of the header to extract.
   * \return Pointer to the header value, or nullptr if it was not present.
   * \group cppws::http_request::http_header
   * \{
   */
  const std::pmr::string *http_header(std::string_view name) const noexcept {
    http_request_header stdh;
    if (from_chars(name.data(), name.data() + name.length(), stdh).ec ==
        std::errc{})
      return http_header(stdh);
    auto it = headers_.find(name);
    return it == headers_.end() ? nullptr : &it->second;
  }
  std::pmr::string *http_header(std::string_view name) noexcept {
    http_request_header stdh;
    if (from_chars(name.data(), name.data() + name.length(), stdh).ec ==
        std::errc{})
      return http_header(stdh);
    auto it = headers_.find(name);
    return it == headers_.end() ? nullptr : &it->second;
  }
  const std::pmr::string *http_header(http_request_header name) const noexcept {
    auto it = standardHeaders_.find(name);
    return it == standardHeaders_.end() ? nullptr : &it->second;
  }
  std::pmr::string *http_header(http_request_header name) noexcept {
    auto it = standardHeaders_.find(name);
    return it == standardHeaders_.end() ? nullptr : &it->second;
  }
  /** \} */

private:
  std::pmr::monotonic_buffer_resource buffer_;

  enum http_method httpMethod_ = http_method::GET;
  int httpVersion_ = 110;

  std::pmr::vector<std::pmr::string> requestUri_{&buffer_};

  std::pmr::vector<std::pmr::string> headerNames_{&buffer_};
  std::pmr::unordered_map<std::string_view, std::pmr::string> headers_{
      &buffer_};
  std::pmr::unordered_map<http_request_header, std::pmr::string>
      standardHeaders_{&buffer_};

  std::pmr::vector<std::byte> data_{&buffer_};
};

} // namespace cppws
