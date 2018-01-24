#-------------------------------------------------
#
# Project created by QtCreator 2017-12-06T17:15:46
#
#-------------------------------------------------

QT      += core gui widgets network webengine webenginewidgets network serialport gui-private

CONFIG += console

TEMPLATE = lib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


INCLUDEPATH +=  /usr/local/lib/node_modules/nan \
                $$PWD \
                $$PWD/build \               # for ui_widget.h
                $$PWD/deps/includes/v8 \
                $$PWD/deps/includes/uv \
                $$PWD/deps/includes/node



CONFIG(release, debug|release) {
    OUTPUT_DIR = "output/Release"
}
else {
    TARGET = $${TARGET}d
    OUTPUT_DIR = "output/Debug"
}

OBJECTS_DIR = $$OUTPUT_DIR/obj
MOC_DIR = $$OUTPUT_DIR/moc
RCC_DIR = $$OUTPUT_DIR/rcc




SOURCES += \
        $$PWD/src/browserwindow.cpp \
        $$PWD/src/common.cpp \
        $$PWD/src/qtobjectwrapper.cpp \
        $$PWD/src/qtsignalrouter.cc \
        $$PWD/src/init.cpp \
        $$PWD/src/qxtglobalshortcut5/qxtglobal.cpp \
        $$PWD/src/qxtglobalshortcut5/qxtwindowsystem.cpp \
        $$PWD/src/qxtglobalshortcut5/qxtglobalshortcut.cpp \
        $$PWD/src/qxtglobalshortcut5/qxtwindowsystemimp.cpp \
        $$PWD/src/qxtglobalshortcut5/qxtglobalshortcutimp.cpp

HEADERS += \
        $$PWD/src/browserwindow.h \
        $$PWD/src/common.h \
        $$PWD/src/qtobjectwrapper.h \
        $$PWD/src/init.h \
        $$PWD/src/qxtglobalshortcut5/qxtglobal.h \
        $$PWD/src/qxtglobalshortcut5/qxtglobalshortcut_p.h \
        $$PWD/src/qxtglobalshortcut5/qxtglobalshortcut.h \
        $$PWD/src/qxtglobalshortcut5/qxtwindowsystem.h

FORMS += \
        $$PWD/src/browserwindow.ui


# for QxtGlobalShortcut
unix:!macx:{ QT += x11extras }
DEFINES += BUILD_QXT_CORE \
    BUILD_QXT_GUI

unix:!macx {
    HEADERS += $$PWD/src/qxtglobalshortcut5/x11info.h
    SOURCES += \
            $$PWD/src/qxtglobalshortcut5/qxtwindowsystem_x11.cpp \
            $$PWD/src/qxtglobalshortcut5/qxtglobalshortcut_x11.cpp
    CONFIG+= X11
}
macx: {
    HEADERS  += $$PWD/src/qxtglobalshortcut5/qxtwindowsystem_mac.h
    QMAKE_LFLAGS += -framework Carbon -framework CoreFoundation
}

win32: {
}




QMAKE_RPATHDIR+= $ORIGIN:$ORIGIN/lib


# for clang and gcc
QMAKE_CXXFLAGS_WARN_ON += -Wall -Wextra
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-variable
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-function
QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
QMAKE_CXXFLAGS_WARN_ON += -fpermissive
