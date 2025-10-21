#pragma once

#include <cctype>
#include <charconv>
#include <iostream>
#include <string_view>
#include <system_error>

#include <cppws/http_def.hpp>
#include <cppws/http_request.hpp>

namespace cppws {

enum class http_response_header {
  AcceptRanges,
  Age,
  CacheControl,
  ContentDisposition,
  ContentEncoding,
  ContentLanguage,
  ContentLength,
  ContentLocation,
  ContentMD5,
  ContentRange,
  ContentType,
  Date,
  ETag,
  Expires,
  LastModified,
  Link,
  Location,
  Pragma,
  ProxyAuthenticate,
  Refresh,
  RetryAfter,
  Server,
  SetCookie,
  StrictTransportSecurity,
  Trailer,
  TransferEncoding,
  Vary,
  Via,
  Warning,
  WWWAuthenticate,
  // Add more if needed

  Unknown
};

constexpr std::string_view to_string(http_response_header header) noexcept {
  switch (header) {
  case http_response_header::AcceptRanges:
    return "Accept-Ranges";
  case http_response_header::Age:
    return "Age";
  case http_response_header::CacheControl:
    return "Cache-Control";
  case http_response_header::ContentDisposition:
    return "Content-Disposition";
  case http_response_header::ContentEncoding:
    return "Content-Encoding";
  case http_response_header::ContentLanguage:
    return "Content-Language";
  case http_response_header::ContentLength:
    return "Content-Length";
  case http_response_header::ContentLocation:
    return "Content-Location";
  case http_response_header::ContentMD5:
    return "Content-MD5";
  case http_response_header::ContentRange:
    return "Content-Range";
  case http_response_header::ContentType:
    return "Content-Type";
  case http_response_header::Date:
    return "Date";
  case http_response_header::ETag:
    return "ETag";
  case http_response_header::Expires:
    return "Expires";
  case http_response_header::LastModified:
    return "Last-Modified";
  case http_response_header::Link:
    return "Link";
  case http_response_header::Location:
    return "Location";
  case http_response_header::Pragma:
    return "Pragma";
  case http_response_header::ProxyAuthenticate:
    return "Proxy-Authenticate";
  case http_response_header::Refresh:
    return "Refresh";
  case http_response_header::RetryAfter:
    return "Retry-After";
  case http_response_header::Server:
    return "Server";
  case http_response_header::SetCookie:
    return "Set-Cookie";
  case http_response_header::StrictTransportSecurity:
    return "Strict-Transport-Security";
  case http_response_header::Trailer:
    return "Trailer";
  case http_response_header::TransferEncoding:
    return "Transfer-Encoding";
  case http_response_header::Vary:
    return "Vary";
  case http_response_header::Via:
    return "Via";
  case http_response_header::Warning:
    return "Warning";
  case http_response_header::WWWAuthenticate:
    return "WWW-Authenticate";
  default:
    return "Unknown";
  }
}

constexpr std::from_chars_result from_chars(const char *first, const char *last,
                                            http_response_header &value,
                                            int = 10) noexcept {
  std::string_view sv(first, static_cast<size_t>(last - first));

  if (iequals(sv, "Accept-Ranges")) {
    value = http_response_header::AcceptRanges;
    return {last, {}};
  }
  if (iequals(sv, "Age")) {
    value = http_response_header::Age;
    return {last, {}};
  }
  if (iequals(sv, "Cache-Control")) {
    value = http_response_header::CacheControl;
    return {last, {}};
  }
  if (iequals(sv, "Content-Disposition")) {
    value = http_response_header::ContentDisposition;
    return {last, {}};
  }
  if (iequals(sv, "Content-Encoding")) {
    value = http_response_header::ContentEncoding;
    return {last, {}};
  }
  if (iequals(sv, "Content-Language")) {
    value = http_response_header::ContentLanguage;
    return {last, {}};
  }
  if (iequals(sv, "Content-Length")) {
    value = http_response_header::ContentLength;
    return {last, {}};
  }
  if (iequals(sv, "Content-Location")) {
    value = http_response_header::ContentLocation;
    return {last, {}};
  }
  if (iequals(sv, "Content-MD5")) {
    value = http_response_header::ContentMD5;
    return {last, {}};
  }
  if (iequals(sv, "Content-Range")) {
    value = http_response_header::ContentRange;
    return {last, {}};
  }
  if (iequals(sv, "Content-Type")) {
    value = http_response_header::ContentType;
    return {last, {}};
  }
  if (iequals(sv, "Date")) {
    value = http_response_header::Date;
    return {last, {}};
  }
  if (iequals(sv, "ETag")) {
    value = http_response_header::ETag;
    return {last, {}};
  }
  if (iequals(sv, "Expires")) {
    value = http_response_header::Expires;
    return {last, {}};
  }
  if (iequals(sv, "Last-Modified")) {
    value = http_response_header::LastModified;
    return {last, {}};
  }
  if (iequals(sv, "Link")) {
    value = http_response_header::Link;
    return {last, {}};
  }
  if (iequals(sv, "Location")) {
    value = http_response_header::Location;
    return {last, {}};
  }
  if (iequals(sv, "Pragma")) {
    value = http_response_header::Pragma;
    return {last, {}};
  }
  if (iequals(sv, "Proxy-Authenticate")) {
    value = http_response_header::ProxyAuthenticate;
    return {last, {}};
  }
  if (iequals(sv, "Refresh")) {
    value = http_response_header::Refresh;
    return {last, {}};
  }
  if (iequals(sv, "Retry-After")) {
    value = http_response_header::RetryAfter;
    return {last, {}};
  }
  if (iequals(sv, "Server")) {
    value = http_response_header::Server;
    return {last, {}};
  }
  if (iequals(sv, "Set-Cookie")) {
    value = http_response_header::SetCookie;
    return {last, {}};
  }
  if (iequals(sv, "Strict-Transport-Security")) {
    value = http_response_header::StrictTransportSecurity;
    return {last, {}};
  }
  if (iequals(sv, "Trailer")) {
    value = http_response_header::Trailer;
    return {last, {}};
  }
  if (iequals(sv, "Transfer-Encoding")) {
    value = http_response_header::TransferEncoding;
    return {last, {}};
  }
  if (iequals(sv, "Vary")) {
    value = http_response_header::Vary;
    return {last, {}};
  }
  if (iequals(sv, "Via")) {
    value = http_response_header::Via;
    return {last, {}};
  }
  if (iequals(sv, "Warning")) {
    value = http_response_header::Warning;
    return {last, {}};
  }
  if (iequals(sv, "WWW-Authenticate")) {
    value = http_response_header::WWWAuthenticate;
    return {last, {}};
  }

  return {first, std::errc::invalid_argument};
}

struct http_resonse_line {
  int http_version = 110;
  int status_code = 200;
  std::string_view reason = "";
};

struct http_header_line {
  std::string_view header;
  std::string_view value;
};

struct http_body {
  std::string_view content_type;
  std::span<const std::byte> content;
};

constexpr std::ostream &operator<<(std::ostream &stream,
                                   const http_resonse_line &line) {

  stream << "HTTP/" << (line.http_version / 100);
  if (line.http_version % 100 != 0)
    stream << '.' << ((line.http_version % 100) / 10);

  stream << ' ' << line.status_code;
  stream << ' ' << line.reason << "\r\n";
  return stream;
}

constexpr std::ostream &operator<<(std::ostream &stream,
                                   const http_header_line &line) {
  return stream << line.header << ": " << line.value << "\r\n";
}

constexpr std::ostream &operator<<(std::ostream &stream,
                                   const http_body &body) {
  std::string len = std::to_string(body.content.size());
  stream << http_header_line("Content-Length", len)
         << http_header_line("Content-Type", body.content_type) << "\r\n";
  stream.write(reinterpret_cast<const char *>(body.content.data()),
               body.content.size());
  return stream;
}

namespace http {

constexpr http_resonse_line OK = {
    .http_version = 110, .status_code = 200, .reason = "OK"};

constexpr http_resonse_line CREATED = {
    .http_version = 110, .status_code = 201, .reason = "Created"};

constexpr http_resonse_line ACCEPTED = {
    .http_version = 110, .status_code = 202, .reason = "Accepted"};

constexpr http_resonse_line NO_CONTENT = {
    .http_version = 110, .status_code = 204, .reason = "No Content"};

constexpr http_resonse_line MOVED_PERMANENTLY = {
    .http_version = 110, .status_code = 301, .reason = "Moved Permanently"};

constexpr http_resonse_line FOUND = {
    .http_version = 110, .status_code = 302, .reason = "Found"};

constexpr http_resonse_line NOT_MODIFIED = {
    .http_version = 110, .status_code = 304, .reason = "Not Modified"};

constexpr http_resonse_line BAD_REQUEST = {
    .http_version = 110, .status_code = 400, .reason = "Bad Request"};

constexpr http_resonse_line UNAUTHORIZED = {
    .http_version = 110, .status_code = 401, .reason = "Unauthorized"};

constexpr http_resonse_line FORBIDDEN = {
    .http_version = 110, .status_code = 403, .reason = "Forbidden"};

constexpr http_resonse_line NOT_FOUND = {
    .http_version = 110, .status_code = 404, .reason = "Not Found"};

constexpr http_resonse_line METHOD_NOT_ALLOWED = {
    .http_version = 110, .status_code = 405, .reason = "Method Not Allowed"};

constexpr http_resonse_line INTERNAL_SERVER_ERROR = {
    .http_version = 110, .status_code = 500, .reason = "Internal Server Error"};

constexpr http_resonse_line NOT_IMPLEMENTED = {
    .http_version = 110, .status_code = 501, .reason = "Not Implemented"};

constexpr http_resonse_line BAD_GATEWAY = {
    .http_version = 110, .status_code = 502, .reason = "Bad Gateway"};

constexpr http_resonse_line SERVICE_UNAVAILABLE = {
    .http_version = 110, .status_code = 503, .reason = "Service Unavailable"};

constexpr http_header_line header(std::string_view name,
                                  std::string_view value) {
  return http_header_line(name, value);
}

constexpr http_header_line header(http_response_header name,
                                  std::string_view value) {
  return header(to_string(name), value);
}

constexpr http_body
body(std::span<const std::byte> content,
     http_content_type type = http_content_type::ApplicationOctetStream) {
  return http_body(to_string(type), content);
}

constexpr http_body
body(std::string_view content,
     http_content_type type = http_content_type::TextPlain) {
  return http_body(to_string(type), std::as_bytes(std::span(content)));
}

} // namespace http

} // namespace cppws
