
set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

include(GoogleTest)

add_executable(url_test url_test.cpp)

target_link_libraries(url_test
  PRIVATE
    cppws
    GTest::gtest_main)

gtest_discover_tests(url_test)

