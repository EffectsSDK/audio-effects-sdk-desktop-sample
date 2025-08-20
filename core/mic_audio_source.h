#ifndef CORE_MIC_AUDIO_SOURCE_H
#define CORE_MIC_AUDIO_SOURCE_H

#include <QAudioSource>
#include "abstract_audio_source.h"

namespace core {

class MicAudioSource : public AbstractAudioSource
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(MicAudioSource)
public:
    MicAudioSource(QObject * parent = nullptr);

    void setMicName(const QString& name);
    void setFormat(const QAudioFormat& format);

    QIODevice* start() override;

private:
    Q_SLOT void handleStateChanged(QAudio::State newState);

private:
    QString _micName;
    QAudioFormat _format;

    std::unique_ptr<QAudioSource> _input;
    std::vector<char> _readBuffer;

};

} // namespace core

#endif // CORE_MIC_AUDIO_SOURCE_H
