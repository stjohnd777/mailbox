cmake_minimum_required(VERSION 3.0)
project(mailbox)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED  )
find_package(GTest REQUIRED  )


add_executable(mailbox main.cpp)

target_link_libraries(mailbox PRIVATE ${OpenCV_LIBS} ${Boost_LIBRARIES})


include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        hello_test
        test/test.cc
        src/MonitorDirectory.h)
target_link_libraries(
        hello_test
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(hello_test)
