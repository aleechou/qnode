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


BrowserWindow::BrowserWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BrowserWindow)
{
    q "BrowserWindow::BrowserWindow" ;
    ui->setupUi(this);

//    resize(1024,768);

//    // web channel
//    QWebChannel * channel = new QWebChannel(this);

//    channel->registerObject("$window", this);

//    ui->browser->page()->setWebChannel(channel);

//    QObject::connect(ui->browser->page(), &QWebEnginePage::loadFinished,[this](bool ok){
//        if(!ok) {
//            emit this->ready(QVariant(ok));
//        }

//        QString initConsts = QString(
//                    "$qnodeapi_console_port = %1;\r\n"
//                )
//                    .arg(QString(qgetenv("QTWEBENGINE_REMOTE_DEBUGGING"))) ;
//        ui->browser->page()->runJavaScript(initConsts);

//        // load boot.js
////        ui->browser->page()->runJavaScript(apiFs.readFile(":/sdk/webkit/boot.js")) ;
//    }) ;

    ui->browser->load(QUrl("http://www.baidu.com")) ;
}

void BrowserWindow::load(const QString & url) {
    q url ;
    ui->browser->load(QUrl(url)) ;
}

void BrowserWindow::onLoaded() {
    emit this->ready(QVariant(true)) ;
}


BrowserWindow::~BrowserWindow()
{
    qDebug() << "BrowserWindow::~BrowserWindow()" ;
    delete ui;
}

void BrowserWindow::runScript(const QString & script) {
//    ui->browser->page()->runJavaScript(script) ;
}

