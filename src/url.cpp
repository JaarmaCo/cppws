#include <cctype>
#include <cppws/url.hpp>
#include <cstring>

std::string cppws::url::encode(const char *str, std::size_t length) {

  std::string out;

  const auto cconv = [&out](char ch) {
    int lo = ch & 0x0F;
    int hi = (ch & 0xF0) >> 4;

    out += '%';
    out += (hi < 10 ? hi + '0' : (hi - 10) + 'A');
    out += (lo < 10 ? lo + '0' : (lo - 10) + 'A');
  };

  for (std::size_t i = 0; i < length; ++i) {
    switch (str[i]) {
    case '/':
    case '%':
    case '?':
    case '&':
    case '#':
    case '<':
    case '>':
    case '+':
    case '{':
    case '}':
    case '|':
    case '\\':
    case '^':
    case '~':
    case '[':
    case ']':
    case '`':
    case ';':
    case ':':
    case '@':
    case '=':
    case '$':
      cconv(str[i]);
      break;
    default:
      if (((str[i] & 0x7F) == str[i]) && !std::isspace(str[i]) &&
          !std::iscntrl(str[i]))
        out += str[i];
      else
        cconv(str[i]);
      break;
    }
  }
  return out;
}

std::string cppws::url::decode(const char *str, std::size_t length) {
  const auto toc = [](int c) -> int {
    return (c >= '0' && c <= '9'
                ? c - '0'
                : (c >= 'a' && c <= 'f' ? c - 'a' + 10 : c - 'A' + 10)) &
           0xF;
  };
  std::string res;
  for (std::size_t i = 0; i < length; ++i) {
    if (str[i] == '%') {
      if (i + 2 >= length)
        throw bad_url(i, "2 digits required after '%'.");

      char lo = str[i + 1];
      char hi = str[i + 2];

      if (!std::isxdigit(lo))
        throw bad_url(i + 1, "Expected a hexadecimal digit.");

      if (!std::isxdigit(hi))
        throw bad_url(i + 2, "Expected a hexadecimal digit.");

      str += static_cast<char>(toc(lo) | (toc(hi) << 4));

      i = i + 1;
      continue;
    }
    res += str[i];
  }
  return res;
}

cppws::url::url(std::string &&str)
    : str_(std::make_shared<std::string>(std::move(str))) {
  full_ = *str_;
  decompose();
}

cppws::url::url(const std::string &str)
    : str_(std::make_shared<std::string>(str)) {
  full_ = *str_;
  decompose();
}

cppws::url::url(const char *str) : url(str, std::strlen(str)) {}

cppws::url::url(const char *str, std::size_t length)
    : str_(std::make_shared<std::string>(str, length)) {
  full_ = *str_;
  decompose();
}

void cppws::url::decompose() {

  std::string_view str = full_;
  if (str.empty())
    return;

  str = decompose_scheme(str);
  str = decompose_authority(str);
  str = decompose_path(str);

  if (str.empty())
    return;

  str = decompose_query(str);

  if (str.starts_with('#'))
    fragment_ = str.substr(1);
}

std::string_view cppws::url::decompose_scheme(std::string_view contents) {

  const char *ptr = contents.data();
  const char *end = contents.data() + contents.length();

  while (ptr != end &&
         (std::isalnum(*ptr) || *ptr == '-' || *ptr == '+' || *ptr == '.'))
    ++ptr;

  if (ptr == end || *ptr != ':')
    return contents;

  scheme_ = {contents.data(), ptr};
  return {ptr, end};
}

std::string_view cppws::url::decompose_authority(std::string_view contents) {

  const char *ptr = contents.data();
  const char *end = contents.data() + contents.length();

  // No authority
  //
  if (!contents.starts_with("://"))
    return contents;

  ptr += 3;

  // Seek for the position of the path.
  //
  const char *seek = ptr;
  while (seek != end && *seek != '/')
    ++seek;

  if (*seek != '/') {
    throw bad_url(seek - full_.data(), "'//' without an authority segment.");
  }

  std::string_view auth = {ptr, seek};
  auth_.full = auth;
  ptr = auth.data() + auth.length() + 1; // Save start of path to ptr

  std::size_t pos = auth.find('@');
  if (pos != std::string_view::npos) { // ...://username:[password]@...

    std::string_view user = auth.substr(0, pos);
    auth = auth.substr(pos + 1);

    pos = user.find(':');
    if (pos == std::string_view::npos)
      throw bad_url(user.data() - full_.data(),
                    "Missing password section of user info in auth segment.");

    auth_.username = user.substr(0, pos);
    auth_.password = user.substr(pos + 1);
  }

  // ...domain[:port]
  //
  pos = auth.find(':');
  if (pos != std::string_view::npos) {
    std::string_view port = auth.substr(pos + 1);
    if (std::from_chars(port.data(), port.data() + port.length(), auth_.port)
            .ec != std::errc{}) {
      throw bad_url(port.data() - full_.data(),
                    "Port number cannot be parsed as a short.");
    }
    auth = auth.substr(0, pos);
  }
  auth_.domain = auth;
  return {ptr, end};
}

std::string_view cppws::url::decompose_path(std::string_view contents) {

  const char *prev = contents.data();
  const char *ptr = contents.data();
  const char *end = contents.data() + contents.length();

  // .../path[/?query][#segment]
  //
  while (ptr != end) {

    if (*ptr == '?' || *ptr == '#')
      break;

    if (*ptr == '/') {
      path_.push_back({prev, ptr});
      prev = ptr + 1;
    }
    ++ptr;
  }

  if (ptr == end && ptr - prev > 0)
    path_.push_back({prev, ptr});

  if (!path_.empty()) {
    pathStr_ = {path_.front().data(), &*path_.back().end()};
  }

  return {ptr, end};
}

std::string_view cppws::url::decompose_query(std::string_view contents) {

  if (!contents.starts_with('?'))
    return contents;

  const char *prev = contents.data() + 1;
  const char *ptr = contents.data() + 1;
  const char *end = contents.data() + contents.length();

  std::string_view key;
  while (ptr != end) {

    if (*ptr == '#')
      break;

    if (*ptr == '&' || *ptr == ';') {
      if (key.empty())
        throw bad_url(ptr - full_.data(),
                      std::format("Missing key to query parameter {}.",
                                  queryParams_.size()));

      queryParams_.push_back(key);
      query_.insert({key, {prev, ptr}});

      prev = ptr + 1;
      key = {};
    } else if (*ptr == '=') {
      key = {prev, ptr};
      prev = ptr + 1;
    }
    ++ptr;
  }

  if (key.empty())
    throw bad_url(prev - full_.data(), "Expected '=' after query key.");

  queryParams_.push_back(key);
  query_.insert({key, {prev, ptr}});

  return {ptr, end};
}

cppws::url cppws::url::parent() const {

  if (path_.empty())
    return *this;

  url c = *this;
  c.path_.pop_back();
  c.pathStr_ = {c.path_.front().data(), &*c.path_.back().end()};
  return c;
}

cppws::url cppws::url::operator/(std::string_view str) const {

  if (str.starts_with('/'))
    str = str.substr(1);

  std::string res{full_};
  std::size_t pathEnd = &*pathStr_.end() - full_.data();

  res.insert(pathEnd, '/' + std::string(str));
  return url(std::move(res));
}
