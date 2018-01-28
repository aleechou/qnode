#ifndef REQUIREURLSCHEMEHANDLER_H
#define REQUIREURLSCHEMEHANDLER_H

#include <QWebEngineUrlSchemeHandler>
#include "common.h"

class RequireUrlSchemeHandler : public QWebEngineUrlSchemeHandler
{
    Q_OBJECT
public:
    explicit RequireUrlSchemeHandler(QObject *parent = nullptr);
    void requestStarted(QWebEngineUrlRequestJob *request);

signals:

public slots:
};

#endif // REQUIREURLSCHEMEHANDLER_H
