#ifndef MYAUDIO_H
#define MYAUDIO_H

#include <QAudioDeviceInfo> //查询音频设备
#include <QAudioInput> //音频输入
#include <QAudioFormat> //音频存储
#include <QFile>
#include <QUdpSocket>
#include <QHostAddress>


class MyAudio:public QObject
{
    Q_OBJECT
public:
    void startAudio();
    void stopAudio();
private:
    QString audioFilePath; //录音结果文件
    QAudioInput *audioInput;// 录音输出文件
    QFile * outFile;// 保存录音
};

#endif // MYAUDIO_H
