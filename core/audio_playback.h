#ifndef CORE_AUDIO_PLAYBACK_H
#define CORE_AUDIO_PLAYBACK_H

#include <memory>

class QAudioFormat;
class QAudioSink;
class QIODevice;
class QThread;

namespace core {

class AudioPlayback
{
public:
    AudioPlayback();
    ~AudioPlayback();

    /// <summary>
    /// Set thread to run audio playback.
    /// Call this method before start and stop method called.
    /// Don't change it while playback is open
    /// </summary>
    /// <param name="thread"> thread must be running before start and stop methods called</param>
    void setAudioThread(QThread* thread);

    void start(const QAudioFormat& format, QIODevice* device);
    void stop();

    float currentVolume() const;

    double processedSeconds() const;

private:
    QThread* _thread = nullptr;
    std::unique_ptr<QAudioSink> _audioOutput;
};

} // namespace core

#endif // CORE_AUDIO_PLAYBACK_H
