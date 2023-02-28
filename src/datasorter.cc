#include <future>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <bitset>
#include <iomanip>
#include <numeric>
#include <iostream>
#include <getopt.h>

int main(int argc, char* argv[])
{
    // std::copy(argv, argv + argc, std::ostream_iterator<char *>(std::cout, "\n"));

    // option cCLOptions[] = {
    //     {"number_of_lines_to_parse", optional_argument, NULL, 'n'}};
    
    std::cout << "Datasorter bare-bones executable\n";
    int  cOption=1;
    while ((cOption = getopt (argc, argv, "abc:")) != -1)
    {
        // const int cCLOption = getopt_long(argc, argv, "nE::", cCLOptions, 0);
        // if (cCLOption == -1) {
        //     break;
        // }

        switch (cOption) {
            case 'n':
               auto cValue = optarg;
               std::cout << "Will only sort through first " << cValue << " lines in the file\n";
               break;
        }
    }
}
