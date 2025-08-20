#include "audio_playback.h"
#include "audio_buffer_qiodevice.h"

#include <QAudioFormat>
#include <QAudioSink>

#include <QThread>

namespace core {

AudioPlayback::AudioPlayback()
{ }

AudioPlayback::~AudioPlayback()
{
    stop();
}

void AudioPlayback::setAudioThread(QThread* thread)
{
    _thread = thread;
}

void AudioPlayback::start(const QAudioFormat& format, QIODevice* device)
{
    stop();

    QObject context;
    context.moveToThread(_thread);

    /// QAudioSink should be created and started in a separate thread so that audio processing occurs within it
    /// QAudioSink::moveToThread does not help.
    QMetaObject::invokeMethod(&context, [this, device, format] {
        _audioOutput = std::make_unique<QAudioSink>(format);
        _audioOutput->start(device);
    }, Qt::BlockingQueuedConnection);
}

void AudioPlayback::stop()
{
    if (nullptr == _audioOutput) {
        return;
    }

    QObject context;
    context.moveToThread(_thread);
    QMetaObject::invokeMethod(&context, [this] {
        _audioOutput->stop();
        _audioOutput = nullptr;
    }, Qt::BlockingQueuedConnection);
}

double AudioPlayback::processedSeconds() const
{
    if (_audioOutput) {
        return double(_audioOutput->processedUSecs()) / 1000000;
    }
    return 0;
}

} // namespace core
