#include "FileOpenDialog.h"
#include <stdio.h>

#ifdef WIN32

#include <windows.h>
#include <stdio.h>
#include <commdlg.h>

std::string FileOpenDialog_t::Browse( std::string title, std::string filter )
{
    OPENFILENAMEW openFileName;
	   
    TCHAR sfile[ MAX_PATH ];
    
    ZeroMemory( &openFileName, sizeof( openFileName ) );
    ZeroMemory( sfile, sizeof( TCHAR ) * MAX_PATH );

    openFileName.lStructSize = sizeof(openFileName);
    openFileName.hwndOwner   = NULL;
    openFileName.lpstrFile   = (LPWSTR)sfile;
    openFileName.nMaxFile    = MAX_PATH;
    
    
    std::wstring wstrTitle = std::wstring( title.begin(), title.end() );
    //std::wstring wstrFilter = std::wstring( filter.begin(), filter.end() );
	
    openFileName.nFilterIndex       = 1;
    openFileName.lpstrTitle         = wstrTitle.c_str();
    openFileName.lpstrFilter        = L"ROM images\0*.nes\0";
    openFileName.lpstrInitialDir    = NULL;
    openFileName.lpstrCustomFilter  = NULL;
    openFileName.nMaxCustFilter     = 0;
    openFileName.lpstrFileTitle     = NULL;
    openFileName.nMaxFileTitle      = 0;
    openFileName.nFileOffset        = 0;
    openFileName.nFileExtension     = 0;
    openFileName.lpstrDefExt        = NULL;
    openFileName.lCustData          = 0;
    openFileName.lpfnHook           = 0;
    openFileName.lpTemplateName     = 0;
    openFileName.Flags              = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
 
    GetOpenFileNameW(&openFileName);

    uint32_t error = GetLastError();
        
    if( 0 != error )
    {
    }
	
    char filePathBuffer[ 256 ];
    memset( filePathBuffer, 0, sizeof( filePathBuffer) );
    wcstombs( filePathBuffer, openFileName.lpstrFile, sizeof( filePathBuffer) );

    return filePathBuffer;
 }


#else

std::string FileOpenDialog_t::Browse( std::string title, std::string filter )
{
    //UNUSED( title );
    //UNUSED( filter );

//    return "/home/xmega/Games/BalloonFight.nes";
   return "/home/xmega/Games/Super Mario Bros. (JU) [!].nes";   
  //  return "/home/xmega/Games/Battletoads (USA).nes";      
    
  //  return "/home/xmega/Games/Battletoads_&_Double_Dragon_-_The_Ultimate_Team_(U).nes";      
   //return "/home/xmega/Games/Felix_the_Cat_(U).nes";      
   //return "/home/xmega/Games/Darkwing_Duck_(U).nes";      
  
   // return "/home/xmega/Games/Contra.nes";  
    //return "/home/xmega/Games/Super_Contra_(J).nes";
  // return "/home/xmega/Games/Sky_Destroyer_(J).nes";
   
   //  return "/home/xmega/Games/Smurfs.nes";
   
}

#endif
