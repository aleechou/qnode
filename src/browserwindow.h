#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QWidget>
#include <QMap>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariant>
#include <QDebug>
#include "qtobjectwrapper.h"

namespace Ui {
class BrowserWindow;
}

class BrowserWindow : public QWidget
{
    Q_OBJECT

public:
    BrowserWindow(const BrowserWindow&):QWidget(0)  {}

    Q_INVOKABLE BrowserWindow(QWidget *parent = 0);
    ~BrowserWindow();

    Q_INVOKABLE void move(int x, int y) ;
    Q_INVOKABLE int width() const ;
    Q_INVOKABLE int height() const ;
    Q_INVOKABLE void resize(int w, int h){
        return QWidget::resize(w,h);
    }
    Q_INVOKABLE void setWindowFlags(uint type);
    Q_INVOKABLE uint windowFlags();

    Q_INVOKABLE QJsonObject desktop() ;

    Q_INVOKABLE void load(const QString &) ;
    Q_INVOKABLE void onLoaded() ;
    Q_INVOKABLE void loadScript(const QString &) ;
    Q_INVOKABLE void loadScript(const QString &, unsigned int) ;
    Q_INVOKABLE void runScript(const QString &) ;
    Q_INVOKABLE void runScriptInMainIsolate(const QString &) ;
    Q_INVOKABLE QByteArray readFile(const QString &) ;
    Q_INVOKABLE QByteArray envVarValue(const QString &) ;
    Q_INVOKABLE void setEnvVarValue(const QString &,const QByteArray &) ;
    Q_INVOKABLE unsigned int nativeWindowId() { return windowId ;}

    Q_INVOKABLE QObject* createQtObject(const QString & className) ;
    Q_INVOKABLE void nativeBridgeQtObject(QObject * object, const QString & receivedFuncCode) ;
    Q_INVOKABLE QObject* _bridgingQtObject(){ return m_bridgingQtObject ;}

    Q_INVOKABLE void emitScriptLoaded(const QString &, unsigned int) ;

signals:
    void ready(QString, bool) ;
    void scriptLoaded(const QString & url, unsigned int retData) ;

protected:
    virtual void initUi() ;
    Ui::BrowserWindow *ui;

    virtual void resizeEvent(QResizeEvent *event);
private:
    static unsigned int assignedWindowId ;
    unsigned int windowId ;
    QObject * m_bridgingQtObject = nullptr ;
};


Q_DECLARE_METATYPE(BrowserWindow)


#endif // BROWSERWINDOW_H
