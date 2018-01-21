#include <qglobal.h>

#ifdef Q_OS_LINUX
#include "qxtglobalshortcut_x11.cpp"
#endif
#ifdef Q_OS_MACOS
#include "qxtglobalshortcut_mac.cpp"
#endif
#ifdef Q_OS_WIN
#include "qxtglobalshortcut_win.cpp"
#endif
