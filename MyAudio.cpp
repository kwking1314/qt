#include "MyAudio.h"
#include "PathDef.h"
#include <QDebug>

//#define AUDIO_PATH "C:/Users/hking123456/Desktop/shixun/aihome/audiofile.pcm"
#define AUDIO_PATH MODULE_PATH"/audiofile.pcm"

void MyAudio::startAudio()
{
    //用于检测音频设备状态信息
   QAudioDeviceInfo device = QAudioDeviceInfo::defaultInputDevice();
   /* 音频编码要求 */
   QAudioFormat m_format;
   m_format.setSampleRate(16000);
   m_format.setChannelCount(1);
   m_format.setSampleSize(16);
   m_format.setCodec("audio/pcm"); // 使用PCM编码
   m_format.setByteOrder(QAudioFormat::LittleEndian); // 字节序
   m_format.setSampleType(QAudioFormat::SignedInt);
   if(!device.isFormatSupported(m_format))
   {
       /* 寻找最接近的格式 */
       m_format = device.nearestFormat(m_format);
   }
   //设置文件路径
   audioFilePath = AUDIO_PATH;
   // 创建文件
   outFile = new QFile;
   outFile->setFileName(audioFilePath);
   outFile->open(QIODevice::WriteOnly);
   /* 创建录音对象 */
   audioInput = new QAudioInput(m_format,this);
   audioInput->start(outFile);
   qDebug() << "Recording start.";
}

void MyAudio::stopAudio()
{
    /* 停止录音 */
    audioInput->stop();
    /* 关闭文件 */
    outFile->close();
    /* 删除文件对象指针并置空 */
    delete outFile;
    outFile = nullptr;
    qDebug() << "Recording stopped.";
}
