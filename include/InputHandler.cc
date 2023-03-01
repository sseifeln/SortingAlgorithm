#include "InputHandler.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <algorithm>
#include <iomanip>
#include <bitset>
#include "FileIO.cc"

InputHandler::InputHandler(const std::string& pInputFileName,  const std::string& pOutputFileName, size_t pReadLimit): fReadTime{0}, fProcessTime{0}, fReadCounter{0}, fProcessedCounter{0}, fFileIsOpened{false}, fReadIsDone{false}
{
    // configure maximum number of lines to read from the file 
    fReadLimit = pReadLimit;
    // configure input stream 
    fIOHandlerInput = new FileIO();
    fInputFileName=pInputFileName;
    fIOHandlerInput->SetOption('r');
    fIOHandlerInput->SetFilename(fInputFileName); 
    fIOHandlerInput->Open(fFileStream); 
    // configure output stream 
    fIOHandlerOutput = new FileIO();
    fOutputFileName=pOutputFileName;
    fIOHandlerOutput->SetOption('w');
    fIOHandlerOutput->SetFilename(fOutputFileName); 
    fIOHandlerOutput->Open(fOutputStream); 
    if(fIOHandlerOutput->IsOpen(fOutputStream)) fOutputStream << "Time\tEnergy\n";
}
InputHandler::~InputHandler()
{
    while(fQueue.empty() == false) std::this_thread::sleep_for(std::chrono::microseconds(DESTROYSLEEP));
    fIOHandlerInput->Close(fFileStream); 
    fIOHandlerOutput->Close(fOutputStream); 

    delete fIOHandlerInput;
    fIOHandlerInput = nullptr;
    
    delete fIOHandlerOutput;
    fIOHandlerOutput = nullptr;
}
// reading event data from file 
// and sorting them into a queue
void InputHandler::readFile()
{
    fReadIsDone=false;
    if(fIOHandlerInput->IsOpen(fFileStream) == false){ 
        fReadIsDone = true;
        return;
    }

    fReadTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    bool cStopCondition = fFileStream.eof();
    std::queue<Event> cLocalQueue; 
    while( !cStopCondition )
    {
        uint64_t cWord=0;
        // first 64 bits --> header --> tells me how many events I have 
        fFileStream.read((char*)&cWord, sizeof(uint64_t));
        auto cFrameSize=cWord&0xFFFF;
        for(size_t cNibble=0; cNibble < cFrameSize; cNibble++){ 
            fFileStream.read((char*)&cWord, sizeof(uint64_t)); 
            Event cEvent; 
            cEvent.fTimestamp = cWord&0xFFFFFFFF;
            cEvent.fEnergy = (cWord >> 32 ) & 0xFFFFFFFF; 
            cEvent.fCycle = 0;
            // std::cout << cEvent.fTimestamp << "\n";
            cLocalQueue.push(cEvent);
            fReadCounter++;
        }

        cStopCondition = ( fReadCounter >= fReadLimit && fReadLimit!=0 ) || fFileStream.eof(); 
        // every time I've accumulated the corect number of events in the local queue.. 
        // sort and push the first half into the processing queue
        if( cLocalQueue.size() >= fSortWindow )
        {
            // std::cout << "Accumulated enough events in my local queue..\t" << cLocalQueue.size() << "\n";    
            std::vector<Event> cFrame(fSortWindow);
            for(size_t cIndx=0;cIndx<fSortWindow;cIndx++){ 
                cFrame[cIndx]=cLocalQueue.front();
                cLocalQueue.pop();
            }
            std::sort( cFrame.begin(), cFrame.end() );
            for(size_t cIndx=0; cIndx<fSortWindow;cIndx++)
            {
                if(cIndx<fSortWindow/2) fQueue.push(cFrame[cIndx]);
                else cLocalQueue.push(cFrame[cIndx]);
            }
        }
    }
    auto cSize=cLocalQueue.size(); 
    std::vector<Event> cFrame(cSize);
    for(size_t cIndx=0;cIndx<cSize;cIndx++){ 
        cFrame[cIndx]=cLocalQueue.front();
        cLocalQueue.pop();
    }
    std::sort( cFrame.begin(), cFrame.end() );
    for(size_t cIndx=0; cIndx<cFrame.size();cIndx++)
    {
        fQueue.push(cFrame[cIndx]);
    }
    fReadIsDone=true;
    fReadTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - fReadTime;
    std::cout << "Done reading.." << fReadCounter << " events from input file\n";
}
// processing data in sorted event queue 
void InputHandler::process()
{
    fProcessing=true;
    fProcessedCounter=0;
    fProcessTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    while(fQueue.size() == 0){}
    
    while(fReadIsDone==0)
    {
        while(fQueue.size() < 100 && !fReadIsDone){}
        auto cSize = fQueue.size();
        // std::cout << "Have got " << cSize << " items to process\n";
        // std::stringstream cOutput;
        size_t cNProcessed=print(cSize,fOutputStream);
        // std::cout << cOutput.str();
        fProcessedCounter+=cNProcessed;
        // std::cout << "... processed " << cNProcessed << "\n";
    }
    auto cSize=fQueue.size(); 
    std::cout << cSize << " items left to process\n";
    // std::stringstream cOutput;
    fProcessedCounter += print(cSize,fOutputStream);
    // std::cout << cOutput.str();
    fProcessTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - fProcessTime;
    std::cout << "Done processing.." << fProcessedCounter << " events from input file\n";
    fProcessing=false;
}
void InputHandler::ProcessData()
{
    fThProcess = std::async(std::launch::async, &InputHandler::process, this);
}
uint32_t InputHandler::print(size_t pSize, std::ostream& pOs)
{
    uint32_t cLastTimer=0;
    size_t cNProcessed=0;
    if(pSize==0) return cNProcessed;
    
    std::vector<uint64_t> cWords(pSize);
    for(size_t cIndx=0; cIndx<pSize; cIndx++){ 
        auto cEvent = fQueue.pop();
        // pOs << cEvent.fTimestamp << "\t" << cEvent.fEnergy << "\n";
        uint64_t cWrd = ((uint64_t)(cEvent.fEnergy) << 32) | (uint64_t)cEvent.fTimestamp;
        // pOs << std::bitset<64>(cWrd) << "\n";
        cWords.push_back(cWrd);
        uint32_t cTimeStamp = cEvent.fTimestamp;
        double cTime = (double)cTimeStamp;
        if( cTime - (double)cLastTimer < 0 ) std::cout << "!!! --ve \t" << cTimeStamp << "\t" << cLastTimer << "\t" << cIndx << "\n";
        cLastTimer=cTimeStamp;
        cNProcessed++;
    }
    pOs.write((char*)&cWords.at(0), cWords.size() * sizeof(uint64_t));   
    return cNProcessed;
}
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
    auto cStartTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    
    // readFile();
    this->ReadFile();
    this->ProcessData();
    this->Wait();
    cStartTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - cStartTime;
    fOutputStream.close();

    std::cout << "Sort window set to " << fSortWindow << "\n";
    std::cout << "Total running time " << cStartTime*1e-6 << " seconds\n";
    std::cout << "It took " << fReadTime*1e-6 << " s to read " << fReadCounter*1e-6 
        << " MEvents [" << std::scientific << std::setprecision(1) << (float)fReadCounter/fReadTime << " MEvents/s]\n";
    std::cout << "It took " << fProcessTime*1e-6 << " s to process " << fProcessedCounter*1e-6 
        << " MEvents [" << std::scientific << std::setprecision(1) << (float)fProcessedCounter/fProcessTime << " MEvents/s]\n";
}