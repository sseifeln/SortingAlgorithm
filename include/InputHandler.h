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
        // set input and output files 
        void SetInputFileName(const std::string& pFileId){ fInputFileName =pFileId; }
        void SetOutputFileName(const std::string& pFileId){ fOutputFileName =pFileId; }
        // get time to read(and sort) events
        uint32_t GetReadTime(){ return fReadTime;}
        uint32_t GetNEventsRead(){return fReadCounter;}
        // get time to process events
        uint32_t GerProcessingTime(){ return fProcessTime;}
        uint32_t GetNEventsProcessed(){return fProcessedCounter;}

        // 
        void ReadFile(); 
        void ProcessData();

        // wait 
        void Wait(); 
        std::pair<bool,uint64_t> GetOutput();
        void Run(); 

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
        TSQueue<Event>  fQueue;
    
    // member variables  - book keeping 
    private :
        size_t fSearchWindow{512};
        size_t fReadLimit{0};
        
        // time to read file
        uint32_t fReadTime;
        // time to process the file
        uint32_t fProcessTime;
        // number of 64 bit wordsread from the file 
        size_t fReadCounter;
        size_t fProcessedCounter; 
        // file name 
        std::string fInputFileName;
        std::string fOutputFileName;
        // is file open 
        bool fFileIsOpened; 

    // memeber variables  - threading control
    private :
        //  futures
        std::future<void> fThRead; 
        std::future<void> fThProcess;

        // synchronization mutex 
        mutable std::mutex  fMemberMutex;
        bool                fReadIsDone;
        bool                fProcessing;
    
        
};