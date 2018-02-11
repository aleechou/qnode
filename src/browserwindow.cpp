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
#include <QWebEngineSettings>
#include <QResizeEvent>

unsigned int BrowserWindow::assignedWindowId = 0 ;


BrowserWindow::BrowserWindow(QWidget *parent) :
    QWidget(parent),
    windowId(assignedWindowId++)
{
    initUi() ;

    resize(1024,768);

    // web channel
    QWebChannel * channel = new QWebChannel(this);
    channel->registerObject("$window", this);
    ui->browser->page()->setWebChannel(channel);

    ui->browser->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    ui->browser->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    ui->browser->page()->settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);

    ui->browser->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    ui->browser->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    ui->browser->settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);

    QObject::connect(ui->browser->page(), &QWebEnginePage::loadFinished,[this](bool ok){

        QWebEnginePage * page = ui->browser->page() ;

        if(!ok) {
            emit this->ready(page->url().toString(), ok) ;
            return ;
        }

        page->runJavaScript(readFile(envVarValue("QNODE_SDKPATH")+"/browser/boot.js")) ;
    }) ;
}

void BrowserWindow::initUi() {
    ui = new Ui::BrowserWindow;
    ui->setupUi(this);
}

void BrowserWindow::loadScript(const QString & url) {
    ui->browser->page()->runJavaScript(QString(
       "var script = document.createElement('script');"
       " script.src='%1';"
       " document.body.appendChild(script)"
    ).arg(url)) ;
}
void BrowserWindow::loadScript(const QString & url, unsigned int retData) {
    ui->browser->page()->runJavaScript(QString(
       "var script = document.createElement('script');"
       " script.src='%1';"
       " script.onload=function() { if($window) $window.emitScriptLoaded('%1', %2) } ;"
       " document.body.appendChild(script)"
    ).arg(url).arg(retData)) ;

}


void BrowserWindow::emitScriptLoaded(const QString & url, unsigned int retData) {
    emit this->scriptLoaded(url, retData);
}

QByteArray BrowserWindow::readFile(const QString & filepath) {
    QFile file(filepath) ;
    if(!file.open(QFile::ReadOnly)){
        qDebug() << "file not exits," << filepath ;
        return QByteArray() ;
    }
    QByteArray content = file.readAll() ;
    file.close() ;
    return content ;
}


QByteArray BrowserWindow::envVarValue(const QString & name) {
    return qgetenv(name.toStdString().c_str()) ;
}
void BrowserWindow::setEnvVarValue(const QString & name,const QByteArray & value) {
    qputenv(name.toStdString().c_str(),value) ;
}

void BrowserWindow::load(const QString & url) {
    ui->browser->load(QUrl(url)) ;
}

void BrowserWindow::onLoaded() {
    emit this->ready(ui->browser->page()->url().toString(), true) ;
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

void BrowserWindow::resizeEvent(QResizeEvent *event){
    ui->browser->resize(event->size());
    QWidget::resizeEvent(event) ;
}
