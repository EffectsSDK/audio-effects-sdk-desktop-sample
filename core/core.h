#ifndef CORE_CORE_H
#define CORE_CORE_H

#include "abstract_audio_source.h"
#include "audio_playback.h"
#include "audio_qiodevice_proxy.h"
#include "sdk_factory.h"
#include "sdk_helpers.h"
#include "sdk_notification_handler.h"

#include <QString>
#include <QThread>

#include <mutex>
#include <atomic>

namespace core {

enum class AudioFilterType
{
    None,
    NoiseSuppression_Balanced
};

class Core 
    : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(Core)
public:
    Core();
    virtual ~Core() override;

    void startAuth();

    void startAudioSource(const QString & name, AudioSourceType type);
    void setNoiseSuppressionEnabled(bool enabled);
    bool isNoiseSuppressionEnabled() const;

    QStringList allMicNames() const;

    void setCustomAudioFilename(const QString & name) {
        if (name == _customAudioFilename) return;
        _customAudioFilename = name;
        emit audioCustomFilenameChanged();
    }
    QString customAudioFilename() const {
        return _customAudioFilename;
    }

    int audioSourceDurationSec() const;
    double playbackProcessedSec() const;

    bool hasAudioSource() const {
        return _audioSource != nullptr;
    }
    
    SDKNotificationHandler* sdkNotificationHandler();

    Q_SIGNAL void audioSourceStarted();
    Q_SIGNAL void audioSourceStopped();
    Q_SIGNAL void audioSourceFailed();
    Q_SIGNAL void audioPipelineCreationFailed();
    Q_SIGNAL void audioCustomFilenameChanged();
    Q_SIGNAL void authorizationFinished(bool success);

private:
    Q_SLOT void onAudioSourceStopped();

    QAudioFormat currentAudioFormat() const;

private:
    SDKFactory _sdkFactory;
    QThread _audioThread;
    std::unique_ptr<AbstractAudioSource> _audioSource;

    std::shared_ptr<audio_effects_sdk::IPipeline> _sdkPipeline;
    std::unique_ptr<AudioQIODeviceProxy> _audioProxy;
    bool _noiseSuppressionEnabled = false;

    SDKUniquePtr<SDKNotificationHandler> _sdkNotificationHandler;

    AudioPlayback _audioPlayback;

    QString _customAudioFilename;
};

} // namespace core

#endif // CORE_CORE_H
