#include <qglobal.h>

#ifdef Q_OS_LINUX
#include "qxtwindowsystem_x11.cpp"
#endif
#ifdef Q_OS_MACOS
#include "qxtwindowsystem_mac.cpp"
#endif
#ifdef Q_OS_WIN
#include "qxtwindowsystem_win.cpp"
#endif
