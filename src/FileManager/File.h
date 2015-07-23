/*
    NES Emulator Project
    Created by Igor Belorus
    07/13/2015
*/

#ifndef _FILE_H_
#define _FILE_H_

#include <stdint.h>
#include <string>
#include <stddef.h>

class File_t
{
public:
    bool OpenForWrite ( std::string filePath );
    bool OpenForRead  ( std::string filePath );
    bool OpenForAppend( std::string filePath );
    
    size_t Read( uint8_t* pBuffer, size_t maxBufferSize );
    size_t GetSize();

    
    void Close();
  
private:
    void* m_pFile;
};
#endif