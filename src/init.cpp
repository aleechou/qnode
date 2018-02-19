#include "init.h"
#include <iostream>
#include <QApplication>
#include <QDebug>
#include <QAbstractEventDispatcher>
#include <QFile>
#include <QMetaMethod>
#include <QWebEngineProfile>
#include "common.h"
#include "qtobjectwrapper.h"
#include "browserwindow.h"
#include "qxtglobalshortcut5/qxtglobalshortcut.h"
#include "requireurlschemehandler.h"
#include "wrapper/MediaPlayer.h"
#include "wrapper/SerialPort.h"
#include "QHotkey/qhotkey.h"


void messageOutputFilter(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // filter messages
    if( type==QtWarningMsg && (msg.indexOf("has no notify signal and is not constant, value updates in HTML will be broken!")>=0
            || msg.indexOf("Remote debugging server started successfully. Try pointing a Chromium-based browser to")>=0
    )) {
        return ;
    }

    QByteArray localMsg = msg.toLocal8Bit();

    QString strMsg("");
    switch((int)type)
    {
    case (int)QtDebugMsg:
        strMsg = QString("Debug:");
        break;
    case (int)QtWarningMsg:
        strMsg = QString("Warning:");
        break;
    case (int)QtCriticalMsg:
        strMsg = QString("Critical:");
        break;
    case (int)QtFatalMsg:
        strMsg = QString("Fatal:");
        break;
    }

    // 设置输出信息格式
    QString strMessage = QString("[%3@%2] %1")
            .arg(localMsg.constData()).arg(context.line).arg(context.file);

    // 输出信息
    std::cout << strMessage.toStdString().c_str() << std::endl << std::flush ;
}

#define ExportFunction(apiobj, jsname, cppname)                                         \
        apiobj->Set(Nan::New(jsname).ToLocalChecked(),                                  \
               Nan::New<v8::FunctionTemplate>(cppname)->GetFunction());



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


void jsQtClassMeta(const Nan::FunctionCallbackInfo<v8::Value>& args){

    QByteArray className = qtstring(args[0]) ;
    int typeId = QMetaType::type(className);
    if(QMetaType::UnknownType==typeId) {
        Throw( QString("unknow qt class `%1`, you must call qRegisterMetaType first.").arg(className.toStdString().c_str()).toStdString().c_str() )
        return ;
    }

    v8::Isolate * isolate = args.GetIsolate() ;
    const QMetaObject * metaObject = QMetaType(typeId).metaObject();
    args.GetReturnValue().Set(v8string(QtObjectWrapper::methodList(metaObject)));
}

void jsQtTypeId(const Nan::FunctionCallbackInfo<v8::Value>& args){
    v8::Isolate * isolate = args.GetIsolate() ;
    QByteArray className = qtstring(args[0]) ;
    int typeId = QMetaType::type(className);
    args.GetReturnValue().Set(v8int32(typeId));
}

void jsApplicationDirPath(const Nan::FunctionCallbackInfo<v8::Value>& args){
    v8::Isolate * isolate = args.GetIsolate() ;
    args.GetReturnValue().Set(v8string(QCoreApplication::applicationDirPath()));
}

void jsApplicationFilePath(const Nan::FunctionCallbackInfo<v8::Value>& args){
    v8::Isolate * isolate = args.GetIsolate() ;
    args.GetReturnValue().Set(v8string(QCoreApplication::applicationFilePath()));
}

int argc = 2 ;
const char * argv[2] = {"qnode", "--disable-web-security"} ;

void QNodeInit(v8::Local<v8::Object> exports) {

    qInstallMessageHandler(messageOutputFilter) ;

    v8::Isolate * isolate = exports->GetIsolate() ;
    setMainIsolate(isolate) ;

    new QApplication (argc, (char **)argv) ;

    int console_port = (rand() % 50000)+10000 ;
    qputenv("QTWEBENGINE_REMOTE_DEBUGGING",QString("%1").arg(console_port).toStdString().c_str());
    exports->Set(v8str("consolePort"),v8int32(console_port)) ;

    qRegisterMetaType<MediaPlayer*>();
    qRegisterMetaType<SerialPort*>();
    qRegisterMetaType<BrowserWindow*>();
    qRegisterMetaType<QxtGlobalShortcut*>();
    qRegisterMetaType<QHotkey*>();

    QtObjectWrapper::Init(exports);

    // event loop for qt
    uv_idle_t * uvidler = new uv_idle_t;
    uv_idle_init(uv_default_loop(), uvidler);
    uv_idle_start(uvidler, [](uv_idle_t*){
//        QCoreApplication::instance()->eventDispatcher()->processEvents(QEventLoop::AllEvents) ;
        QApplication::processEvents();
    }) ;

    // sheme "require://" for webengine page
    QWebEngineProfile * profile = QWebEngineProfile::defaultProfile() ;
    if (!profile->urlSchemeHandler("require")) {
        RequireUrlSchemeHandler * handler = new RequireUrlSchemeHandler(profile) ;
        profile->installUrlSchemeHandler("require", handler);
    }

    // export functions to nodejs
    ExportFunction(exports, "readQrc", jsReadQrc)
    ExportFunction(exports, "qtClassMeta", jsQtClassMeta)
    ExportFunction(exports, "qtTypeId", jsQtTypeId)
    ExportFunction(exports, "applicationDirPath", jsApplicationDirPath)
    ExportFunction(exports, "applicationFilePath", jsApplicationFilePath)
}



