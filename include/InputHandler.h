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
#include <condition_variable>
#include "Utilities.h"
#include "FileIO.h"

// #############
// # CONSTANTS #
// #############
#define DESTROYSLEEP 100 // [microseconds]
#define WAITSLEEP 1 // [microseconds]
using namespace std;

class InputHandler 
{
    public :
        InputHandler(const std::string& pFileId,size_t pReadLimit=0);
        ~InputHandler();

        // setter and getter functions for class members
        void SetOutputStream(const std::string& pFileId); 
        void SetFileName(const std::string& pFileId){ fFileId =pFileId; }
        uint32_t GetReadTime(){ return fReadTime;}
        uint32_t GetNEventsRead(){return fReadCounter;}
        uint32_t GerProcessingTime(){ return fProcessTime;}
        uint32_t GetNEventsProcessed(){return fProcessedCounter;}

        // file access 
        void ReadFile(); 
        void ProcessData();

        // wait 
        void Wait(); 
        std::pair<bool,uint64_t> GetOutput();
        void Run(); 

    // member functions for internal use 
    // all safe-guarded using synchronization mutex 
    private : 
        // check if file (fFileId) is open 
        bool isFileOpen(); 
        // open file (fFileId)
        bool openFile();
        // close file (fFileId)
        void closeFile();
        // read from file 
        void readFile();
        // get head of the queue 
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
        // file input stream
        std::fstream fFileStream;
        std::ofstream fOutputStream; 
        // file name 
        std::string fFileId;
        // is file open 
        bool fFileIsOpened; 

    // memeber variables  - threading control
    private :
        //  futures
        std::future<void> fThRead; 
        std::future<void> fThProcess;
        std::future<void> fThStream;

        // synchronization mutex 
        mutable std::mutex  fMemberMutex;
        std::condition_variable fMyConditionVar;
        bool                fReadIsDone;
        bool                fConsumed; 
        bool                fIsReady;
        bool                fProcessing;
        bool                fRunning;
    
        
};