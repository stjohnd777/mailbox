cmake_minimum_required(VERSION 3.22)
project(mailbox)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED  )


add_executable(mailbox main.cpp)

target_link_libraries(mailbox PRIVATE ${OpenCV_LIBS} ${Boost_LIBRARIES})
