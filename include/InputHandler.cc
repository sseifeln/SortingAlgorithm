#include "InputHandler.h"
#include <chrono>
#include <iostream>
#include <thread>

InputHandler::InputHandler(const std::string& pFileId, size_t pReadLimit): fReadTime{0}, fReadCounter{0}, fFileIsOpened{false}, fReadIsDone{false}, fConsumed{false}, fIsReady{false}
{
    fFileId = pFileId; 
    fReadLimit = pReadLimit;
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
void InputHandler::readFile()
{
    fReadIsDone=false;
    if(InputHandler::isFileOpen() == false){ 
        fReadIsDone = true;
        return;
    }

    fReadTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    bool cStopCondition = fFileStream.eof();
    while( !cStopCondition )// && fReadCounter < 10000  )
    {
        uint64_t cWord=0;
        // first 64 bits --> header --> tells me how many events I have 
        fFileStream.read((char*)&cWord, sizeof(uint64_t));
        auto cFrameSize=cWord&0xFFFF;
        // fFrameCounter += cFrameSize;
        fIsReady=false;
        fMyConditionVar.notify_one(); 
        std::unique_lock<std::mutex> cLock(fMemberMutex);
        for(size_t cNibble=0; cNibble < cFrameSize; cNibble++){ 
            fFileStream.read((char*)&cWord, sizeof(uint64_t)); 
            fQueue.push(cWord);
            fReadCounter++;
        }
        fIsReady=true;
        while (!fConsumed){ 
            cLock.unlock();
            fMyConditionVar.notify_one(); 
            cLock.lock();
        }
        cStopCondition = (fReadLimit == 0) ? fFileStream.eof() : (fFileStream.eof() || fReadCounter >= fReadLimit);
    }
    fReadIsDone=true;
    fReadTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - fReadTime;
    std::cout << "Done reading.." << fReadCounter << " events from input file\n";
}
void InputHandler::process()
{
    while(fReadIsDone==0)
    {
        size_t  cSize=0; 
        {
            // std::cout << "Waiting...\n";
            fConsumed = false;
            std::unique_lock<std::mutex> cLock(fMemberMutex);
            fMyConditionVar.wait(cLock, [this] {return (fIsReady==true || this->fReadIsDone == 1); });
            cSize = fQueue.size(); 
            // std::cout << "...Finished waiting; isReady == " << fIsReady << '\n';
            fConsumed = true;
        }
        
        std::vector<uint64_t> cMyData(cSize);
        for(size_t cIndx = 0 ; cIndx < cSize; cIndx++){ 
            cMyData[cIndx]=fQueue.front();
            fQueue.pop();
        }
    }
    auto cSize = fQueue.size(); 
    for(size_t cIndx = 0 ; cIndx < cSize; cIndx++){ 
        // cMyData[cIndx]=fQueue.front();
        fQueue.pop();
    }
    // std::vector<uint64_t> cMyData; 
    // return cMyData;
}
void InputHandler::ProcessData()
{
    fThProcess = std::async(std::launch::async, &InputHandler::process, this);
}
// add data streamer here 
void InputHandler::ReadFile()
{
    fThRead = std::async(std::launch::async, &InputHandler::readFile, this);
}
void InputHandler::Wait()
{
    fThProcess.get();//promise 
    fThRead.get();//promise 
}