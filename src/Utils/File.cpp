#include "File.h"
#include <stdio.h>

bool File_t::OpenForWrite( std::string filePath )
{
    bool status = false;
    
    m_pFile = fopen( filePath.c_str(), "w" );
          
    if( NULL != m_pFile )
    {
        status = true;
    }
    
    return status;
}

bool File_t::OpenForRead( std::string filePath )
{
    bool status = false;
    
    m_pFile = fopen( filePath.c_str(), "r" );
          
    if( NULL != m_pFile )
    {
        status = true;
    }
    
    return status;
}

bool File_t::OpenForAppend( std::string filePath )
{
    bool status = false;
    
    m_pFile = fopen( filePath.c_str(), "w+" );
          
    if( NULL != m_pFile )
    {
        status = true;
    }
    
    return status;
}
 
void File_t::Close()
{
    if( NULL != m_pFile )
    {
        fclose( static_cast< FILE* >( m_pFile ) );
        m_pFile = NULL;
    }
}

size_t File_t::Read( uint8_t* pBuffer, size_t maxBufferSize )
{
    FILE* pFile = static_cast< FILE* >( m_pFile );

    return fread( pBuffer, sizeof( uint8_t ), maxBufferSize, pFile );
}

bool File_t::Write( const uint8_t* pBuffer, size_t bytesCnt )
{
    FILE* pFile = static_cast< FILE* >( m_pFile );
    
    size_t len = bytesCnt;
    
    do
    {
        len = fwrite( pBuffer, sizeof( uint8_t ), len, pFile );

        pBuffer += len;
    
        bytesCnt -= len;

    }while( bytesCnt > 0 );

    return true;
}

bool File_t::Write( const char* pBuffer, size_t bytesCnt )
{
    return Write( ( uint8_t* )pBuffer, bytesCnt );
}

size_t File_t::GetSize()
{
    FILE* pFile = static_cast< FILE* >( m_pFile );
    
    fseek( pFile, 0, SEEK_END );
    
    int fileSizeBytes = ftell( pFile );
    
    fseek( pFile, 0, SEEK_SET );
    
    return fileSizeBytes;
}