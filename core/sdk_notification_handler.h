#ifndef CORE_SDK_NOTIFICATION_OBSERVER_H
#define CORE_SDK_NOTIFICATION_OBSERVER_H

#include <audio_effects_sdk/notification.h>

#include <QObject>

#include <atomic>

namespace core {

class SDKNotificationHandler: public QObject, public audio_effects_sdk::INotificationHandler
{
    Q_OBJECT
public:
    SDKNotificationHandler();

    void receive(audio_effects_sdk::INotification* notification) override;

    void addRef();
    void release() override;

signals:
    void slowProcessingDetected();

private:
    std::atomic<size_t> _refCounter;
};

} // namespace core

#endif 
