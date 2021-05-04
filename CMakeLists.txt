cmake_minimum_required(VERSION 3.12)
project(rtpg VERSION 0.0.1 LANGUAGES CXX)

# Get the project name in capital letters
# for the lib namespace
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_CAPS)

# Check that the compoiler has c++17 support
# Crash otherwise.
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
if(COMPILER_SUPPORTS_CXX17)
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has C++17 support.")
else()
  message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} "
    "has no C++17 support. Please use a different C++ compiler.")
endif()

# Options. Turn on with 'cmake -DBUILD_TESTING=ON'.
# catkin build manif --cmake-args -DBUILD_TESTING=ON -DBUILD_EXAMPLES=ON
option(BUILD_PYTHON "Build Python bindings." OFF)
option(BUILD_TESTING "Build all tests." OFF)
option(ENABLE_CPPCHECK "Enable cppcheck." OFF)
option(BUILD_EXAMPLES "Build all examples." OFF)

set(REQUIRED_PYTHON_VERSION "DEFAULT" CACHE STRING "The required python version")

###########
## Build ##
###########

include(ExternalProject)

ExternalProject_Add(
  handle_allocator
  GIT_REPOSITORY    https://github.com/lounick/handle_allocator.git
  # GIT_TAG origin/master
  CMAKE_ARGS -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
  TIMEOUT 5
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
  INSTALL_COMMAND "" 
)
ExternalProject_Get_Property(handle_allocator source_dir)
set(HAIncludeDir ${source_dir}/include)

ExternalProject_Add(
  stduuid
  GIT_REPOSITORY    https://github.com/mariusbancila/stduuid.git
  # GIT_TAG origin/master
  CMAKE_ARGS -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
  TIMEOUT 5
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
  INSTALL_COMMAND "" 
)
ExternalProject_Get_Property(stduuid source_dir)
set(StduuidIncludeDir ${source_dir}/include)

#TODO this is required for C++ < 20
set(StduuidIncludeGSL ${source_dir}/gsl)

# If we are building python bindings
if(BUILD_PYTHON)
include(FetchContent)
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG origin/stable
)
if (REQUIRED_PYTHON_VERSION STREQUAL "DEFAULT")
  message(STATUS "No python version defined will search for a default")
  find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
else()
  find_package(Python3 ${REQUIRED_PYTHON_VERSION} EXACT COMPONENTS Interpreter Development REQUIRED)
endif()
endif(BUILD_PYTHON)

add_library(${PROJECT_NAME} SHARED
    src/property_graph.cpp
    src/graph_element.cpp
    src/node.cpp
    src/edge.cpp
)

add_dependencies(${PROJECT_NAME} handle_allocator)
add_dependencies(${PROJECT_NAME} stduuid)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${HAIncludeDir}>
        $<BUILD_INTERFACE:${StduuidIncludeDir}>
        $<BUILD_INTERFACE:${StduuidIncludeGSL}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

if(BUILD_PYTHON)
FetchContent_MakeAvailable(pybind11)
add_subdirectory(${pybind11_SOURCE_DIR})

# Do python library things
pybind11_add_module(_${PROJECT_NAME}_py WITHOUT_SOABI
  src/bindings/bindings.cpp
  src/bindings/graph_element.cpp
  src/bindings/uuid.cpp
  src/bindings/handle.cpp
  src/bindings/node.cpp
  src/bindings/edge.cpp
  src/bindings/property_graph.cpp
)
target_include_directories(_${PROJECT_NAME}_py
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${StduuidIncludeDir}>
        $<BUILD_INTERFACE:${HAIncludeDir}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(_${PROJECT_NAME}_py PRIVATE cxx_std_17)
target_link_libraries(_${PROJECT_NAME}_py PRIVATE ${PROJECT_NAME})

pybind11_extension(_${PROJECT_NAME}_py)
pybind11_strip(_${PROJECT_NAME}_py)

# Overrides the pybind11 crappy suffix
#TODO: Check if you can remove the ABI info with WITHOUT_SOABI parameter
# See: https://github.com/pybind/pybind11/issues/2937
set_target_properties(_${PROJECT_NAME}_py PROPERTIES PREFIX ""
                                                     SUFFIX ".so")

add_custom_command(TARGET _${PROJECT_NAME}_py POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:_${PROJECT_NAME}_py>
  ${PROJECT_SOURCE_DIR}/python/rtpg/_${PROJECT_NAME}_py.so
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}>
  ${PROJECT_SOURCE_DIR}/python/rtpg/lib${PROJECT_NAME}.so
)
endif(BUILD_PYTHON)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_compile_options(${PROJECT_NAME} -ftemplate-depth=512)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # GCC is not strict enough by default, so enable most of the warnings.
  target_compile_options(${PROJECT_NAME} PUBLIC
    -Werror=all
    -Werror=extra
    -Wno-unknown-pragmas
    -Wno-sign-compare
    -Wno-unused-parameter
    -Wno-missing-field-initializers
  )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  target_compile_options(${PROJECT_NAME} /bigobj)
endif()

#############
## Install ##
#############
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# TODO check the correctness of the config_install_dir
set(config_install_dir "share/${PROJECT_NAME}/cmake")
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

# Targets:
install(
  TARGETS  ${PROJECT_NAME}
  EXPORT   ${PROJECT_NAME}Targets
  INCLUDES DESTINATION include
  LIBRARY DESTINATION lib
)

install(
  EXPORT      ${PROJECT_NAME}Targets
  NAMESPACE   ${PROJECT_NAME_CAPS}::
  DESTINATION "${config_install_dir}"
)

export(
  TARGETS ${PROJECT_NAME}
  NAMESPACE ${PROJECT_NAME_CAPS}::
  FILE ${PROJECT_NAME}Targets.cmake
)

export(PACKAGE ${PROJECT_NAME})

if(BUILD_PYTHON)
#TODO: Add install rule for python
endif(BUILD_PYTHON)

## Configuration

#
configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
  "${generated_dir}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "${config_install_dir}"
)

# Configure '<PROJECT-NAME>ConfigVersion.cmake'
write_basic_package_version_file(
  "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION 0.0.0
  COMPATIBILITY AnyNewerVersion
)

# Config
install(
  FILES       "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake"
              "${generated_dir}/${PROJECT_NAME}Config.cmake"
  DESTINATION "${config_install_dir}"
)

# Headers:

install(
  DIRECTORY "${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}"
  DESTINATION include
)

##############
## Examples ##
##############

#if(BUILD_EXAMPLES)
#
#  add_subdirectory(examples)
#
#endif(BUILD_EXAMPLES)

#############
## Testing ##
#############

# ------------------------------------------------------------------------------
# Coverage
# ------------------------------------------------------------------------------

if(ENABLE_COVERAGE)
  add_definitions(-DMANIF_COVERAGE_ENABLED)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftest-coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

# ------------------------------------------------------------------------------
# CppCheck
# ------------------------------------------------------------------------------

if(ENABLE_CPPCHECK)

  # Find CppCheck executable
  find_program(CPPCHECK "cppcheck")
  if(CPPCHECK)

      # Set export commands on
      # set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

      list(APPEND CPPCHECK_ARGS
        --enable=all
        --std=c++11
        # --verbose
        --quiet
        # --check-config
        --xml-version=2
        --language=c++
        # Comment the line below to run cppcheck-html
        --error-exitcode=1
        --inline-suppr
        --suppress=*:*googletest-*
        --suppress=missingIncludeSystem
        --suppress=missingInclude
        --suppress=unmatchedSuppression:*
        --suppress=syntaxError:*tangent_base.h:370
        # Uncomment the line below to run cppcheck-html
        # --output-file=${CMAKE_BINARY_DIR}/cppcheck_results.xml
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/examples
        ${CMAKE_SOURCE_DIR}/test
      )

      add_custom_target(run-cppcheck
          COMMAND ${CPPCHECK} ${CPPCHECK_ARGS}
          COMMENT "Generate cppcheck report for the project"
      )

      find_program(CPPCHECK_HTML "cppcheck-htmlreport")
      if(CPPCHECK_HTML)
          add_custom_target(cppcheck-html
              COMMAND ${CPPCHECK_HTML}
              --title=${CMAKE_PROJECT_NAME}
              --file=${CMAKE_BINARY_DIR}/cppcheck_results.xml
              --report-dir=${CMAKE_BINARY_DIR}/cppcheck_results
              --source-dir=${CMAKE_SOURCE_DIR}
              COMMENT "Convert cppcheck report to HTML output"
          )
          ADD_DEPENDENCIES(cppcheck-html run-cppcheck)
      endif()
  endif()

endif()

# ------------------------------------------------------------------------------
# Valgrind
# ------------------------------------------------------------------------------

if(ENABLE_VALGRIND)
  # ctest -T memcheck
  include(CTest)
  set(MEMORYCHECK_COMMAND_OPTIONS "${MEMORYCHECK_COMMAND_OPTIONS} --leak-check=full")
  set(MEMORYCHECK_COMMAND_OPTIONS "${MEMORYCHECK_COMMAND_OPTIONS} --track-fds=yes")
  set(MEMORYCHECK_COMMAND_OPTIONS "${MEMORYCHECK_COMMAND_OPTIONS} --trace-children=yes")
  set(MEMORYCHECK_COMMAND_OPTIONS "${MEMORYCHECK_COMMAND_OPTIONS} --show-reachable=yes")
  set(MEMORYCHECK_COMMAND_OPTIONS "${MEMORYCHECK_COMMAND_OPTIONS} --error-exitcode=1")
endif()

# ------------------------------------------------------------------------------
# Unit Tests
# ------------------------------------------------------------------------------

if(BUILD_TESTING)

  enable_testing()
  add_subdirectory(test)
  #TODO: Add python testing like https://github.com/SimpleITK/SimpleITK

endif(BUILD_TESTING)
