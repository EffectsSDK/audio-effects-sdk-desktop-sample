#ifndef CORE_AUDIO_BUFFER_QIODEVICE_H
#define CORE_AUDIO_BUFFER_QIODEVICE_H

#include <QIODevice>
#include <QAudioBuffer>

#include <mutex>

namespace core {

class AudioBufferQIODevice : public QIODevice
{
    Q_OBJECT
public:
    AudioBufferQIODevice();
    ~AudioBufferQIODevice();

    void push(const QAudioBuffer& buffer);

    bool isSequential() const override;
    qint64 bytesAvailable() const override;

protected:
    qint64 readData(char* data, qint64 maxLen) override;
    qint64 writeData(const char* data, qint64 len) override;

private:
    bool isCurrentBufferEmpty() const;
    qint64 readCurrentBuffer(char* data, qint64 maxLen);

private:
    mutable std::mutex _mutex;
    QList<QAudioBuffer> _audioBufferQueue;

    QAudioBuffer _currentBuffer;
    qsizetype _currentBufferPos;
    std::atomic<qint64> _bytesAvailable;
};

} // namespace core

#endif // CORE_AUDIO_BUFFER_QIODEVICE_H
