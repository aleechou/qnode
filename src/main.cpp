#include "widget.h"
#include <QApplication>
#include <nan.h>
#include <node.h>
#include <QDebug>
#include <v8.h>
#include <uv.h>

//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    Widget w;
//    w.show();

//    return a.exec();
//}

#define ExportFunction(apiobj, jsname, cppname)                                         \
        apiobj->Set(Nan::New(jsname).ToLocalChecked(),                                  \
               Nan::New<v8::FunctionTemplate>(cppname)->GetFunction());


void jsHello(const Nan::FunctionCallbackInfo<v8::Value>& args){
    qDebug() << "hello" ;
}

void Init(v8::Local<v8::Object> exports) {

    int argv = 0 ;
    QApplication * a = new QApplication (argv, nullptr) ;

    Widget * w = new Widget();
    w->show();


    ExportFunction(exports, "hello", jsHello)
}

NODE_MODULE(qnode, Init)
