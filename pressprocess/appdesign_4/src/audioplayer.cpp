#include "audioplayer.h"
#include <QEventLoop>
#include <QUrl>

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent), player(new QMediaPlayer(this)) {
    // 初始化播放器
    qDebug() << "AudioPlayer created in thread:" << QThread::currentThread();
}

void AudioPlayer::playAudio(const QString &filePath) {
    if (!player) {
        qWarning() << "Player is not initialized!";
        return;
    }

    if (player->state() == QMediaPlayer::PlayingState) {
        player->stop();  // 停止正在播放的音频
    }

    player->setMedia(QUrl::fromLocalFile(filePath));
    player->setVolume(100);
    player->play();
    qDebug() << "Audio playback started.";
}



