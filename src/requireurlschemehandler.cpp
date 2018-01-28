#include "requireurlschemehandler.h"
#include <QWebEngineUrlRequestJob>
#include <QBuffer>
#include <QDebug>

RequireUrlSchemeHandler::RequireUrlSchemeHandler(QObject *parent)
    : QWebEngineUrlSchemeHandler(parent)
{

}

void RequireUrlSchemeHandler::requestStarted(QWebEngineUrlRequestJob *request){
    QBuffer *buffer = new QBuffer;
    connect(request, SIGNAL(destroyed()), buffer, SLOT(deleteLater()));

    qDebug() << request->requestUrl() ;

    QString script = QString("requireForBrowser(\"%1\")").arg(request->requestUrl().toString()) ;
    qDebug() << script ;

    v8::Isolate * isolate = mainIsolate() ;
    v8::HandleScope scope(isolate);
    v8::Local<v8::Value> rs = v8::Script::Compile ( v8string(script) )->Run();

    qDebug() << qtstring(rs) ;

    buffer->open(QIODevice::WriteOnly);
    buffer->write(qtstring(rs));
    buffer->close();

    request->reply("application/javascript", buffer);
}
