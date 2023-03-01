#include "FileIO.h"

FileIO::FileIO() : fOption('r'), fFileIsOpened(false)
{

}
bool FileIO::isFileOpen()
{
    std::lock_guard<std::mutex> cLock(fMemberMutex);
    return fFileIsOpened;
}
FileIO::~FileIO()
{
    
}
bool FileIO::Open(std::fstream& pIO)
{
    if(FileIO::IsOpen(pIO) == false)
    {
        std::lock_guard<std::mutex> cLock(fMemberMutex);

        if(fOption == 'w')
        {
            pIO.open(fFileId.c_str(), std::fstream::out | std::fstream::binary );

        }
        else if(fOption == 'r')
        {
            pIO.open(fFileId.c_str(), std::fstream::in | std::fstream::binary);
        }

        fFileIsOpened = true;
        if(fFileIsOpened) std::cout << "Opened file " << fFileId.c_str() << "\n";
        return fFileIsOpened;

        if(fOption=='r')
        {
            pIO.clear();
            pIO.seekg(0, std::ios::beg);
        }
    }
    else std::cout << fFileId.c_str() << " is already open\n";
    return fFileIsOpened;
}
void FileIO::Close(std::fstream& pIO)
{
    if(fFileIsOpened == true)
    {
        fFileIsOpened = false;
        std::cout << "Closing " << fFileId.c_str() << "\n";
        pIO.close();
    }
}