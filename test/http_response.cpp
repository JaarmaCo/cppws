#include <iostream>

#include <cppws/http_response.hpp>

int main() {
  using namespace cppws;

  std::cout << http::OK << http::body("Success!");
}
