#include "InputHandler.h"
#include <chrono>
#include <iostream>
#include <thread>


InputHandler::InputHandler(const std::string& pFileId): fReadTime{0}, fReadCounter{0}, fFileIsOpened{false}
{
    fFileId = pFileId; 
    InputHandler::openFile();
    // fOutputStream.open("../solution.dat", std::ios::out  | std::fstream::binary);
    // fFileStream.open("../test_data.dat", std::fstream::in | std::fstream::binary);
    // fFileStream.clear();
    // fFileStream.seekg(0, std::ios::beg);
    // fBuffer.reserve(myBufferSize);
}
bool InputHandler::isFileOpen()
{
    std::lock_guard<std::mutex> cLock(fMemberMutex);
    return fFileIsOpened;
}
InputHandler::~InputHandler()
{
    while(fQueue.empty() == false) std::this_thread::sleep_for(std::chrono::microseconds(DESTROYSLEEP));
    InputHandler::closeFile();
}
bool InputHandler::openFile()
{
    if(InputHandler::isFileOpen() == false)
    {
        std::lock_guard<std::mutex> cLock(fMemberMutex);
        // std::cout << "Opening file : " <<  fFileId.c_str() << "\n";
        fFileStream.open(fFileId.c_str(), std::fstream::in | std::fstream::binary);
        fFileIsOpened = fFileStream.is_open();
        if(fFileIsOpened){
            std::cout << fFileId.c_str() << " opened...\n";
            fFileStream.clear();
            fFileStream.seekg(0, std::ios::beg);
        }
    }
    return fFileIsOpened;
}
void InputHandler::closeFile()
{
    if(fFileIsOpened == true)
    {
        fFileIsOpened = false;
        fFileStream.close();
        // add to message stream 
        // std::cout << "Closed binary file: " << BOLDYELLOW << fBinaryFileName << RESET;
    }
}