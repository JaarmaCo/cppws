# Suggested API for process based request managers

If each request will be managed by each process, we can easily abstract the endpoint manager as the
program entry point of the request manager.

If we are to continue with the idea of modelling the framework after the Java Rest framework, we can
do something "_reflective-esc_" for specifying these endpoints.

The user would provide a function that serves as the endpoint manager. This function would accept an
arbitrary number of arguments, where the arguments are processed by the framework and injected through
an automatic mechanism.

Given that C++ is a naturally _static_ language, it makes sense to implement this through either templates
or some pre-compilation tooling.

## Suggested usage

```c++
#include <format>
#include <iostream>

#include <nlohmann/json.hpp>

#include <cppws/endpoint.hpp>

namespace http = cppws::http;

WS_BINDING("userdb")
extern cppws::database database;

WS_BINDING("tgenerator")
extern cppws::token_generator tgen;

void WS_POST("/api/users/login") login(const nlohmann::json &args) {

  auto password = database.build_query("SELECT password FROM users WHERE username={}", args["username"])
    .find_one();

  if (cppws::cryptographic_hash(args["password"]) != password) {
    std::cout << http::ACCESS_DENIED << http::body("Invalid password");
    return;
  }

  std::cout << http::OK << http::body(
    std::format("Token: {}", tgen.generate(args["username"])));
}
```

