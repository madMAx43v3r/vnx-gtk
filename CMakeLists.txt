cmake_minimum_required(VERSION 2.6)
project(vnx-gtk)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -pthread -Wall -Wno-unused-variable -Wno-sign-compare")

file(GLOB GENERATED_SOURCE_FILES generated/src/*.cpp)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK3 REQUIRED gtkmm-3.0)

add_library(vnx_gtk
	${GENERATED_SOURCE_FILES}
)

target_include_directories(vnx_gtk
	PUBLIC include/ generated/include/ ${GTK3_INCLUDE_DIRS}
)

target_link_libraries(vnx_gtk vnx_base ${GTK3_LIBRARIES})

add_executable(vnxgtkplayer src/vnxgtkplayer.cpp)

target_link_libraries(vnxgtkplayer vnx_gtk)

install(TARGETS vnxgtkplayer DESTINATION bin)
