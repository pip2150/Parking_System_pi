cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 11)
project(Parking_System)

find_package( OpenCV REQUIRED )
find_package(OpenMP)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

set(OPENCV_FILES opencv/plate.cpp opencv/svm.cpp opencv/ocr.cpp opencv/tools.cpp  )
set(NETWORK_FILES network/http.cpp network/socket.cpp network/api.cpp network/serverAPI.cpp network/proxyAPI.cpp )
set(WIRINGPI_FILES wiringpi/lcd.cpp wiringpi/barricade.cpp )

add_executable(test_server network/test_server.cpp network/proxyAPI.cpp network/serverAPI.cpp network/api.cpp network/socket.cpp network/list.cpp network/http.cpp )
add_executable(http_test network/http_test.cpp network/serverAPI.cpp network/api.cpp network/socket.cpp network/http.cpp )
add_executable(main main.cpp process.cpp ${OPENCV_FILES} ${NETWORK_FILES} ${WIRINGPI_FILES} )

target_link_libraries(main ${OpenCV_LIBS} )
target_link_libraries(main wiringPi wiringPiDev)
