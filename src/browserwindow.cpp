#include "ui_browserwindow.h"
#include <QUrl>
#include <QCoreApplication>
#include "browserwindow.h"
#include "common.h"
#include <QMetaObject>
#include <QFile>
#include <QDesktopServices>
#include <QWebChannel>
#include <QWebEnginePage>


QByteArray readFile(const QString & filepath) {
    QFile file(filepath) ;
    if(!file.open(QFile::ReadOnly)){
        qDebug() << "file not exits," << filepath ;
        return QByteArray() ;
    }
    QByteArray content = file.readAll() ;
    file.close() ;
    return content ;
}

BrowserWindow::BrowserWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BrowserWindow)
{
    ui->setupUi(this);

    resize(1024,768);

    // web channel
    QWebChannel * channel = new QWebChannel(this);
    channel->registerObject("$window", this);
    ui->browser->page()->setWebChannel(channel);

    QObject::connect(ui->browser->page(), &QWebEnginePage::loadFinished,[this](bool ok){

        QWebEnginePage * page = ui->browser->page() ;

        page->runJavaScript(readFile(":/qtwebchannel/qwebchannel.js")) ;
//        page->runJavaScript(apiFs.readFile(":/sdk/webkit/require.js")) ;

        page->runJavaScript(QString("new QWebChannel(qt.webChannelTransport, function(channel) {;"
        "    for (var name in channel.objects)"
        "        window[name] = channel.objects[name];"
        "})")) ;

        emit this->ready(ok) ;
    }) ;

}



void BrowserWindow::load(const QString & url) {
    ui->browser->load(QUrl(url)) ;
}

void BrowserWindow::onLoaded() {
    emit this->ready(QVariant(true)) ;
}


BrowserWindow::~BrowserWindow()
{
    delete ui;
}

void BrowserWindow::runScript(const QString & script) {
    ui->browser->page()->runJavaScript(script) ;
}

void BrowserWindow::runScriptInMainIsolate(const QString & script) {
    runNodeScript(script);
}
