#include "sdk_factory.h"

#include <audio_effects_sdk/sdk_factory.h>

namespace core {

static audio_effects_sdk::AudioFormatType mapQtSampleFormat(QAudioFormat::SampleFormat sampleFormat)
{
	switch (sampleFormat) {
	case QAudioFormat::SampleFormat::Float:
		return audio_effects_sdk::AudioFormatType::pcmFloat32;
	case QAudioFormat::SampleFormat::Int16:
		return audio_effects_sdk::AudioFormatType::pcmSignedInt16;

	default:
		throw std::invalid_argument("Unsupported sample format");
	}
}

SDKFactory::SDKFactory()
{
	_sdkFactory.reset(createAudioEffectsSDKFactory());
}

SDKFactory::~SDKFactory()
{
	_sdkFactory->waitUntilAsyncWorkFinished();
}

void SDKFactory::startAuth()
{
	/// Pass nullptr as calllback to authorize synchronously
	_sdkFactory->auth("CUSTOMER_ID", &SDKFactory::onAuthFinishedCallback, this);
}

bool SDKFactory::isAuthorized() const
{
	return _authorized;
}

std::shared_ptr<audio_effects_sdk::IPipeline> SDKFactory::createPipeline(const QAudioFormat& format)
{
	if (format.channelCount() != 1) {
		throw std::invalid_argument("Currently only mono sound is supported");
	}

	auto sdkFormatType = mapQtSampleFormat(format.sampleFormat());
	auto pipeline = _sdkFactory->createPipeline(
		sdkFormatType, 
		format.sampleRate(), 
		format.channelCount(), 
		0
	);
	if (nullptr == pipeline) {
		return nullptr;
	}

	return wrapSDKObject(pipeline);
}

void SDKFactory::onAuthFinished(audio_effects_sdk::IAuthResult* result)
{
	_authorized = (result->status() == audio_effects_sdk::AuthStatus::active);
	emit authorizationFinished(_authorized);
}

void SDKFactory::onAuthFinishedCallback(audio_effects_sdk::IAuthResult* result, void* self)
{
	reinterpret_cast<SDKFactory*>(self)->onAuthFinished(result);
}

} // namespace core