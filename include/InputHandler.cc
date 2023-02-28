#include "InputHandler.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <algorithm>
// #include <numeric>

InputHandler::InputHandler(const std::string& pFileId, size_t pReadLimit): fReadTime{0}, fReadCounter{0}, fFileIsOpened{false}, fReadIsDone{false}, fConsumed{false}, fIsReady{false}
, fProcessing{false}
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
    while( !cStopCondition )
    {
        uint64_t cWord=0;
        // first 64 bits --> header --> tells me how many events I have 
        fFileStream.read((char*)&cWord, sizeof(uint64_t));
        auto cFrameSize=cWord&0xFFFF;
        fIsReady=false;
        fMyConditionVar.notify_one(); 
        std::unique_lock<std::mutex> cLock(fMemberMutex);
        for(size_t cNibble=0; cNibble < cFrameSize; cNibble++){ 
            fFileStream.read((char*)&cWord, sizeof(uint64_t)); 
            Event cEvent; 
            cEvent.fTimestamp = cWord&0xFFFFFFFF;
            cEvent.fEnergy = (cWord >> 32 ) & 0xFFFFFFFF; 
            cEvent.fCounter = 0; 
            fQueue.push(cEvent);
            fReadCounter++;
        }
        
        cStopCondition = ( fReadCounter >= fReadLimit && fReadLimit!=0 ) || fFileStream.eof(); 
        // every time I've accumulated 500 events in the queue 
        // do something with them 
        if( fQueue.size() > fSearchWindow || cStopCondition  ) 
        {
            // sort entire queue 
            auto cSize=fQueue.size();
            std::vector<Event> cFrame(cSize);
            for(size_t cIndx=0;cIndx<cFrame.size();cIndx++) cFrame[cIndx]=fQueue.pop();
            std::sort( cFrame.begin(), cFrame.end() );
            for(const auto& cEvent : cFrame){ 
                fQueue.push(cEvent);
                // std::cout << cEvent.fTimestamp << "\n";
            }

            fIsReady=true;
            while (!fConsumed){ 
                cLock.unlock();
                fMyConditionVar.notify_one(); 
                cLock.lock();
            }
        }
        std::this_thread::sleep_for(std::chrono::microseconds(WAITSLEEP));
    }
    fReadIsDone=true;
    fReadTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - fReadTime;
    std::cout << "Done reading.." << fReadCounter << " events from input file\n";
}
void InputHandler::process()
{
    size_t cNCalls=0;
    fProcessing=true;
    uint32_t cLastTimer=0;
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
        if(cSize/2 == 0 ) continue;

        std::vector<Event> cMyData(cSize/2);
        for(size_t cIndx = 0 ; cIndx < cSize/2; cIndx++){ 
            cMyData[cIndx]=fQueue.pop();
        }
        std::sort( cMyData.begin(), cMyData.end() );
        std::vector<uint64_t> cWords(cMyData.size());
        for(size_t cIndx = 0 ; cIndx < cMyData.size(); cIndx++){ 
            uint32_t cTimeStamp = cMyData[cIndx].fTimestamp;
            double cTime = (double)cTimeStamp;
            uint64_t cWrd = ((uint64_t)(cMyData[cIndx].fEnergy) << 32) | cMyData[cIndx].fTimestamp;
            // if( cWrd == 0x00 ) std::cout << "!!!! Event " << cIndx << " has 0 energy + 0 timestamp ..\n";
            cWords[cIndx]=cWrd;
            if( cTime - (double)cLastTimer < 0 ) std::cout << "!!! --ve \t" << cTimeStamp << "\t" << cLastTimer << "\n";
            // else std::cout << "+ve \t" << cTime  << "\t" << cLastTimer << "\n";
            cLastTimer=cMyData[cIndx].fTimestamp;
        }

        if(cNCalls%1000 == 0 )  std::cout << "Holding " << cMyData.size() << " entries\n";
        cNCalls++;
    }
    auto cSize = fQueue.size(); 
    for(size_t cIndx = 0 ; cIndx < cSize; cIndx++){ 
        // cMyData[cIndx]=fQueue.front();
        fQueue.pop();
    }
    fProcessing=false;
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
    fThRead.get();//promise 
    fThProcess.get();//promise 
    std::cout << "Promises received\n";
}
void InputHandler::Run()
{
    this->ReadFile();
    this->ProcessData();
    this->Wait();
}