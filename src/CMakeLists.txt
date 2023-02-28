###############
# EXECUTABLES #
###############

#check for CXXOPT include directory
if(${CXXOPT_FOUND})
    include_directories(${CXXOPTS_INCLUDE_DIR})
    link_directories(${CXXOPTS_INCLUDE_DIR})
endif()

# add headers that should be accessible from the executable
include_directories(${PROJECT_SOURCE_DIR}/include)

# Initial set of libraries
set(LIBS ${LIBS} FileSorter_Headers)

# find all .cc files in this directory and add executable
file(GLOB BINARIES RELATIVE ${PROJECT_SOURCE_DIR}/src *.cc)
message("--     ${BoldCyan}#### Building the following executables: ####${Reset}")
foreach(sourcefile ${BINARIES})
    string(REPLACE ".cc" "" name ${sourcefile})
    message(STATUS "--          ${BoldCyan}####    ${name} ####${Reset}")
    add_executable(${name} ${sourcefile})
    target_link_libraries(${name} ${LIBS})
endforeach(sourcefile ${BINARIES})
