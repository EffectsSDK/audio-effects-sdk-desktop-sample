#ifndef CORE_ABSTRACT_AUDIO_SOURCE_H
#define CORE_ABSTRACT_AUDIO_SOURCE_H

#include <QObject>

namespace core {

enum class AudioSourceType
{
    File,
    Microphone
};

class AbstractAudioSource : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(AbstractAudioSource)
public:
    AbstractAudioSource(QObject * parent = nullptr);

    virtual QIODevice* start() = 0;

    virtual int durationSeconds() const {
        return -1;
    }
    
    Q_SIGNAL void started();
    Q_SIGNAL void stopped();
    Q_SIGNAL void failed();

};

} // namespace core

#endif // CORE_ABSTRACT_AUDIO_SOURCE_H
