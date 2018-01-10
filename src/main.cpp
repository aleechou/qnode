#include <QApplication>
#include <nan.h>
#include <node.h>
#include <QDebug>
#include <v8.h>
#include <uv.h>
#include <QAbstractEventDispatcher>
#include <iostream>
#include "common.h"
#include "qtobjectwrapper.h"
#include "browserwindow.h"


#define ExportFunction(apiobj, jsname, cppname)                                         \
        apiobj->Set(Nan::New(jsname).ToLocalChecked(),                                  \
               Nan::New<v8::FunctionTemplate>(cppname)->GetFunction());



void jsHello(const Nan::FunctionCallbackInfo<v8::Value>& args){

}



int argc = 0 ;

void Init(v8::Local<v8::Object> exports) {

    QApplication * a = new QApplication (argc, NULL) ;

    qputenv("QTWEBENGINE_REMOTE_DEBUGGING","18011");
    // qputenv("QTWEBENGINE_REMOTE_DEBUGGING",QString("%1").arg((rand() % 50000)+10000).toStdString().c_str());

    qRegisterMetaType<BrowserWindow>() ;
    qRegisterMetaType<BrowserWindow*>();

    QtObjectWrapper::Init(exports);

    // event loop for qt
    uv_idle_t * uvidler = new uv_idle_t;
    uv_idle_init(uv_default_loop(), uvidler);
    uv_idle_start(uvidler, [](uv_idle_t*){
        QCoreApplication::instance()->eventDispatcher()->processEvents(QEventLoop::EventLoopExec) ;
    }) ;

    ExportFunction(exports, "hello", jsHello)
}



NODE_MODULE(qnode, Init)
