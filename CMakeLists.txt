cmake_minimum_required(VERSION 3.0)
project(mailbox)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV CONFIG REQUIRED )
find_package(Boost REQUIRED  )
find_package(nlohmann_json REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
message( STATUS "opencv include: ${OpenCV_INCLUDE_DIRS}" )
message( STATUS "opencv libs dir: ${OpenCV_LIBRARY_DIRS} ")
message( STATUS "opencv libs: ${OpenCV_LIBS} ")


include_directories(src/include)

add_library( mailbox
    src/include/image.h
    src/include/macros.h
    src/include/MonitorBehavior.h
    src/include/MonitorDirectory.h
    src/include/MonitorSharedMem.h
    src/include/utils.h
    src/cpp/MonitorDirectory.cpp
)
target_link_libraries(mailbox
        PRIVATE
        ${OpenCV_LIBS}
        ${Boost_LIBRARIES}
        nlohmann_json::nlohmann_json )

add_executable(cv_sobel main/main_sobel.cpp
        _sandbox/image_utils.h)
target_link_libraries(cv_sobel PRIVATE mailbox ${OpenCV_LIBS} ${Boost_LIBRARIES})

add_executable(main_dir_mailbox_sobel main/main_dir_sobel.cpp)
target_link_libraries(main_dir_mailbox_sobel PRIVATE mailbox ${OpenCV_LIBS} ${Boost_LIBRARIES})

add_executable(main_shmem_sobel main/main_shmem_sobel.cpp)
target_link_libraries(main_shmem_sobel PRIVATE mailbox ${OpenCV_LIBS} ${Boost_LIBRARIES})


find_package(GTest REQUIRED  )

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        hello_test
        test/test.cc
)


target_link_libraries(
        hello_test
        GTest::gtest_main
        mailbox ${OpenCV_LIBS} ${Boost_LIBRARIES}
)

include(GoogleTest)
gtest_discover_tests(hello_test)
