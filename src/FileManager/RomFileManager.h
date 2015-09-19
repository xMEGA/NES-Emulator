/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _FileManager_h_
#define _FileManager_h_

#include <stdint.h>
#include <string>



class RomFileManager_t
{
public:
    RomFileManager_t();
    ~RomFileManager_t();
	
    std::string GetFilePath();
 
    bool Load( std::string& filePath );
    void Unload();
    uint8_t* GetDataPointer();
    uint32_t GetRomSize();
    uint32_t GetRomFileChs();

private:
    uint8_t*    m_pDataPointer;
    uint32_t    m_RomSize;
    std::string m_pGameFileName;
};
#endif