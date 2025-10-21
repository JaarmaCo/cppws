#include <gtest/gtest.h>

#include <cppws/url.hpp>

TEST(cppws_test, url) {

  cppws::url url;

  ASSERT_NO_THROW(url = cppws::url("https://git.cppws.com/raw/src/include"));

  ASSERT_EQ(url.scheme(), "https");
  ASSERT_EQ(url.authority().domain, "git.cppws.com");
  ASSERT_EQ(url.path_str(), "raw/src/include");

  ASSERT_EQ((url / "cppws/url.hpp").path_str(),
            "raw/src/include/cppws/url.hpp");
}
