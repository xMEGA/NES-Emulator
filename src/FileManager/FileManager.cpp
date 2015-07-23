#include "FileManager.h"
#include "File.h"

FileManager_t::FileManager_t()
{
    m_pDataPointer = NULL;
}

FileManager_t::~FileManager_t()
{
    Unload();
}

FileStatus_t FileManager_t::Load( std::string& filePath )
{
    FileStatus_t status = FILE_NOT_FOUND_STATUS;
          
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
    
        status = FILE_SUCCESS_STATUS;
    }
    
    return status;
}

std::string FileManager_t::GetFilePath()
{
    return m_pGameFileName;
}

uint8_t* FileManager_t::GetDataPointer()
{
    return m_pDataPointer;
}

void FileManager_t::Unload()
{
    if( NULL != m_pDataPointer )
    {
        delete[] m_pDataPointer;
        m_pDataPointer = NULL;
    }
}