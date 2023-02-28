cmake_minimum_required(VERSION 3.10)
project(FileSorter)

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

add_subdirectory(src)
# add_executable(FileSorter sorter.cc)

