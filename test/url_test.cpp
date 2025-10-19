#include <gtest/gtest.h>

#include <cppws/url.hpp>

TEST(cppws_test, url) {

  cppws::url url;

  ASSERT_NO_THROW(
      url = cppws::url("https://git.cppws.com/raw/src/include/url.hpp"));

  ASSERT_EQ(url.protocol(), "https");
  ASSERT_EQ(url.domain(), "git.cppws.com");
  ASSERT_EQ(url.path(), "raw/src/include/url.hpp");

  ASSERT_EQ(url.parent(), cppws::url("https://git.cppws.com/raw/src/include"));
}
