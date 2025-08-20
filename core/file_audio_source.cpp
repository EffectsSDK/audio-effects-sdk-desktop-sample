#include "file_audio_source.h"
#include "audio_buffer_qiodevice.h"
//#include <QAudioDeviceInfo>
#include <qdebug.h>

namespace core {

FileAudioSource::FileAudioSource(QObject * parent)
    : AbstractAudioSource(parent)
    , _currentTimestampMs(0.0)
    , _durationMs(0.0)
{ }

void FileAudioSource::setSourceFilePath(const QString& path)
{
    _sourcePath = path;
}

void FileAudioSource::setFormat(const QAudioFormat& format)
{
    _format = format;
}

QIODevice* FileAudioSource::start()
{
    _currentTimestampMs = 0.0;
    _durationMs = 0.0;

    _decoder = std::make_unique<QAudioDecoder>(); // TODO: move decoder to it's own thread
    _openFile = new QFile(_sourcePath, _decoder.get());
    bool ok = _openFile->open(QFile::ReadOnly);
    if (!ok) {
        return nullptr;
    }
    _decoder->setSourceDevice(_openFile);
    if (_format.isValid()) {
        _decoder->setAudioFormat(_format);
    }

    connect(
        _decoder.get(), &QAudioDecoder::bufferReady,
        this, &FileAudioSource::onBufferReady);

    connect(
        _decoder.get(), QOverload<QAudioDecoder::Error>::of(&QAudioDecoder::error),
        this, &FileAudioSource::onError);

    connect(
        _decoder.get(), &QAudioDecoder::isDecodingChanged,
        this, &FileAudioSource::onDecodingChanged);

    _buffer = std::make_unique<AudioBufferQIODevice>();
    _buffer->moveToThread(this->thread());
    _buffer->open(QIODevice::ReadOnly);

    _decoder->start();
    return _buffer.get();
}

int FileAudioSource::durationSeconds() const
{
    int64_t durMs = _durationMs;
    if (durMs > 0.0) {
        return ceil(durMs / 1000);
    }
    else {
        return -1;
    }
}

void FileAudioSource::onBufferReady()
{
    QAudioBuffer qBuffer = _decoder->read();
    if (qBuffer.isValid()) {
        _durationMs = _decoder->duration();
        _buffer->push(qBuffer);
    }
}

void FileAudioSource::onDecodingChanged(bool value)
{
    if (value) {
        emit started();
    }
    else {
        emit stopped();
    }
}

void FileAudioSource::onError(QAudioDecoder::Error error)
{
    qWarning() << __FUNCTION__ << (int)error;
    emit failed();
}

} // namespace core