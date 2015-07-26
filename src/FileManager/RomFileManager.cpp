#include "RomFileManager.h"
#include "File.h"

RomFileManager_t::RomFileManager_t()
{
    m_pDataPointer = NULL;
}

RomFileManager_t::~RomFileManager_t()
{
    Unload();
}

uint32_t RomFileManager_t::GetRomFileChs()
{
    return 0;
}

bool RomFileManager_t::Load( std::string& filePath )
{
    bool status = false;
    
    m_pGameFileName = filePath;

    File_t file;
    
    bool isOpened = file.OpenForRead( filePath );
            
    if( true == isOpened )
    {
        uint32_t fileSize = file.GetSize();
        
        if( NULL != m_pDataPointer )
        {
            delete m_pDataPointer;
        }

        m_pDataPointer = new uint8_t [ fileSize ];
        
        if( NULL != m_pDataPointer )
        {
            file.Read( m_pDataPointer, fileSize );
        }
            
        file.Close();
    
        status = true;
    }
    
    return status;
}

std::string RomFileManager_t::GetFilePath()
{
    return m_pGameFileName;
}

uint8_t* RomFileManager_t::GetDataPointer()
{
    return m_pDataPointer;
}

void RomFileManager_t::Unload()
{
    if( NULL != m_pDataPointer )
    {
        delete[] m_pDataPointer;
        m_pDataPointer = NULL;
    }
}