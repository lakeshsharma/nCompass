// dllmain.cpp : Defines the entry point for the DLL application.
#include	"stdafx.h"
#include	"S5dll_api.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_DETACH:	// 0
		S5_DLL_LogMsg("%s(): PROCESS_DETACH\n",__FUNCTION__);
		break;
	case DLL_PROCESS_ATTACH:	// 1
		S5_DLL_LogMsg("%s(): PROCESS_ATTACH\n",__FUNCTION__);
		break;
	case DLL_THREAD_ATTACH:		// 2
		S5_DLL_LogMsg("%s(): THREAD_ATTACH\n",__FUNCTION__);
		break;
	case DLL_THREAD_DETACH:		// 3
		S5_DLL_LogMsg("%s(): THREAD_DETACH\n",__FUNCTION__);
		break;
	}
	return TRUE;
}
