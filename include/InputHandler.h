/*!

        \file                   InputHandler.h
        \brief                                   Handles retreival of event (timestamp.energy) data from text file
        \author                                  Sarah SEIF EL NASR STOREY
        \version                 1.0
        \date                                    28/02/22
        Support :                                mail to : sarah.storey@cern.ch
 */
 
#include <stdint.h>
#include <stddef.h>
#include <fstream>
#include <mutex>
#include <queue>
#include <future>
#include "Utilities.h"

// #############
// # CONSTANTS #
// #############
#define DESTROYSLEEP 100 // [microseconds]
using namespace std;

// forward declare FileIO object 
class FileIO;
class InputHandler 
{
    public :
        InputHandler(const std::string& pInputFileName, const std::string& pOutputFileName, size_t pReadLimit=0);
        ~InputHandler();

        // setter and getter functions for class members
        // setter and getter for search window 
        // defines how many events to look at when performing the search 
        void SetSortWindow(uint32_t pWindow){ fSortWindow=pWindow;}
        uint32_t GetSortWindow(){return fSortWindow;}

        // set input and output files 
        void SetInputFileName(const std::string& pFileId){ fInputFileName =pFileId; }
        void SetOutputFileName(const std::string& pFileId){ fOutputFileName =pFileId; }
        // get time to read(and sort) events
        uint32_t GetReadTime(){ return fReadTime;}
        uint32_t GetNEventsRead(){return fReadCounter;}
        // get time to process events
        uint32_t GerProcessingTime(){ return fProcessTime;}
        uint32_t GetNEventsProcessed(){return fProcessedCounter;}
        // enable debug printout 
        void  SetDebugOut(bool pDebug){ fDebugOut=pDebug;}
        // get average number of events handled at a time 
        void ReadFile(); 
        void ProcessData();

        // wait 
        void Wait(); 
        std::pair<bool,uint64_t> GetOutput();
        void Run(); 
        void ConvertRawOutput(const std::string& pInputFileName);

    // member functions to handle file IO 
    private : 
        // FileIO object to handle files 
        FileIO* fIOHandlerInput;
        FileIO* fIOHandlerOutput;
         
        // file streams 
        std::fstream fFileStream;
        std::fstream fOutputStream; 

    // member functions for internal use 
    // all safe-guarded using synchronization mutex 
    private : 
        // read event data from file and sort 
        void readFile();
        // send data to the output 
        void process(); 
        // stream data 
        uint32_t print(size_t pSize, std::ostream& pOs);

    // queue handlings 
    private : 
        // queue to hold event data from the file 
        TSQueue<uint64_t>  fQueue;

    // member variables  - book keeping 
    private :
        size_t fSortWindow{512};
        size_t fReadLimit{0};
        
        // time to read file
        uint32_t fReadTime;
        // time to process the file
        uint32_t fProcessTime;
        // number of 64 bit wordsread from the file 
        size_t fReadCounter;
        size_t fProcessedCounter; 
        // internal variables to estimate 
        // how much data is stored in memory 
        double fAverageQueueHandled;
        size_t fNCalls; 

        // file name 
        std::string fInputFileName;
        std::string fOutputFileName;
        // debug output flag 
        bool  fDebugOut{false};


    // member variables  - threading control
    private :
        //  futures
        std::future<void> fThRead; 
        std::future<void> fThProcess;

        // synchronization mutex 
        mutable std::mutex  fMemberMutex;
        bool                fReadIsDone;
        bool                fProcessing;
    
        
};