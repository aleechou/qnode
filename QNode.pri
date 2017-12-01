SOURCES += \
    $$PWD/browserwindow.cpp \
    $$PWD/nodethread.cpp \
    $$PWD/nodeapi.cc \
    $$PWD/scriptobjects.cpp

HEADERS += \
    $$PWD/browserwindow.h \
    $$PWD/nodethread.h \
    $$PWD/nodeapi.h \
    $$PWD/common.h \
    $$PWD/scriptobjects.h

FORMS += \
    $$PWD/browserwindow.ui


INCLUDEPATH += $$PWD \
    $$PWD/deps/node/deps/v8/include \
    $$PWD/deps/node/deps/v8 \
    $$PWD/deps/node/deps/uv/include \
    $$PWD/deps/node/src

RESOURCES += \
    $$PWD/lib.qrc


#LIBS += -L"$$PWD/deps/node/out/Release" \
#    $$PWD/deps/node/out/Release/obj.target/libnode.a \
#    $$PWD/deps/node/out/Release/obj.target/deps/gtest/libgtest.a \
#    $$PWD/deps/node/out/Release/obj.target/deps/cares/libcares.a \
#    $$PWD/deps/node/out/Release/obj.target/deps/openssl/libopenssl.a \
#    $$PWD/deps/node/out/Release/obj.target/deps/zlib/libzlib.a \
#    $$PWD/deps/node/out/Release/obj.target/deps/http_parser/libhttp_parser.a \
#    $$PWD/deps/node/out/Release/obj.target/deps/uv/libuv.a \
#    $$PWD/deps/node/out/Release/obj.target/deps/v8/src/libv8_libplatform.a \
#    $$PWD/deps/node/out/Release/obj.target/deps/v8/src/libv8_base.a \
#    $$PWD/deps/node/out/Release/obj.target/deps/v8/src/libv8_libbase.a \
#    $$PWD/deps/node/out/Release/obj.target/deps/v8/src/libv8_nosnapshot.a \
#    $$PWD/deps/node/out/Release/obj.target/deps/v8/src/libv8_snapshot.a \
#    $$PWD/deps/node/out/Release/obj.target/deps/v8/src/libv8_libsampler.a \
#    $$PWD/deps/node/out/Release/obj.target/tools/icu/libicudata.a \
#    $$PWD/deps/node/out/Release/obj.target/tools/icu/libicuucx.a \
#    $$PWD/deps/node/out/Release/obj.target/tools/icu/libicustubdata.a \
#    $$PWD/deps/node/out/Release/obj.target/tools/icu/libicui18n.a

unix:macx {
    LIBS+= -L$$PWD/deps/node/out/Release -lnode.59
}else {
    LIBS+= -L$$PWD/deps/node/out/Release/lib.target -lnode
}

QMAKE_RPATHDIR+= $ORIGIN:$ORIGIN/lib



CONFIG(release, debug|release) {
    OUTPUT_DIR = "output/release"
}
else {
    TARGET = $${TARGET}d
    OUTPUT_DIR = "output/debug"
}

# for clang and gcc
QMAKE_CXXFLAGS_WARN_ON += -Wall -Wextra
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-function
QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
QMAKE_CXXFLAGS_WARN_ON += -fpermissive



