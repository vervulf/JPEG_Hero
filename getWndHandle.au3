#include <AutoItConstants.au3>

WinWait("[CLASS:Photo_Lightweight_Viewer]", "")
Local $hWnd = WinGetHandle("[CLASS:Photo_Lightweight_Viewer]")
FileDelete("viewer.wnd")
FileWrite ("viewer.wnd", $hwnd )