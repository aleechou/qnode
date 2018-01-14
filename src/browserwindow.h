#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QWidget>
#include <QMap>
#include <QJsonValue>
#include <QJsonArray>
#include <QVariant>
#include <QDebug>

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

    Q_INVOKABLE void resize(int w, int h){
        return QWidget::resize(w,h);
    }

    Q_INVOKABLE void load(const QString & url) ;
    Q_INVOKABLE void onLoaded() ;

    Q_INVOKABLE void runScript(const QString & script) ;
    Q_INVOKABLE void runScriptInMainIsolate(const QString & script) ;


signals:
    void ready(QString, bool) ;

private:
    Ui::BrowserWindow *ui;
};


Q_DECLARE_METATYPE(BrowserWindow)


#endif // BROWSERWINDOW_H
