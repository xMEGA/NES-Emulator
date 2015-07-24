/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _FileManager_h_
#define _FileManager_h_

#include <stdint.h>
#include <string>

enum FileStatus_t
{
    FILE_SUCCESS_STATUS = 0,
    FILE_NOT_FOUND_STATUS,
    FILE_NOT_OPENED_STATUS,
    FILE_GET_FILE_SIZE_ERROR_STATUS,
    FILE_ERROR_ALLOCATE_MEMORY_STATUS,
    FILE_READ_ERROR_FILE_STATUS,
};

class FileManager_t
{
public:
	FileManager_t();
	~FileManager_t();
	
    std::string GetFilePath();
 
    FileStatus_t Load( std::string& filePath );
    void Unload();
    uint8_t* GetDataPointer();

private:
    uint8_t*    m_pDataPointer;
    const char* m_pGameFileName;
};
#endif