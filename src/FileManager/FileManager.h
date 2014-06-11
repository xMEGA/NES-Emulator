/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _FileManager_h_
#define _FileManager_h_

#include <stdint.h>

#define FILE_DIALOG_TITLE_MAX_SIZE  128
#define FILE_DIALOG_FILTER_MAX_SIZE 128

enum FileLoaderStatus_t
{
    FILE_LOADED_SUCCESS_STATUS = 0,
    FILE_UNLOADED_SUCCESS_STATUS,
    FILE_NOT_FOUND_STATUS,
    FILE_NOT_OPENED_STATUS,
    FILE_GET_FILE_SIZE_UNSUCCESS_STATUS,
    FILE_ERROR_ALLOCATE_MEMORY_STATUS,
    FILE_READ_ERROR_FILE_STATUS,
};

class FileManager_t
{
public:
    void SetDialogTitle( char* pTitle );
    void SetDialogFilter( char* pTitle );
    FileLoaderStatus_t BrowseAndLoad(void);
    FileLoaderStatus_t Load( char *pPath );
    void Unload(void);
    uint8_t* GetDataPointer(void);

private:
    uint8_t*  m_pDataPointer;
    char      m_DialogTitle [ FILE_DIALOG_TITLE_MAX_SIZE  ];
    char      m_DialogFilter[ FILE_DIALOG_FILTER_MAX_SIZE ];
};
#endif