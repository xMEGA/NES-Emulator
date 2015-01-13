/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/
#include  "Emulator.h"

#ifdef WIN32
#include <windows.h>
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow ) //For VS2012
#else
int main()
#endif
{
    Emulator_t* pEmulator = new Emulator_t;

	if( NULL != pEmulator )
	{
		pEmulator->Init();

		bool isExit = false;

		while( false == isExit )
		{
			isExit = pEmulator->Run();
		}

		delete pEmulator;
	}
}