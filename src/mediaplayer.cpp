#include "mediaplayer.h"
#include <QDebug>

MediaPlayer::MediaPlayer(QObject *parent) : QMediaPlayer(parent)
{
}

void MediaPlayer::setMedia(const QString & url) {
    QMediaPlayer::setMedia(QUrl(url));
}
