#include <QApplication>
#include <nan.h>
#include <node.h>
#include <QDebug>
#include <v8.h>
#include <uv.h>
#include <QAbstractEventDispatcher>
#include <QFile>
#include <iostream>
#include <QMetaMethod>
#include "common.h"
#include "qtobjectwrapper.h"
#include "browserwindow.h"


void messageOutputFilter(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // filter messages
    if( type==QtWarningMsg && (msg.indexOf("of object 'BrowserWindow' has no notify signal and is not constant")>=0
            || msg.indexOf("Remote debugging server started successfully. Try pointing a Chromium-based browser to")>=0
    ))
        return ;

    QByteArray localMsg = msg.toLocal8Bit();

    QString strMsg("");
    switch(type)
    {
    case QtDebugMsg:
        strMsg = QString("Debug:");
        break;
    case QtWarningMsg:
        strMsg = QString("Warning:");
        break;
    case QtCriticalMsg:
        strMsg = QString("Critical:");
        break;
    case QtFatalMsg:
        strMsg = QString("Fatal:");
        break;
    }

    // 设置输出信息格式
    QString strMessage = QString("[@%2] %1")
            .arg(localMsg.constData()).arg(context.line);

    // 输出信息
    std::cout << strMessage.toStdString().c_str() << std::endl << std::flush ;
}

#define ExportFunction(apiobj, jsname, cppname)                                         \
        apiobj->Set(Nan::New(jsname).ToLocalChecked(),                                  \
               Nan::New<v8::FunctionTemplate>(cppname)->GetFunction());



void jsReflect(const Nan::FunctionCallbackInfo<v8::Value>& args){
    v8::Isolate * isolate = args.GetIsolate() ;

    int typeId = QMetaType::type(qtstring(args[0]));
    if(QMetaType::UnknownType==typeId) {
        Throw("unknow qt class, you must call qRegisterMetaType first.")
        return ;
    }
    qDebug() << typeId ;
    const QMetaObject * metaObject = QMetaType(typeId).metaObject();

    qDebug() << metaObject ;

    QString output = "[\r\n" ;

    for(int i = 0; i < metaObject->methodCount(); ++i) {
        QMetaMethod metaMethod = metaObject->method(i) ;
        output+= "  {\r\n" ;
        output+= "    name:\"" + QString(metaMethod.name()) + "\",\r\n" ;
        output+= "    returnType:\"" + QString(metaMethod.typeName()) + "\",\r\n" ;

        output+= "    params:[\r\n" ;
        QList<QByteArray> parameterNames = metaMethod.parameterNames() ;
        QList<QByteArray> parameterTypes = metaMethod.parameterTypes() ;
        for(int p=0;p<metaMethod.parameterCount();p++){
            output+= "      {\r\n" ;
            output+= "        name: \"" +QString(parameterNames[p])+ "\",\r\n" ;
            output+= "        type: \"" +QString(parameterTypes[p])+ "\",\r\n" ;
            output+= "      },\r\n" ;

        }
        output+= "    ],\r\n" ;

        output+= "    signature:\"" + QString(metaMethod.methodSignature()) + "\",\r\n" ;
        output+= "  },\r\n" ;
    }

    output + "]" ;

    args.GetReturnValue().Set(v8string(output));
}

void jsReadQrc(const Nan::FunctionCallbackInfo<v8::Value>& args){

    v8::Isolate * isolate = args.GetIsolate() ;
    qDebug() << "..>" << qtstring(args.Data()->TypeOf(isolate)) ;

    QString qrcFilepath = qtstring(args[0]) ;
    qDebug() << qrcFilepath ;
    QFile file(qrcFilepath) ;
    if(!file.open(QFile::ReadOnly)){
        qDebug() << file.errorString() ;
        qDebug() << "file not exits," << qrcFilepath ;
        return ;
    }
    QByteArray content = file.readAll() ;
    file.close() ;


    args.GetReturnValue().Set(v8str(content.data())) ;
}


int argc = 0 ;

void Init(v8::Local<v8::Object> exports) {

    qInstallMessageHandler(messageOutputFilter) ;

    v8::Isolate * isolate = exports->GetIsolate() ;

    setMainIsolate(isolate) ;

//    exports->TypeOf()

    QApplication * a = new QApplication (argc, NULL) ;

    int console_port = (rand() % 50000)+10000 ;
    qputenv("QTWEBENGINE_REMOTE_DEBUGGING",QString("%1").arg(console_port).toStdString().c_str());
    exports->Set(v8str("consolePort"),v8int32(console_port)) ;

    qRegisterMetaType<BrowserWindow>() ;
    qRegisterMetaType<BrowserWindow*>();

    QtObjectWrapper::Init(exports);

    // event loop for qt
    uv_idle_t * uvidler = new uv_idle_t;
    uv_idle_init(uv_default_loop(), uvidler);
    uv_idle_start(uvidler, [](uv_idle_t*){
        QCoreApplication::instance()->eventDispatcher()->processEvents(QEventLoop::EventLoopExec) ;
    }) ;

    ExportFunction(exports, "reflect", jsReflect)
    ExportFunction(exports, "readQrc", jsReadQrc)
}



NODE_MODULE(qnode, Init)
