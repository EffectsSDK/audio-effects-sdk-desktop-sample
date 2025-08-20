#ifndef CORE_AUDIO_QIODEVICE_PROXY_H
#define CORE_AUDIO_QIODEVICE_PROXY_H

#include <QAudioFormat>
#include <QIODevice>

#include <memory>

namespace audio_effects_sdk { class IPipeline; }

namespace core {

class AudioQIODeviceProxy : public QIODevice
{
    Q_OBJECT
public:
    AudioQIODeviceProxy(const QAudioFormat& format, QObject* parent = nullptr);
    AudioQIODeviceProxy(const AudioQIODeviceProxy&) = delete;

    void setSourceDevice(QIODevice* device);
    void setSDKPipeline(const std::shared_ptr<audio_effects_sdk::IPipeline>& pipeline);

    bool isSequential() const override;
    qint64 bytesAvailable() const override;

    AudioQIODeviceProxy& operator=(const AudioQIODeviceProxy&) = delete;

protected:
    qint64 readData(char* data, qint64 maxLen) override;
    qint64 writeData(const char* data, qint64 len) override;

private:
    QAudioFormat _format;
    QIODevice* _sourceDevice = nullptr;
    std::shared_ptr<audio_effects_sdk::IPipeline> _pipeline;
};

} // namespace core

#endif // CORE_AUDIO_BUFFER_QIODEVICE_H
