#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(coinc)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

message("Found Geant4-Version: " ${Geant4_VERSION})

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})

##---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
SET(CMAKE_MODULE_PATH $ENV{G4LIB}/Geant4-${Geant4_VERSION}/Modules)

#----------------------------------------------------------------------------
# Find ROOT (required package)
#
# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS RIO Net)

# ROOT version 6 required
if(ROOT_FOUND)
  STRING(REGEX MATCH "6.*" VERSION6MATCH ${ROOT_VERSION})
  if(NOT VERSION6MATCH)
    message(FATAL_ERROR "project requires ROOT 6")
  endif()
    message("Found Root-Version: " ${ROOT_VERSION})
endif()

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

message("ROOT_USE_FILE: " ${ROOT_USE_FILE})
message("ROOT_INCLUDE_DIRS: " ${ROOT_INCLUDE_DIRS})
message("GEANT4_INCLUDE_DIR: " ${Geant4_INCLUDE_DIR})

cmake_host_system_information(RESULT _host_name QUERY HOSTNAME)
message("Set configuration for: " ${_host_name})

#----------------------------------------------------------------------------
#create folder structure in build directory
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/results)
#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build the executable. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(coinc_SCRIPTS
      vis.mac
      run.mac
  )

foreach(_script ${coinc_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include 
                    ${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIRS})

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)



include(${Geant4_USE_FILE})

include_directories(${PROJECT_SOURCE_DIR}/include)

EXECUTE_PROCESS(COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS}")

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries and common library
#
add_executable(coinc coinc.cc ${sources} ${headers})

EXECUTE_PROCESS(COMMAND root-config --libs OUTPUT_VARIABLE ROOT_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_EXE_LINKER_FLAGS ${ROOT_LD_FLAGS})

#----------------------------------------------------------------------------
# Add the library
#
link_directories(/usr/lib64)
target_link_libraries(coinc ${Geant4_LIBRARIES})
target_link_libraries(coinc ${ROOT_LIBRARIES} Eve)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
# and the library to 'lib' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS coinc DESTINATION bin)

