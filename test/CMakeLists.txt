
set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

include(GoogleTest)

add_executable(http_response http_response.cpp)
target_link_libraries(http_response
  PRIVATE
    cppws)

add_executable(url_test url_test.cpp)
target_link_libraries(url_test
  PRIVATE
    cppws
    GTest::gtest_main)

add_executable(http_request_test http_request_test.cpp)
target_link_libraries(http_request_test
  PRIVATE
    cppws
    GTest::gtest_main)

gtest_discover_tests(url_test http_request_test)

