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

BrowserWindowCreator * BrowserWindowCreator::global=nullptr ;

BrowserWindowCreator::BrowserWindowCreator() {
    moveToThread(QCoreApplication::instance()->thread()) ;
    _id = ScriptObjects::registerScriptObject(this) ;
}
unsigned int BrowserWindowCreator::createBrowserWindow(unsigned int parentId){
    BrowserWindow * window = new BrowserWindow(parentId);
    return window->id() ;
}


BrowserWindowCreator * BrowserWindowCreator::singleton() {
    if(BrowserWindowCreator::global==nullptr) {
        BrowserWindowCreator::global = new BrowserWindowCreator() ;
    }
    return BrowserWindowCreator::global ;
}


BrowserWindow::BrowserWindow(unsigned int parentObjectId, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BrowserWindow),
    parentObjectId(parentObjectId)
{
    ui->setupUi(this);
    objectId = ScriptObjects::registerScriptObject(this) ;

    resize(1024,768);

    // web channel
    QWebChannel * channel = new QWebChannel(this);
    channel->registerObject("$window", this);
    channel->registerObject("$qnodeapi_script_objects", ScriptObjects::singleton());
    channel->registerObject("$qnodeapi_browser_window_creator", BrowserWindowCreator::singleton());
    ui->browser->page()->setWebChannel(channel);

    QObject::connect(ui->browser->page(), &QWebEnginePage::loadFinished,[this](bool ok){
        if(!ok) {
            emit this->ready(QVariant(ok));
        }

        QString initConsts = QString(
                    "$qnodeapi_thread_id = %1;\r\n"
                    "$qnodeapi_parent_thread_id = %2;\r\n"
                    "$qnodeapi_console_port = %3;\r\n"
                    "$qnodeapi_instance_id = %4;\r\n"
                )
                    .arg(this->objectId)
                    .arg(this->parentObjectId)
                    .arg(QString(qgetenv("QTWEBENGINE_REMOTE_DEBUGGING")))
                    .arg(webpageInstanceId++) ;
        ui->browser->page()->runJavaScript(initConsts);

        // load boot.js
        ui->browser->page()->runJavaScript(apiFs.readFile(":/sdk/webkit/boot.js")) ;
    }) ;
}

void BrowserWindow::onLoaded() {
    emit this->ready(QVariant(true)) ;
}

void BrowserWindow::runScriptInThread(unsigned int threadId, const QString & script) {
    QObject * parentThread = ScriptObjects::queryScriptObjectById(threadId) ;
    if(!parentThread) {
        return ;
    }
    bool ok = QMetaObject::invokeMethod(parentThread, "runScript", Qt::QueuedConnection, Q_ARG(QString, script)) ;
    if(!ok) {
        qCritical() << "can not QMetaObject::invokeMethod() runScript(QString) , for" << parentThread  ;
    }
}

BrowserWindow::~BrowserWindow()
{
    delete ui;
}

void BrowserWindow::load(const QString & url) {
    ui->browser->load(QUrl(url)) ;
}

void BrowserWindow::loadScript(const QString & url) {
    ui->browser->page()->runJavaScript(QString(
       "var script = document.createElement('script');"
       " script.src='%1';"
       " document.body.appendChild(script)"
    ).arg(url)) ;
}
void BrowserWindow::loadScript(const QString & url, unsigned int data) {
    ui->browser->page()->runJavaScript(QString(
       "var script = document.createElement('script');"
       " script.src='%1';"
       " script.onload=function() { if($window) $window.emitScriptLoaded('%1', %2) } ;"
       " document.body.appendChild(script)"
    ).arg(url).arg(data)) ;
}

void BrowserWindow::runScript(const QString & script) {
    ui->browser->page()->runJavaScript(script) ;
}

bool BWApiFs::exists(const QString & filepath) {
    return false ;
}

QByteArray BWApiFs::readFile(const QString & filepath) {
    QFile file(filepath) ;
    if(!file.open(QFile::ReadOnly)){
        qDebug() << "file not exits," << filepath ;
        return QByteArray() ;
    }
    QByteArray content = file.readAll() ;
    file.close() ;
    return content ;
}
