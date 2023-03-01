#include <future>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <bitset>
#include <iomanip>
#include <numeric>
#include <iostream>
#include "cxxopts.hpp"
#include "InputHandler.h"

int main(int argc, char* argv[])
{
    int cNlines = 0;
    std::string  cInputFile="data/test_data.dat";
    std::string  cOutputFile="data/solution.raw";
    uint32_t cSortWindow=512;
    // options to command line parser 
    cxxopts::Options cOptions("Datasorter", "Produce time-ordered list of events from a medical imaging device.");
    cOptions.add_options()
        ("d,debug", "Enable debugging") // a bool parameter
        ("n,number_of_frames", "Number of frames to parse", cxxopts::value<int>()->default_value("0"))
        ("f,file", "Input file name", cxxopts::value<std::string>()->default_value(cInputFile))
        ("o,output_file", "Output file name", cxxopts::value<std::string>()->default_value(cOutputFile))
        ("c,convert_to_tsv", "Convert default output (raw 64 bit words) into a human readable tsv file", cxxopts::value<std::string>()->default_value(cInputFile))
        ("s,sort_window", "Size of sorting window (in events)", cxxopts::value<int>()->default_value("512"))
        ("h,help", "Print usage")
    ;

    // parse result
    auto cParsedResults = cOptions.parse(argc, argv);
    if (cParsedResults.count("help"))
    {
        std::cout << cOptions.help() << std::endl;
        exit(0);
    }
    
    if ( cParsedResults.count("number_of_frames") ) 
    {
        cNlines = cParsedResults["number_of_frames"].as<int>();
    }
    if ( cParsedResults.count("file") ) 
    {
        cInputFile = cParsedResults["file"].as<std::string>();
    }
    if ( cParsedResults.count("output_file") ) 
    {
        cOutputFile = cParsedResults["output_file"].as<std::string>();
    }

    if( cNlines == 0 ) std::cout << "Parse complete dataset in " << cInputFile << "\n";
    else std::cout << "Parsing " << cNlines << " line(s) from " << cInputFile << "\n";

    // now have to decide how to structure the rest of this 
    InputHandler cMyInputHandler(cInputFile,cOutputFile, cNlines);
    cMyInputHandler.SetSortWindow(cSortWindow);
    cMyInputHandler.SetDebugOut( cParsedResults.count("debug")  );
    cMyInputHandler.Run();
    
    // simple file converter from raw to csv 
    if ( cParsedResults.count("convert_to_tsv") ) 
    {
        auto cFileName = cParsedResults["convert_to_tsv"].as<std::string>();
        cMyInputHandler.ConvertRawOutput(cFileName);
    }

}
