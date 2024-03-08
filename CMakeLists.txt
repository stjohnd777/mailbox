cmake_minimum_required(VERSION 3.0)
project(mailbox)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV CONFIG REQUIRED )
find_package(Boost REQUIRED  )
find_package(GTest REQUIRED  )
find_package(nlohmann_json REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
message( STATUS "opencv include: ${OpenCV_INCLUDE_DIRS}" )
message( STATUS "opencv libs dir: ${OpenCV_LIBRARY_DIRS} ")
message( STATUS "opencv libs: ${OpenCV_LIBS} ")

include_directories(/usr/include)
link_directories(/usr/lib)

#include_directories( ~/vcpkg/installed/x64-linux/include)
#include_directories( ~/vcpkg/installed/x64-linux/include/opencv4)
#link_directories(~/vcpkg/installed/x64-linux/lib)

include_directories(include)

add_library(mailbox
        include/macros.h
        include/utils.h
        include/MonitorDirectory.h
        src/MonitorDirectory.cpp
        include/MonitorBehavior.h
        include/MonitorSharedMem.h)
target_link_libraries(mailbox PRIVATE ${OpenCV_LIBS} ${Boost_LIBRARIES}  nlohmann_json::nlohmann_json )

add_executable(sobel_cv main/main_sobel.cpp)
target_link_libraries(sobel_cv PRIVATE   mailbox ${OpenCV_LIBS} ${Boost_LIBRARIES})

add_executable(main_mailbox_sobel main/main_mailbox_sobel.cpp )
target_link_libraries(main_mailbox_sobel PRIVATE mailbox ${OpenCV_LIBS} ${Boost_LIBRARIES})

add_executable(main_shmem_sobel main/main_shmem_sobel.cpp)
target_link_libraries(main_shmem_sobel PRIVATE mailbox ${OpenCV_LIBS} ${Boost_LIBRARIES})

#add_executable(main_shmem_boost main/main_shmem_boost.cpp)
#target_link_libraries(main_shmem_boost PRIVATE mailbox ${OpenCV_LIBS} ${Boost_LIBRARIES})

#include(FetchContent)
#FetchContent_Declare(
#        googletest
#        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
#)
#FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        hello_test
        test/test.cc
        include/MonitorDirectory.h
        include/utils.h
        include/image.h
        include/MonitorSharedMem.h
        main/main_shmem_sobel.cpp)
target_link_libraries(
        hello_test
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(hello_test)
