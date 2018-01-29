#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QMediaPlayer>

class MediaPlayer : public QMediaPlayer
{
    Q_OBJECT
public:
    Q_INVOKABLE MediaPlayer(QObject *parent = nullptr);

signals:

public slots:
    void setMedia(const QString & url) ;
};

#endif // MEDIAPLAYER_H
