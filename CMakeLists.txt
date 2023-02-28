string(ASCII 27 Esc)
set(Reset       "${Esc}[m"    )
set(Bold        "${Esc}[1m"   )
set(Red         "${Esc}[31m"  )
set(Green       "${Esc}[32m"  )
set(Yellow      "${Esc}[33m"  )
set(Blue        "${Esc}[34m"  )
set(Magenta     "${Esc}[35m"  )
set(Cyan        "${Esc}[36m"  )
set(White       "${Esc}[37m"  )
set(BoldRed     "${Esc}[1;31m")
set(BoldGreen   "${Esc}[1;32m")
set(BoldYellow  "${Esc}[1;33m")
set(BoldBlue    "${Esc}[1;34m")
set(BoldMagenta "${Esc}[1;35m")
set(BoldCyan    "${Esc}[1;36m")
set(BoldWhite   "${Esc}[1;37m")
cmake_minimum_required(VERSION 3.10)
project(FileSorter)

# Set the output directory
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
MESSAGE( STATUS "${BoldRed}" )
MESSAGE( FATAL_ERROR "${Reset}In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt." )
endif()

# ---------- Setup output Directories -------------------------
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
${PROJECT_SOURCE_DIR}/lib
CACHE PATH
"Output Output Directory for all Libraries"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
${PROJECT_SOURCE_DIR}/bin
CACHE PATH
"Output Output Directory for all Executables."
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
${PROJECT_SOURCE_DIR}/bin
CACHE PATH
"Single Output Directory for all static libraries."
)

# ---------- Setup module path -------------------------
# Set the cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set (CMAKE_CXX_FLAGS "-O3 -fopenmp -fno-omit-frame-pointer -Werror -pedantic -Wno-unused-parameter -Wno-ignored-qualifiers -Wno-missing-field-initializers -Wall -Werror=return-type -Wextra -Winit-self -Wno-unused-local-typedefs -Woverloaded-virtual -Wnon-virtual-dtor -Wdelete-non-virtual-dtor -pthread -Wcpp -fPIC ${CMAKE_CXX_FLAGS} -DELPP_THREAD_SAFE")
    

# Check for external dependences
message("--")
message("--     ${BoldCyan}#### Checking for external Dependencies ${CMAKE_MODULE_PATH} ####${Reset}")

# CXXOPT
find_package(CXXOPT)
if(${CXXOPT_FOUND})
    message(STATUS "--      ${BoldGreen}  Enabling CXXOPT component ${CXXOPTS_INCLUDE_DIR}")
endif(${CXXOPT_FOUND})

# ---------- Compile sub-directories -------------------------
add_subdirectory(src)
add_subdirectory(include)

