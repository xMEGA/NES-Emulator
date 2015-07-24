/*
    NES Emulator Project
    Created by Igor Belorus
    07/23/2015
*/

#ifndef _FILE_OPEN_DIALOG_H_
#define _FILE_OPEN_DIALOG_H_

#include <stdint.h>
#include <string>
#include <stddef.h>

class FileOpenDialog_t
{
public:
    std::string Browse( std::string title, std::string filter );

};
#endif //_FILE_OPEN_DIALOG_H_