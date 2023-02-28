#include <stdint.h>
#include <stddef.h>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

class InputHandler 
{
    public :
        InputHandler(); 
        InputHandler(const std::string& pFileId);
        ~InputHandler();

        void SetFileName(const std::string& pFileId){ fFileId =pFileId; }
        uint32_t GetReadTime(){ return fReadTime;}
        uint32_t GetNEventsRead(){return fReadCounter;}
    private :
        uint32_t fReadTime;
        size_t fReadCounter;
        std::fstream fFileStream;
        std::string fFileId;
};