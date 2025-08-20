#include "core.h"
#include "file_audio_source.h"
#include "mic_audio_source.h"
#include "mic_enumerator.h"

#include <audio_effects_sdk/pipeline.h>

namespace core {

Core::Core()
{
    connect(
        &_sdkFactory, &SDKFactory::authorizationFinished,
        this, &Core::authorizationFinished
    );
    _audioPlayback.setAudioThread(&_audioThread);

    _sdkNotificationHandler.reset(new SDKNotificationHandler);
}

Core::~Core()
{
    _audioPlayback.stop();
    _audioProxy = nullptr;
    _audioSource = nullptr;
    _audioThread.quit();
    _audioThread.wait();
}

void Core::startAuth()
{
    _sdkFactory.startAuth();
}

void Core::startAudioSource(const QString& name, AudioSourceType type)
{
    auto audioFormat = currentAudioFormat();
    auto sdkPipeline = _sdkFactory.createPipeline(audioFormat);
    if (nullptr == sdkPipeline) {
        emit audioPipelineCreationFailed();
        return;
    }
    _sdkNotificationHandler->addRef();
    sdkPipeline->setNotificationHandler(_sdkNotificationHandler.get());
    sdkPipeline->setNoiseSuppressionEnabled(_noiseSuppressionEnabled);

    {
        _audioPlayback.stop();
        if (type == AudioSourceType::File) {
            auto fileAudioSource = std::make_unique<FileAudioSource>();
            fileAudioSource->setSourceFilePath(name);
            fileAudioSource->setFormat(currentAudioFormat());
            _audioSource = std::move(fileAudioSource);
        } else if (type == AudioSourceType::Microphone) {
            auto micAudioSource = std::make_unique<MicAudioSource>();
            micAudioSource->setMicName(name);
            micAudioSource->setFormat(currentAudioFormat());
            _audioSource = std::move(micAudioSource);
        }

        _audioSource->moveToThread(&_audioThread);

        _audioProxy.reset(new AudioQIODeviceProxy(audioFormat));
        _audioProxy->moveToThread(&_audioThread);
        _audioProxy->setSDKPipeline(sdkPipeline);
        _sdkPipeline = sdkPipeline;

        if (!_audioThread.isRunning()) {
            _audioThread.start(QThread::HighestPriority);
        }
    }

    connect(
        _audioSource.get(), &AbstractAudioSource::started,
        this, &Core::audioSourceStarted,
        Qt::QueuedConnection
    );

    connect(
        _audioSource.get(), &AbstractAudioSource::stopped,
        this, &Core::onAudioSourceStopped,
        Qt::QueuedConnection
    );

    connect(
        _audioSource.get(), &AbstractAudioSource::failed,
        this, &Core::audioSourceFailed,
        Qt::QueuedConnection
    );

    QIODevice* device = _audioSource->start();
    if (nullptr == device) {
        emit audioSourceFailed();
        return;
    }
    _audioProxy->setSourceDevice(device);
    _audioProxy->open(QIODevice::ReadOnly);
    _audioPlayback.start(currentAudioFormat(), _audioProxy.get());
}

void Core::setNoiseSuppressionEnabled(bool enabled)
{
    _noiseSuppressionEnabled = enabled;
    if (nullptr != _sdkPipeline) {
        _sdkPipeline->setNoiseSuppressionEnabled(enabled);
    }
}

bool Core::isNoiseSuppressionEnabled() const
{
    return _noiseSuppressionEnabled;
}

QStringList Core::allMicNames() const
{
    MicEnumerator enumerator;
    return enumerator.allMicNames();
}

int Core::audioSourceDurationSec() const
{
    if (_audioSource) {
        return _audioSource->durationSeconds();
    } else {
        return -1;
    }
}

double Core::playbackProcessedSec() const
{
    return _audioPlayback.processedSeconds();
}

SDKNotificationHandler* Core::sdkNotificationHandler()
{
    return _sdkNotificationHandler.get();
}

void Core::onAudioSourceStopped()
{
    emit audioSourceStopped();
}

QAudioFormat Core::currentAudioFormat() const
{
    QAudioFormat format;
    format.setSampleRate(48000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::SampleFormat::Float);
    return format;
}

} // namespace core
