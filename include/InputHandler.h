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

// #############
// # CONSTANTS #
// #############
#define DESTROYSLEEP 100 // [microseconds]
using namespace std;

class InputHandler 
{
    public :
        InputHandler(const std::string& pFileId,size_t pReadLimit=0);
        ~InputHandler();

        // setter and getter functions for class members
        void SetFileName(const std::string& pFileId){ fFileId =pFileId; }
        uint32_t GetReadTime(){ return fReadTime;}
        uint32_t GetNEventsRead(){return fReadCounter;}

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
    
    // member variables  - book keeping 
    private :
        //
        size_t fSearchWindow{500};
        //
        size_t fReadLimit{0};
        // queue to hold 64 bit words from file 
        EventQueue fQueue; 

        // time to read file
        uint32_t fReadTime;
        // number of 64 bit wordsread from the file 
        size_t fReadCounter;
        // file input stream
        std::fstream fFileStream;
        // file name 
        std::string fFileId;
        // is file open 
        bool fFileIsOpened; 

    // memeber variables  - threading control
    private :
        //  futures
        std::future<void> fThRead; 
        std::future<void> fThProcess;

        // synchronization mutex 
        std::mutex  fMemberMutex;
        std::condition_variable fMyConditionVar;
        bool                fReadIsDone;
        bool                fConsumed; 
        bool                fIsReady;
        bool                fProcessing;
        bool                fRunning;
    
        
};