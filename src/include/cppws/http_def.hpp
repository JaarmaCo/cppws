#pragma once

#include <cctype>
#include <charconv>
#include <cstring>
#include <string_view>
#include <system_error>

namespace cppws {

enum class http_content_type {
  TextPlain,
  TextHtml,
  TextCss,
  TextJavascript,
  ApplicationJson,
  ApplicationXml,
  ApplicationOctetStream,
  ApplicationPdf,
  ApplicationZip,
  ApplicationGzip,
  ImagePng,
  ImageJpeg,
  ImageGif,
  ImageSvgXml,
  AudioMpeg,
  VideoMp4,
  MultipartFormData,
  MultipartMixed,
  Unknown
};

inline std::string_view to_string(http_content_type ct) {
  switch (ct) {
  case http_content_type::TextPlain:
    return "text/plain";
  case http_content_type::TextHtml:
    return "text/html";
  case http_content_type::TextCss:
    return "text/css";
  case http_content_type::TextJavascript:
    return "text/javascript";
  case http_content_type::ApplicationJson:
    return "application/json";
  case http_content_type::ApplicationXml:
    return "application/xml";
  case http_content_type::ApplicationOctetStream:
    return "application/octet-stream";
  case http_content_type::ApplicationPdf:
    return "application/pdf";
  case http_content_type::ApplicationZip:
    return "application/zip";
  case http_content_type::ApplicationGzip:
    return "application/gzip";
  case http_content_type::ImagePng:
    return "image/png";
  case http_content_type::ImageJpeg:
    return "image/jpeg";
  case http_content_type::ImageGif:
    return "image/gif";
  case http_content_type::ImageSvgXml:
    return "image/svg+xml";
  case http_content_type::AudioMpeg:
    return "audio/mpeg";
  case http_content_type::VideoMp4:
    return "video/mp4";
  case http_content_type::MultipartFormData:
    return "multipart/form-data";
  case http_content_type::MultipartMixed:
    return "multipart/mixed";
  default:
    return "unknown";
  }
}

inline bool iequals_n(const char *a, const char *b, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    if (std::tolower(static_cast<unsigned char>(a[i])) !=
        std::tolower(static_cast<unsigned char>(b[i])))
      return false;
  }
  return true;
}

inline bool iequals_sv(const char *first, const char *last,
                       std::string_view sv) {
  size_t len = last - first;
  if (len != sv.size())
    return false;
  return iequals_n(first, sv.data(), len);
}

inline std::from_chars_result from_chars(const char *first, const char *last,
                                         http_content_type &value) {
  // We'll try matching known content-types, case-insensitive, full match only
  // If matched, return {last, std::errc()} indicating success
  // Else return {first, std::errc::invalid_argument}

  struct http_content_type_entry {
    http_content_type type;
    std::string_view str;
  };

  constexpr http_content_type_entry table[] = {
      {http_content_type::TextPlain, "text/plain"},
      {http_content_type::TextHtml, "text/html"},
      {http_content_type::TextCss, "text/css"},
      {http_content_type::TextJavascript, "text/javascript"},
      {http_content_type::ApplicationJson, "application/json"},
      {http_content_type::ApplicationXml, "application/xml"},
      {http_content_type::ApplicationOctetStream, "application/octet-stream"},
      {http_content_type::ApplicationPdf, "application/pdf"},
      {http_content_type::ApplicationZip, "application/zip"},
      {http_content_type::ApplicationGzip, "application/gzip"},
      {http_content_type::ImagePng, "image/png"},
      {http_content_type::ImageJpeg, "image/jpeg"},
      {http_content_type::ImageGif, "image/gif"},
      {http_content_type::ImageSvgXml, "image/svg+xml"},
      {http_content_type::AudioMpeg, "audio/mpeg"},
      {http_content_type::VideoMp4, "video/mp4"},
      {http_content_type::MultipartFormData, "multipart/form-data"},
      {http_content_type::MultipartMixed, "multipart/mixed"},
  };

  for (const auto &entry : table) {
    if (iequals_sv(first, last, entry.str)) {
      value = entry.type;
      return {last, std::errc()}; // success
    }
  }

  // No match found
  return {first, std::errc::invalid_argument};
}

} // namespace cppws
