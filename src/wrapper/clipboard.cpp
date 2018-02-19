#include "clipboard.h"
#include <QClipboard>
#include <QApplication>
#include <QDebug>

Clipboard::Clipboard(QObject *parent) : QObject(parent)
{

}

void Clipboard::setText(const QString & text) {
    QApplication::clipboard()->setText(text);
}
QString Clipboard::text() const {
    return QApplication::clipboard()->text();
}
