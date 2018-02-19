#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <QObject>

class Clipboard : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit Clipboard(QObject *parent = nullptr);

    Q_INVOKABLE void setText(const QString &) ;
    Q_INVOKABLE QString text() const ;

signals:

public slots:
};

#endif // CLIPBOARD_H
