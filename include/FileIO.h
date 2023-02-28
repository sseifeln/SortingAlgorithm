/*!

        \file                   FileIO.h
        \brief                                   Handles opening access to iostreams (files only) 
        \author                                  Sarah SEIF EL NASR STOREY
        \version                 1.0
        \date                                    28/02/22
        Support :                                mail to : sarah.storey@cern.ch
 */
 
#include <stdint.h>
#include <stddef.h>
#include <fstream>
#include <mutex>
#include <condition_variable>

// #############
// # CONSTANTS #
// #############
using namespace std;

class FileIO 
{
    public :
        FileIO();
        ~FileIO();

        // open file  (pIO == stream object) 
        bool Open(std::fstream& pIO);
        // close file (pIO == stream object) 
        void Close(std::fstream& pIO);
        // Check if file is open 
        bool IsOpen(std::fstream& pIO){ return isFileOpen();}

        // setter and getter functions 
        std::string GetFilename() const { return fFileId; }
        void  SetFilename(const std::string& pFileId){ fFileId=pFileId;}
        void  SetOption(char pOption){ fOption=pOption;}
        char  GetOption(char pOption){ return fOption;}

    // member functions for internal use 
    // all safe-guarded using synchronization mutex 
    private : 
        // check if file (fFileId) is open 
        bool isFileOpen(); 
        
        
    // member variables  - book keeping 
    private :
        // file input stream
        char   fOption;
        // file name 
        std::string fFileId;
        // is file open 
        bool fFileIsOpened; 

    // memeber variables  - threading control
    private :
        // synchronization mutex 
        std::thread                       fThread;
        mutable std::mutex  fMemberMutex;
        std::condition_variable fMyConditionVar;
    
        
};