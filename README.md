# Coding exercise 
C++ based solution to a programming exercise. This repository contains the source code needed to output a sequence of data from a medical imager ordered by their true arrival time.  The code provided : 
 * loads the raw data 
 * orders the event data by their true arrival time 
 * saves the ordered event data in the same raw format as the input file and also, if requested, as a tab separated file

# Quick start for Unix Systems

This repository includes a pre-compiled executable that can be used to test the implemented algorithm on a dataset of your choice. An example raw file is also included as a reference. To run the code for the first time 

```
git clone -b --recursive https://github.com/sseifeln/SortingAlgorithm.git
source setup.sh
datasorter -h 
```
clone the repository, and the required submodules, and configure the local environment.  Executing **datasorter** with the -h flag  lists all the available command line options and their default options. The command line arguments allow you to 
*  Enable/disable debug printout (-d)  of events in binary format
*  Configure the number of frames (-n) to run over 
*  Choose the input file (-f) to parse 
*  Choose the name of the binary output file (-o)  
*  Choose the name of the tab separated output file (-c) 
* Configure the size of the sorting window (-s) 

For example, the following command can be used to 
```
datasorter -f data/raw_data.dat -c data/test_output.tsv -d
```
time-order the events stored in data/raw_data.dat and store them in a tab separated file located at data/test_output.tsv.
## Build instructions   

The only build requirement is a C++ compiler that supports C++11 features and  [CMake3](https://cmake.org/install/).  The only external dependancy is [cxxopts](https://github.com/jarro2783/cxxopts), a simple stand-alone library for command line parsing, that is included as a submodule in this respository.  To compile from the top directory 
```
mkdir -p build
cmake3 ..
cd $PROJECT_SOURCE_DIR
make -C build -j$(nproc) 
``'
