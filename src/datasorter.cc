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
    // options to command line parser 
    cxxopts::Options cOptions("Datasorter", "Produce time-ordered list of events from a medical imaging device.");
    cOptions.add_options()
        ("d,debug", "Enable debugging") // a bool parameter
        ("n,number_of_lines", "Number of lines to parse", cxxopts::value<int>()->default_value("0"))
        ("f,file", "Input file name", cxxopts::value<std::string>()->default_value(cInputFile))
        ("h,help", "Print usage")
    ;

    // parse result
    auto cParsedResults = cOptions.parse(argc, argv);
    if (cParsedResults.count("help"))
    {
        std::cout << cOptions.help() << std::endl;
        exit(0);
    }
    
    if ( cParsedResults.count("number_of_lines") ) 
    {
        cNlines = cParsedResults["number_of_lines"].as<int>();
    }
    if ( cParsedResults.count("file") ) 
    {
        cInputFile = cParsedResults["file"].as<std::string>();
    }

    if( cNlines == 0 ) std::cout << "Parse complete dataset in " << cInputFile << "\n";
    else std::cout << "Parsing " << cNlines << " line(s) from " << cInputFile << "\n";

    // now have to decide how to structure the rest of this 

}
