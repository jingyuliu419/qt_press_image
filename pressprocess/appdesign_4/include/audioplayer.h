#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QThread>
#include <QDebug>
#include <QFile>

class AudioPlayer : public QObject {
    Q_OBJECT
public:
    explicit AudioPlayer(QObject *parent = nullptr);

public slots:
    void playAudio(const QString &filePath); // 音频播放槽

private:
    QMediaPlayer *player; // 用于播放音频的播放器
};

#endif // AUDIOPLAYER_H
