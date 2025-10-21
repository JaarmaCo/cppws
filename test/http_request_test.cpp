#include <cppws/http_request.hpp>
#include <gtest/gtest.h>

TEST(cppws_test, http_request) {
  using namespace cppws;

  std::string_view payload = R"json({
    "username": "John",
    "email": "john@gmail.com",
    "password": "password123"
  })json";

  http_request request;
  std::stringstream ss{std::format(""
                                   "POST /users/login HTTP/1.1\r\n"
                                   "Content-Type: application/json\r\n"
                                   "Content-Length: {}\r\n"
                                   "\r\n"
                                   "{}",
                                   payload.length(), payload)};

  ASSERT_TRUE(http_request::accept(request, ss));

  ASSERT_EQ(request.http_method(), http_method::POST);
  ASSERT_EQ(request.http_version(), 110);

  const std::pmr::string *header;
  ASSERT_NE((header = request.http_header(http_request_header::ContentType)),
            nullptr);
  ASSERT_EQ(*header, "application/json");

  ASSERT_NE((header = request.http_header(http_request_header::ContentLength)),
            nullptr);
  ASSERT_EQ(std::string_view(*header), std::to_string(payload.length()));

  ASSERT_EQ(request.uri().size(), 2);
  ASSERT_EQ(request.uri()[0], "users");
  ASSERT_EQ(request.uri()[1], "login");
  ASSERT_EQ(request.body_text(), payload);

  ss = std::stringstream{"GET /users/login HTTP/1.1\r\n"
                         "Content-Type \r\n"};
  ASSERT_FALSE(http_request::accept(request, ss));
}
