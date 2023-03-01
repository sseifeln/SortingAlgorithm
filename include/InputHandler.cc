#include "InputHandler.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <algorithm>
#include <iomanip>
#include <bitset>
#include "FileIO.cc"

InputHandler::InputHandler(const std::string& pInputFileName,  const std::string& pOutputFileName, size_t pReadLimit): fReadTime{0}, fProcessTime{0}, fReadCounter{0}, fProcessedCounter{0}, fReadIsDone{false}
{
    fDebugOut=false;
    fNCalls=0;
    fAverageQueueHandled=0;
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
    std::deque<uint64_t> cLclDQ;
    size_t cFrameCounter=0; 
    while( !cStopCondition )
    {
        uint64_t cWord=0;
        // first 64 bits --> header --> tells me how many events I have 
        fFileStream.read((char*)&cWord, sizeof(uint64_t));
        auto cFrameSize=cWord&0xFFFF;
        // push events into local dq
        std::vector<uint64_t> cEventData(cFrameSize,0);
        fFileStream.read((char*)&cEventData[0], cFrameSize*sizeof(uint64_t));
        cLclDQ.insert( cLclDQ.end(), cEventData.begin(), cEventData.end());
        fReadCounter+=cFrameSize;
        cFrameCounter++;

        cStopCondition = ( cFrameCounter >= fReadLimit && fReadLimit!=0 ) || fFileStream.eof(); 
        // every time I've accumulated the corect number of events in the local queue.. 
        // sort and push the first half into the processing queue
        if( cLclDQ.size() >= fSortWindow )
        {
            // sort based on the time-stamp
            sort( cLclDQ.begin( ), cLclDQ.end( ), [ ]( const uint64_t& lhs, const uint64_t& rhs )
            {
                return ((lhs&0xFFFFFFFF) < (rhs&0xFFFFFFFF));
            });
            // insert first half into the TS queue
            auto cElementsToRemove = cLclDQ.size()/2; 
            fQueue.insert( cLclDQ , cLclDQ.size()/2 );
            // remove items from the local queue
            cLclDQ.erase( cLclDQ.begin(), cLclDQ.begin() + cElementsToRemove); 
        }
    }
    // sort based on the time-stamp
    sort( cLclDQ.begin( ), cLclDQ.end( ), [ ]( const uint64_t& lhs, const uint64_t& rhs )
    {
        return ((lhs&0xFFFFFFFF) < (rhs&0xFFFFFFFF));
    });
    fQueue.insert( cLclDQ , cLclDQ.size() );
    // remove items from the local queue
    cLclDQ.erase( cLclDQ.begin(), cLclDQ.end());
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
    size_t cNProcessed=0;
    if(pSize==0) return cNProcessed;
    fNCalls++;
    fAverageQueueHandled += (double)pSize;
    std::vector<uint64_t> cMyData(0);
    fQueue.to_vector(cMyData, pSize);
    fQueue.erase( pSize);
    pOs.write((char*)&cMyData[0], pSize*sizeof(uint64_t)); 
    cNProcessed += cMyData.size();
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
    fIOHandlerInput->Close(fFileStream); 
    fIOHandlerOutput->Close(fOutputStream); 
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

    fAverageQueueHandled/=fNCalls;

    std::cout << "Sort window set to " << fSortWindow << "\n";
    std::cout << "Average number of sorted events sent out at a time <" << (uint32_t)fAverageQueueHandled << "> events over " << fNCalls << " writes\n";
    std::cout << "Total running time " << cStartTime*1e-6 << " seconds\n";
    std::cout << "It took " << fReadTime*1e-6 << " s to read " << fReadCounter*1e-6 
        << " MEvents [" << std::scientific << std::setprecision(1) << (float)fReadCounter/fReadTime << " MEvents/s]\n";
    std::cout << "It took " << fProcessTime*1e-6 << " s to process " << fProcessedCounter*1e-6 
        << " MEvents [" << std::scientific << std::setprecision(1) << (float)fProcessedCounter/fProcessTime << " MEvents/s]\n";
}
void InputHandler::ConvertRawOutput(const std::string& pFileName)
{
    std::cout << "Converting .raw output to tab separated file format\n";
    std::ofstream cFile;
    cFile.open(pFileName, std::ios::out );
    cFile << "Time\tEnergy\n";
    fIOHandlerInput->SetOption('r');
    fIOHandlerInput->SetFilename(fOutputFileName); 
    fIOHandlerInput->Open(fFileStream); 
    while(!fFileStream.eof())
    {
        std::stringstream cOutput; 
        for(size_t cChunk=0; cChunk<256;cChunk++)
        {
            if( fFileStream.eof() ) break; 

            uint64_t cWord=0;
            fFileStream.read((char*)&cWord, sizeof(uint64_t));
            if(cWord==0) continue;

            if( fDebugOut ) std::cout << cChunk << ":" << std::bitset<64>(cWord) << "\n";
            Event cEvent; 
            cEvent.decode(cWord); 
            cOutput << cEvent.fTimestamp << "\t" << cEvent.fEnergy << "\n";
        }
        cFile << cOutput.str(); 
    }
    fIOHandlerInput->Close(fFileStream);
    cFile.close();     
}