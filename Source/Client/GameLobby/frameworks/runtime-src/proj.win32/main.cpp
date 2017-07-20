#include "main.h"
#include "SimulatorWin.h"
#include <shellapi.h>

#define WIN32_USE_CONSOLE

int WINAPI _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef WIN32_USE_CONSOLE
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
#endif

    auto simulator = SimulatorWin::getInstance();
    return simulator->run();
}
