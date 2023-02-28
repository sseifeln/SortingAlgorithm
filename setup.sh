#!/bin/bash
export PROJECT_SOURCE_DIR=$(pwd)
#######
# HEADER PATH FOR EXTERNAL CXXOPT #
#######
export CXXOPT_HEADER_PATH=$PROJECT_SOURCE_DIR/include/cxxopts/include

##########
# ADD EXECUTABLES IN BIN TO PATH #
##########
export PATH=$PROJECT_SOURCE_DIR/bin:$PATH