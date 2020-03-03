// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

extern bool g_Skip_wpad;
extern std::wofstream Log;

static const char* configFile = "./config.ini";

void init_log (const char* logFile)
{
	if (1 == GetPrivateProfileInt ("Config", "Log", 0, configFile)) {
		Log.open (logFile, std::ios::out | std::ios::app);
		Log << __TIMESTAMP__ << '\n';
	}
}

BOOL APIENTRY DllMain (HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
)
{
	DisableThreadLibraryCalls (hModule);
	std::string procname (GetCommandLine ());
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		// block ads request - main process
		if (std::string::npos == procname.find ("--type=")) {
			InstallHookApi ("Winhttp.dll", "WinHttpOpenRequest", winhttpopenrequesthook);
			init_log ("main_log.txt");
			//MessageBox (NULL, procname.c_str(), "Command line", 0);
		}
		else if (std::string::npos != procname.find ("--type=utility")) {
			// block ads banner by hostname - utility process
			InstallHookApi ("ws2_32.dll", "getaddrinfo", getaddrinfohook);
			init_log ("utility_log.txt");
			//MessageBox (NULL, procname.c_str (), "Command line", 0);
			if (1 == GetPrivateProfileInt ("Config", "Skip_wpad", 0, configFile))
				g_Skip_wpad = true;
		}
		break;
	case DLL_PROCESS_DETACH:
		if (Log.is_open ()) {
			Log.flush ();
			Log.close ();
		}
		break;
	}
	return TRUE;
}

