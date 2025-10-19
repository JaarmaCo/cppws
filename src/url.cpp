#include <cctype>
#include <cppws/url.hpp>
#include <cstring>
#include <regex>

bool cppws::url::validate(const char *str, std::size_t length) {
  static std::regex pattern = std::regex(
      R"rx([-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*))rx");
  return std::regex_match(str, str + length, pattern);
}

std::string cppws::url::encode(const char *str, std::size_t length) {

  std::string out;

  const auto cconv = [&out](char ch) {
    int lo = ch & 0x0F;
    int hi = ch & 0xF0;

    out += '%';
    out += (lo < 10 ? lo + '0' : lo + 'A');
    out += (hi < 10 ? hi + '0' : hi + 'A');
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

cppws::url::url(std::string &&str) : std::string(std::move(str)) {
  if (!validate(data(), length()))
    throw std::runtime_error("Bad URL format.");
}

cppws::url::url(const std::string &s) : std::string(s) {
  if (!validate(data(), length()))
    throw std::runtime_error("Bad URL format.");
}

cppws::url::url(const char *str) : url(str, std::strlen(str)) {}

cppws::url::url(const char *str, std::size_t len) : std::string(str, len) {
  if (!validate(data(), length()))
    throw std::runtime_error("Bad URL format.");
}

std::string_view cppws::url::protocol() const {

  std::size_t pos = find("://");
  if (pos == npos)
    return "";
  return {data(), data() + pos + 3};
}

std::string_view cppws::url::domain() const {

  std::size_t offset;
  std::string_view prot = protocol();
  if (prot.empty())
    offset = 0;
  else
    offset = prot.length();

  std::string_view dom = {data() + offset, data() + length()};
  offset = dom.find('/');
  if (offset != npos)
    dom = dom.substr(0, offset);
  return dom;
}

std::string_view cppws::url::path() const {
  std::size_t offset = 0;
  std::string_view dom = domain();
  if (!dom.empty())
    offset = dom.data() + dom.length() - data() + 1;
  return {data() + offset, data() + length()};
}

cppws::url cppws::url::parent() const {
  std::size_t pos = rfind('/');
  if (pos == npos)
    return url();
  return url(substr(0, pos));
}

cppws::url cppws::url::operator/(std::string_view suffix) const & {

  std::string str;

  if (ends_with('/') && suffix.starts_with('/')) {
    str.resize(length() + suffix.length() - 1);

    std::memcpy(str.data(), data(), length());
    std::memcpy(str.data() + length(), suffix.data() + 1, suffix.length() - 1);

  } else if (ends_with('/') || suffix.starts_with('/')) {
    str.resize(length() + suffix.length());

    std::memcpy(str.data(), data(), length());
    std::memcpy(str.data() + length(), suffix.data(), suffix.length());

  } else {
    str.resize(length() + suffix.length() + 1);

    std::memcpy(str.data(), data(), length());
    str.data()[length()] = '/';
    std::memcpy(str.data() + length() + 1, suffix.data(), suffix.length());
  }
  return url(std::move(str));
}

cppws::url &&cppws::url::operator/(std::string_view suffix) && {

  if (ends_with('/') && suffix.starts_with('/')) {
    resize(length() + suffix.length() - 1);

    std::memcpy(data() + length(), suffix.data() + 1, suffix.length() - 1);

  } else if (ends_with('/') || suffix.starts_with('/')) {
    resize(length() + suffix.length());

    std::memcpy(data() + length(), suffix.data(), suffix.length());

  } else {
    resize(length() + suffix.length() + 1);

    data()[length()] = '/';
    std::memcpy(data() + length() + 1, suffix.data(), suffix.length());
  }
  return std::move(*this);
}
