#include "mic_audio_source.h"
#include "mic_enumerator.h"

#include <QDebug>

namespace core {

MicAudioSource::MicAudioSource(QObject* parent) : AbstractAudioSource(parent)
{

}

void MicAudioSource::setMicName(const QString& name)
{
    _micName = name;
}

void MicAudioSource::setFormat(const QAudioFormat& format)
{
    _format = format;
}

QIODevice* MicAudioSource::start()
{
    if (_input) {
        _input->stop();
        _input = nullptr;
    }

    MicEnumerator micEnumerator;

    QAudioDevice device = micEnumerator.deviceByName(_micName);
    if (device.isNull()) {
        qDebug() << "Unable to find device: " << _micName;
        emit failed();
        return nullptr;
    }

    _input = std::make_unique<QAudioSource>(device, _format);

    connect(
        _input.get(), &QAudioSource::stateChanged,
        this, &MicAudioSource::handleStateChanged
    );

    return _input->start();
}

void MicAudioSource::handleStateChanged(QAudio::State newState)
{
    switch (newState) {
        case QAudio::StoppedState:
            if (_input->error() != QAudio::NoError) {
                emit failed();
            } else {
                emit stopped();
            }
            break;

        case QAudio::ActiveState:
            emit started();
            break;

        default:
            break;
    }
}

} // namespace core