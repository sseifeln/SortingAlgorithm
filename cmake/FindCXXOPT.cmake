find_path(CXXOPTS_INCLUDE_DIR
  NAMES
    cxxopts.hpp
  PATHS
    ${PROJECT_SOURCE_DIR}/include/cxxopts/include
)

if(CXXOPTS_INCLUDE_DIR)
  set(CXXOPT_FOUND TRUE)
endif (CXXOPTS_INCLUDE_DIR)

# file(GLOB_RECURSE CXXOPT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/include/cxxopts/include/cxxopts.hpp)
# if(CXXOPT_SOURCE_DIR)
#     set(CXXOPT_FOUND TRUE)
#         #  #strip to the blank path
#         #  get_filename_component(PH2_TCUSB_SOURCE_DIR "${PH2_TCUSB_SOURCE_DIR}" PATH)

#         #  find_library(PH2_TCUSB_LIBRARY_DIRS
#         #  NAMES
#         #  libcmsph2_tcusb.so
#         #  libcmsph2_tcusb_utils.so
#         #  PATHS
#         #  ${PH2_TCUSB_SOURCE_DIR}/../lib
#         #  )

#         #  #strip away the path
#         #  get_filename_component(PH2_TCUSB_LIBRARY_DIRS "${PH2_TCUSB_LIBRARY_DIRS}" PATH)
         
#         #  set(PH2_TCUSB_INCLUDE_DIRS
#         #      ${PH2_TCUSB_SOURCE_DIR})

#         #  file(GLOB_RECURSE PH2_TCUSB_LIBRARIES ${PH2_TCUSB_LIBRARY_DIRS}/*.so)
# else(CXXOPT_SOURCE_DIR)
#     set(CXXOPT_FOUND FALSE)
# endif(CXXOPT_SOURCE_DIR)

# include(FindPackageHandleStandardArgs)
# find_package_handle_standard_args(CXXOPT DEFAULT_MSG CXXOPT_SOURCE_DIR)
# # show the PH2_USBINSTLIB_INCLUDE_DIRS and PH2_USBINSTLIB_LIBRARIES variables only in the advanced view
# # mark_as_advanced(PH2_TCUSB_INCLUDE_DIRS PH2_TCUSB_LIBRARY_DIRS)