cmake_minimum_required(VERSION 3.8)

set(TESTCF TRUE)

project("CubicFramework")

add_subdirectory(src)

if(TESTCF)
	add_subdirectory(test)
endif()