#ifndef CORE_MIC_ENUMERATOR_H
#define CORE_MIC_ENUMERATOR_H

#include <QAudioDevice>
#include <QMediaDevices>

namespace core {

class MicEnumerator
{
public:
    QStringList allMicNames() const
    {
        QList<QAudioDevice> devices = QMediaDevices::audioInputs();

        QStringList names;

        for (auto& device : devices) {
            if (!names.contains(device.description())) {
                names << device.description();
            }
        }

        return names;
    }

    QAudioDevice deviceByName(const QString& name) const
    {
        QList<QAudioDevice> devices = QMediaDevices::audioInputs();

        for (auto& device : devices) {
            if (name == device.description()) {
                return device;
            }
        }

        return QAudioDevice();
    }
};

} // namespace core

#endif // CORE_MIC_ENUMERATOR_H
