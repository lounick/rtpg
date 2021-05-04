cmake_minimum_required(VERSION 2.8.8)

find_package(GTest QUIET)

if(NOT GTEST_FOUND)
  # If not found, download it

  # Download and unpack googletest at configure time
  configure_file(gtest/CMakeLists.txt.in gtest/CMakeLists.txt)
  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gtest )
  if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
  endif()
  execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gtest )
  if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
  endif()

  # Prevent overriding the parent project's compiler/linker
  # settings on Windows
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  # Add googletest directly to our build. This defines
  # the gtest and gtest_main targets.
  add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                   ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                   EXCLUDE_FROM_ALL)

  # The gtest/gtest_main targets carry header search path
  # dependencies automatically when using CMake 2.8.11 or
  # later. Otherwise we have to add them here ourselves.
  if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
  endif()
endif()

# small helper function
function(rtpg_add_gtest target)
  add_executable(${target} ${ARGN})
  add_dependencies(${target} gtest)
  target_link_libraries(${target} ${PROJECT_NAME} gtest)

  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # GCC is not strict enough by default, so enable most of the warnings.
    target_compile_options(${target} PRIVATE
      -Werror=all
      -Werror=extra
      -Wno-unknown-pragmas
      -Wno-sign-compare
      -Wno-unused-parameter
      -Wno-missing-field-initializers
      )
  endif()

  add_test(NAME ${target} COMMAND ${target})
endfunction()

include_directories(${GTEST_INCLUDE_DIRS})

rtpg_add_gtest(gtest_pg gtest_pg.cpp)

set(CXX_17_TEST_TARGETS gtest_pg)

# Set required C++17 flag
if(CMAKE_VERSION VERSION_LESS "3.1")
  set_target_properties(${CXX_17_TEST_TARGETS} PROPERTIES COMPILE_FLAGS "-std=c++17")
else()
  set_property(TARGET ${CXX_17_TEST_TARGETS} PROPERTY CXX_STANDARD 17)
  set_property(TARGET ${CXX_17_TEST_TARGETS} PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET ${CXX_17_TEST_TARGETS} PROPERTY CXX_EXTENSIONS OFF)
endif()

if(BUILD_PYTHON)
# function(rtpg_add_python_test target)
# add_test (NAME ${target}
#   COMMAND ${Python3_EXECUTABLE} -m unittest ${CMAKE_SOURCE_DIR}/test/${target}.py
#   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/python
# )
# endfunction()

# rtpg_add_python_test(unittest_pg)

add_test(NAME "Python_Tests"
  COMMAND ${Python3_EXECUTABLE} -m unittest discover -s ${CMAKE_CURRENT_SOURCE_DIR}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/python
)

endif(BUILD_PYTHON)
