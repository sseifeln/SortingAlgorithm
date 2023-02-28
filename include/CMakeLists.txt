###############
# EXECUTABLES #
###############

# Find source files
file(GLOB HEADERS *.h)
file(GLOB SOURCES *.cc)

# Add the library
add_library(FileSorter_Headers STATIC ${SOURCES} ${HEADERS})
    

file(GLOB BINARIES RELATIVE ${PROJECT_SOURCE_DIR}/include *.cc)
message("--     ${BoldYellow}#### Building the following headers: ####${Reset}")
foreach(sourcefile ${BINARIES})
        string(REPLACE ".cc" "" name ${sourcefile})
        message(STATUS "--          ${BoldYellow}####        ${name}####${Reset}")
endforeach(sourcefile ${BINARIES})
# TARGET_LINK_LIBRARIES(Ph2_Tools ${LIBS})
