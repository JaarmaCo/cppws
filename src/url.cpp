#include <cppws/url.hpp>
#include <cstring>

cppws::url::url(std::string &&str) : std::string(std::move(str)) {
  validate(data(), length());
}

cppws::url::url(const std::string &s) : std::string(s) {
  validate(data(), length());
}

cppws::url::url(const char *str) : url(str, std::strlen(str)) {}

cppws::url::url(const char *str, std::size_t len) : std::string(str, len) {
  validate(data(), length());
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
