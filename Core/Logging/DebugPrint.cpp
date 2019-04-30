#include "DebugPrint.h"

#include <windows.h>

namespace Baroque
{
    void DebugPrint(const char* text)
    {
        OutputDebugStringA(text);
    }

    void DebugPrintLine(const char* text)
    {
        DebugPrint(text);
        DebugPrint("\n");
    }
}