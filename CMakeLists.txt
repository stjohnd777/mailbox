cmake_minimum_required(VERSION 3.0)
project(mailbox)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED  )
find_package(GTest REQUIRED  )
find_package(nlohmann_json REQUIRED)


include_directories(include)


add_library(mailbox
        include/macros.h
        include/utils.h
        include/MonitorDirectory.h
        src/MonitorDirectory.cpp
        include/MonitorBehavior.h)
target_link_libraries(mailbox PRIVATE ${OpenCV_LIBS} ${Boost_LIBRARIES}  nlohmann_json::nlohmann_json )

add_executable(main  main.cpp )
target_link_libraries(main PRIVATE mailbox ${OpenCV_LIBS} ${Boost_LIBRARIES})

add_executable(sobel  sobel.cpp)
target_link_libraries(sobel PRIVATE mailbox ${OpenCV_LIBS} ${Boost_LIBRARIES})



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
        include/MonitorDirectory.h
        include/utils.h)
target_link_libraries(
        hello_test
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(hello_test)
