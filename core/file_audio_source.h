#ifndef CORE_FILE_AUDIO_SOURCE_H
#define CORE_FILE_AUDIO_SOURCE_H

#include "abstract_audio_source.h"

#include <QAudioFormat>
#include <QAudioDecoder>
#include <QFile>

namespace core {

class AudioBufferQIODevice;

// Doesn't work on macOS as QAudioDecoder is not implemented there!
class FileAudioSource : public AbstractAudioSource
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(FileAudioSource)
public:
    FileAudioSource(QObject * parent = nullptr);
    ~FileAudioSource();

    void setSourceFilePath(const QString& path);
    void setFormat(const QAudioFormat& format);

    QIODevice* start() override;

    int durationSeconds() const override;

private:
    Q_SLOT void onBufferReady();
    Q_SLOT void onError(QAudioDecoder::Error error);
    Q_SLOT void onDecodingChanged(bool value);

private:
    QString _sourcePath;
    QAudioFormat _format;

    std::unique_ptr<QAudioDecoder> _decoder;
    QFile* _openFile = nullptr;
    std::unique_ptr<AudioBufferQIODevice> _buffer;
    std::atomic<int64_t> _currentTimestampMs;
    std::atomic<int64_t> _durationMs;
};

} // namespace core

#endif // CORE_FILE_AUDIO_SOURCE_H
