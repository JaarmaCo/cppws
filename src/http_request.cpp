#include <cppws/http_request.hpp>
#include <ranges>

static void read_http_line(std::pmr::string &out, std::istream &stream) {
  out.clear();
  int pc = 0;
  int c = 0;
  while (stream && !(c == '\n' && pc == '\r')) {
    pc = c;
    c = stream.get();
    out.append(1, c);
  }
  if (c == '\n' && pc == '\r') {
    out.pop_back();
    out.pop_back();
  }
}

bool cppws::http_request::accept(http_request &out, std::istream &stream) {

  std::pmr::string line{&out.buffer_};

  // Get request line.
  read_http_line(line, stream);

  auto [str, res] =
      from_chars(line.data(), line.data() + line.length(), out.httpMethod_);
  if (res != std::errc{})
    return false;

  const char *save = str;
  while (str != line.data() + line.length() && *str != ' ')
    ++str;

  out.requestUri_ = std::pmr::vector<std::pmr::string>{&out.buffer_};
  for (auto rng : std::string_view(save, str) | std::views::split('/')) {
    if (rng.empty())
      continue;
    out.requestUri_.push_back(
        std::pmr::string(rng.begin(), rng.end(), &out.buffer_));
  }

  std::string_view rest{str, line.data() + line.length()};
  if (!rest.starts_with(" HTTP/"))
    return false;

  rest = rest.substr(6);
  if (!std::isdigit(rest.front()))
    return false;

  out.httpVersion_ = 100 * (rest.front() - '0');
  rest = rest.substr(1);

  if (rest.starts_with('.')) {
    rest = rest.substr(1);
    if (!std::isdigit(rest.front()))
      return false;
    out.httpVersion_ += 10 * (rest.front() - '0');
    rest = rest.substr(1);
  }

  if (!rest.empty())
    return false;

  // Get the HTTP headers
  //
  out.headerNames_.clear();
  out.headers_.clear();
  for (;;) {
    read_http_line(line, stream);
    if (line.empty())
      break;

    std::size_t delim = line.find(": ");
    if (delim == std::pmr::string::npos)
      return false;

    http_request_header hdr;
    if (from_chars(line.data(), line.data() + delim, hdr).ec == std::errc{}) {
      out.standardHeaders_.insert({hdr, line.substr(delim + 2)});
    } else {
      out.headerNames_.push_back(line.substr(0, delim));
      out.headers_.insert(
          {std::string_view(out.headerNames_.back()), line.substr(delim + 2)});
    }
  }

  // Get contents
  //
  out.data_.clear();
  auto clen = out.standardHeaders_.find(http_request_header::ContentLength);
  if (clen != out.standardHeaders_.end()) {
    std::size_t length;
    if (std::from_chars(clen->second.data(),
                        clen->second.data() + clen->second.length(), length)
            .ec != std::errc{})
      return false;
    out.data_.resize(length);
    stream.read(reinterpret_cast<char *>(out.data_.data()), length);
  }
  return true;
}
