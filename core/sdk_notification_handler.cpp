#include "sdk_notification_handler.h"

namespace core {
SDKNotificationHandler::SDKNotificationHandler()
	: _refCounter(1)
{ }

void SDKNotificationHandler::receive(audio_effects_sdk::INotification* notification)
{
	if (audio_effects_sdk::NotificationCode::slowProcessing == notification->code()) {
		emit slowProcessingDetected();
	}
}

void SDKNotificationHandler::addRef()
{
	_refCounter++;
}

void SDKNotificationHandler::release()
{
	_refCounter--;
	if (0 == _refCounter) {
		delete this;
	}
}

}