
void (*Winmon_LoadHook)(DWORD myPid, HWND dskWnd);
BOOL (*Winmon_Moved)(void);
BOOL (*Winmon_DeskWndPainted)(RECT *dskRt);
BOOL (*Winmon_DesktopChanged)(void);
void (*Winmon_UnloadHook)(void);

void LoadWinmonFunctions(HMODULE winmon)
{
	(FARPROC &)Winmon_LoadHook = ::GetProcAddress(winmon, "Winmon_LoadHook");
	(FARPROC &)Winmon_Moved = ::GetProcAddress(winmon, "Winmon_Moved");
	(FARPROC &)Winmon_DeskWndPainted = ::GetProcAddress(winmon, "Winmon_DeskWndPainted");
	(FARPROC &)Winmon_DesktopChanged = ::GetProcAddress(winmon, "Winmon_DesktopChanged");
	(FARPROC &)Winmon_UnloadHook = ::GetProcAddress(winmon, "Winmon_UnloadHook");
}

