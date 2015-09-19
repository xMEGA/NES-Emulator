/*
    NES Emulator Project
    Created by Igor Belorus
    09/18/2015
*/

#ifndef _MACROS_H_
#define _MACROS_H_

#define ALIGN( expression ) __attribute__ ( ( aligned ( expression ) ) )
#define ALIGN_SIZE 8
#define UNUSED( expression ) do{ ( void )( expression ); } while ( 0 )

#endif
