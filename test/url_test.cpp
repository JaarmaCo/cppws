#include <gtest/gtest.h>

#include <cppws/url.hpp>

TEST(cppws_test, url) {

  using namespace cppws::url_literals;

  // clang-format off
  ASSERT_STRCASEEQ(cppws::url::encode("http://user:passw@website.org:1234/some/path/?var1=value1;var2=value2&var3=value3#fragment").c_str(), 
            "http%3A%2F%2Fuser%3Apassw%40website.org%3A1234%2Fsome%2Fpath%2F%3Fvar1%3Dvalue1%3Bvar2%3Dvalue2%26var3%3Dvalue3%23fragment");

  ASSERT_STRCASEEQ(cppws::url::decode("http%3A%2F%2Fuser%3Apassw%40website.org%3A1234%2Fsome%2Fpath%2F%3Fvar1%3Dvalue1%3Bvar2%3Dvalue2%26var3%3Dvalue3%23fragment").c_str(),
            "http://user:passw@website.org:1234/some/path/?var1=value1;var2=value2&var3=value3#fragment");

  // clang-format on

  cppws::url url;

  ASSERT_NO_THROW(url = cppws::url("https://git.cppws.com/raw/src/include"));

  ASSERT_EQ(url.scheme(), "https");
  ASSERT_EQ(url.authority().domain, "git.cppws.com");
  ASSERT_EQ(url.path_str(), "raw/src/include");

  ASSERT_EQ((url / "cppws/url.hpp").path_str(),
            "raw/src/include/cppws/url.hpp");

  ASSERT_EQ(url.parent().path_str(), "raw/src");

  url =
      "http://user:passw@website.org:1234/some/path/?var1=value1;var2=value2&var3=value3#fragment"_url;

  ASSERT_EQ(url.scheme(), "http");
  ASSERT_EQ(url.authority().username, "user");
  ASSERT_EQ(url.authority().password, "passw");
  ASSERT_EQ(url.authority().domain, "website.org");
  ASSERT_EQ(url.authority().port, 1234);
  ASSERT_EQ(url.path_str(), "some/path");
  ASSERT_EQ(url.query_params().size(), 3);
  ASSERT_EQ(url.query_params()[0], "var1");
  ASSERT_EQ(url.query_params()[1], "var2");
  ASSERT_EQ(url.query_params()[2], "var3");
  ASSERT_EQ(url.query("var1").value(), "value1");
  ASSERT_EQ(url.query("var2").value(), "value2");
  ASSERT_EQ(url.query("var3").value(), "value3");
  ASSERT_EQ(url.fragment(), "fragment");
}
