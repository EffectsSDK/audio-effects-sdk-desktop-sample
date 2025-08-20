#ifndef CORE_SDK_FACTORY_H
#define CORE_SDK_FACTORY_H

#include "sdk_helpers.h"

#include <QObject>
#include <QAudioFormat>

#include <memory>

namespace audio_effects_sdk {
class IAuthResult;
class ISDKFactory;
class IPipeline;
}

namespace core {

class SDKFactory: public QObject
{
    Q_OBJECT
public:
    SDKFactory();
    ~SDKFactory();

    void startAuth();
    bool isAuthorized() const;

    std::shared_ptr<audio_effects_sdk::IPipeline> createPipeline(const QAudioFormat& format);

signals:
    void authorizationFinished(bool success);

private:
    static void onAuthFinishedCallback(audio_effects_sdk::IAuthResult* result, void* self);
    void onAuthFinished(audio_effects_sdk::IAuthResult* result);

private:
    SDKUniquePtr<audio_effects_sdk::ISDKFactory> _sdkFactory;
    bool _authorized = false;
};

} // namespace core

#endif // CORE_MIC_ENUMERATOR_H
