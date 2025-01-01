# CMakeList.txt : CMake project for WindowEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("WindowEngine")

set(LibSRC defWindow.h win32_Window.h win32_Window.c WindowEvent.h WindowEvent.c Image.h Image.c defImage.h RGBcolor.h win32_thread.h win32_thread.c)
set(MainSRC "main.cpp")

# Add source to this project's executable.
if(DIST STREQUAL "ON")
add_compile_definitions(DIST=${BUILD_TYPE})
add_executable (${PROJECT_NAME} WIN32 ${LibSRC} ${MainSRC})
else()
add_executable (${PROJECT_NAME} ${LibSRC} ${MainSRC})
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET WindowEngine PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
