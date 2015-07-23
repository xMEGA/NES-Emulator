#include "FileManager.h"
#include <string.h>
#include "File.h"

#ifdef WIN32

#include <windows.h>
#include <stdio.h>
#include <commdlg.h>

void FileManager_t::SetDialogTitle( char* pTitle )
{
    swprintf( ( wchar_t * )m_DialogTitle, FILE_DIALOG_TITLE_MAX_SIZE, L"%S", pTitle);
}

void FileManager_t::SetDialogFilter( char* pTitle )
{
    swprintf( ( wchar_t * )m_DialogFilter, FILE_DIALOG_FILTER_MAX_SIZE, L"%S", pTitle);
}

FileStatus_t FileManager_t::BrowseAndLoad( void )
{
    OPENFILENAMEW openFileName;
    FileStatus_t status = FILE_LOADED_SUCCESS_STATUS;
    uint32_t error;
    TCHAR sfile[MAX_PATH];
    
    ZeroMemory(&openFileName, sizeof(openFileName));
    ZeroMemory(sfile, sizeof(TCHAR)*MAX_PATH);

    openFileName.lStructSize = sizeof(openFileName);
    openFileName.hwndOwner            = NULL;
    openFileName.lpstrFile            = (LPWSTR)sfile;
    openFileName.nMaxFile            = MAX_PATH;
    
    //openFileName.lpstrFilter        = ( LPCWSTR )m_DialogFilter;//L"ROM images\0*.nes\0";  //( LPCWSTR )m_DialogFilter;

    openFileName.lpstrFilter        = L"ROM images\0*.nes\0";

    
    openFileName.nFilterIndex        = 1;
    openFileName.lpstrTitle            = ( LPCWSTR ) m_DialogTitle;
    openFileName.lpstrInitialDir    = NULL;
    openFileName.lpstrCustomFilter    = NULL;
    openFileName.nMaxCustFilter        = 0;
    openFileName.lpstrFileTitle        = NULL;
    openFileName.nMaxFileTitle        = 0;
    openFileName.nFileOffset        = 0;
    openFileName.nFileExtension        = 0;
    openFileName.lpstrDefExt        = NULL;
    openFileName.lCustData            = 0;
    openFileName.lpfnHook            = 0;
    openFileName.lpTemplateName        = 0;
    openFileName.Flags                = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
 
    GetOpenFileNameW(&openFileName);

    error = GetLastError();
        
    if( error )
    {
        status = FILE_NOT_FOUND_STATUS;
    }

    if( FILE_LOADED_SUCCESS_STATUS == status )
    {
        status = Load( (char *)openFileName.lpstrFile );
    }

    return status;
 }

FileStatus_t FileManager_t::Load( char *pPath )
{
    FileStatus_t  status = FILE_LOADED_SUCCESS_STATUS;
    HANDLE    m_DataFileHandle;
    uint32_t error;
    LPWSTR openFileName;
    openFileName = (LPWSTR)pPath;
    DWORD numberOfReadBytes = 0;
    DWORD m_RomSize;
    DWORD m_RomSizeH;

    do
    {    
        m_DataFileHandle = CreateFileW( openFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        
        if( INVALID_HANDLE_VALUE == m_DataFileHandle )
        {
            error  = GetLastError();
            status = FILE_NOT_FOUND_STATUS;
            break;
        }
        
        m_RomSize = GetFileSize( m_DataFileHandle, &m_RomSizeH );
            
       /* if( m_pDataPointer ) //!!!
        {
            delete[] m_pDataPointer;
        }*/

        m_pDataPointer = new uint8_t [ m_RomSize ];

        if( NULL == m_pDataPointer )
        {
            status = FILE_ERROR_ALLOCATE_MEMORY_STATUS;
            break;
        }
        
        if( false == ReadFile(m_DataFileHandle, m_pDataPointer, m_RomSize, &numberOfReadBytes, NULL) )
        {
            status = FILE_READ_ERROR_FILE_STATUS;
            break;
        }
        
        CloseHandle(m_DataFileHandle);
        error  = GetLastError();
    }while(false);

    return status;
}

uint8_t* FileManager_t::GetDataPointer(void)
{
    return m_pDataPointer;
}


void FileManager_t::Unload( void )
{
    if( m_pDataPointer )
    {
        delete[] m_pDataPointer;
        m_pDataPointer = NULL;
    }
}



#else

#include <stdio.h>

void FileManager_t::SetDialogTitle( char* pTitle )
{
   
}

void FileManager_t::SetDialogFilter( char* pTitle )
{
    
}
/*
FileStatus_t FileManager_t::ResetSaveFile()
{
    FileStatus_t status = FILE_NOT_FOUND_STATUS;

    uint32_t gameFileNameSize = strlen( m_pGameFileName ) + sizeof( char );
    
    char saveFileName[ gameFileNameSize ];
    
    memset( saveFileName, 0, sizeof( saveFileName ) );
    
    const char* saveFileExtension = "sav";
        
    memcpy( saveFileName, m_pGameFileName, strlen( m_pGameFileName ) );
    memcpy( &saveFileName[ gameFileNameSize - strlen( saveFileExtension ) - 1 ], saveFileExtension, strlen( saveFileExtension ) );
    
    FILE* pFile = fopen( saveFileName, "w" );
     
    if( NULL != pFile )
    {
        fwrite( "save", sizeof( uint8_t ), 5, pFile );
        
        fclose( pFile );
        
        status = FILE_SUCCESS_STATUS;
    }
    
    return status;
}

FileStatus_t FileManager_t::AppendToSaveFile( uint8_t* pDataBuff, uint32_t maxBufferSize )
{
    FileStatus_t status = FILE_SUCCESS_STATUS;

    return status;
}
 */

int GetFileSize(FILE *input)
{
    int fileSizeBytes;
    
    fseek(input, 0, SEEK_END); // set file pointer to end of file
    
    fileSizeBytes = ftell(input); // get current pointer position in bytes (== file size in bytes)
    
    fseek(input, 0, SEEK_SET); // return pointer to begin of file (it's very IMPORTANT)
    return fileSizeBytes;
}


FileStatus_t FileManager_t::BrowseAndLoad()
{
    FileStatus_t status = FILE_NOT_FOUND_STATUS;
        
    m_pGameFileName = "/home/xmega/Bitbucket/NES_Hardware/resources/Games/BalloonFight.nes";
         
    File_t file;
    
    bool isOpened = file.OpenForRead( m_pGameFileName );
            
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

FileStatus_t FileManager_t::Load( char *pPath )
{
    return FILE_SUCCESS_STATUS;
}

uint8_t* FileManager_t::GetDataPointer(void)
{
    return m_pDataPointer;
}

 
const char* FileManager_t::GetFilePath()
{
    return m_pGameFileName;
}
  

void FileManager_t::Unload( void )
{
   
}


#endif
