#include "audio_qiodevice_proxy.h"

#include <audio_effects_sdk/pipeline.h>

namespace core {

AudioQIODeviceProxy::AudioQIODeviceProxy(const QAudioFormat& format, QObject* parent)
	: QIODevice(parent)
	, _format(format)
{ }

void AudioQIODeviceProxy::setSourceDevice(QIODevice* device)
{
	if (_sourceDevice) {
		_sourceDevice->disconnect(this);
	}

	_sourceDevice = device;
	if (nullptr == _sourceDevice) {
		return;
	}

	connect(_sourceDevice, &QIODevice::readyRead, this, &QIODevice::readyRead);
}

void AudioQIODeviceProxy::setSDKPipeline(const std::shared_ptr<audio_effects_sdk::IPipeline>& pipeline)
{
	_pipeline = pipeline;
}

bool AudioQIODeviceProxy::isSequential() const
{
	if (_sourceDevice) {
		return _sourceDevice->isSequential();
	}
	return QIODevice::isSequential();
}

qint64 AudioQIODeviceProxy::bytesAvailable() const
{
	if (!_sourceDevice) {
		return 0;
		
	}
	return _sourceDevice->bytesAvailable();
}

qint64 AudioQIODeviceProxy::readData(char* data, qint64 maxLen)
{
	if (nullptr == _sourceDevice) {
		return 0;
	}

	qint64 bytesRead = _sourceDevice->read(data, maxLen);

	if (nullptr != _pipeline) {
		auto framesRead = _format.framesForBytes(bytesRead);
		_pipeline->process(
			data,
			framesRead,
			data,
			framesRead
		);
	}

	return bytesRead;
}

qint64 AudioQIODeviceProxy::writeData(const char* data, qint64 len)
{
	return 0;
}

} // namespace core